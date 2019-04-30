//
// Created by Harry Chen on 2019/5/1.
//

#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/socket.h>

#define TAG "lib4over6"

#include "log.h"
#include "4over6.h"


pthread_cond_t config_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t config_mutex = PTHREAD_MUTEX_INITIALIZER;

char ip[20], route[20], dns1[20], dns2[20], dns3[20];
volatile int received_configuration = 0;

pthread_t receive_pid, timer_pid;

uint8_t *read_exact(int fd, size_t len) {
    static size_t pos = 0;
    static size_t end = 0;
    const static size_t buffer_size = 40960;
    static uint8_t buffer[buffer_size];
    if (pos + len >= buffer_size) {
        // not enough size, put back to beginning
        memmove(buffer, buffer + pos, end - pos);
        end = end - pos;
        pos = 0;
    }

    uint8_t *result = buffer + pos;
    uint8_t *ptr = buffer + end;
    while (ptr < buffer + pos + len) {
        ssize_t read_bytes = read(fd, ptr, buffer_size - end);
        if (read_bytes < 0) {
            if (read_bytes == -1 && errno == EAGAIN) {
                continue;
            }
            LOGE("read from socket: %s", strerror(errno));
            assert(false);
        } else {
            ptr += read_bytes;
            end += read_bytes;
        }
    }

    pos += len;
    return result;
}

void *receive_thread(void *args) {

    uint8_t buffer[4096];
    //   struct tun_pi *pi = (struct tun_pi *)buffer;
    //   pi->flags = 0;
    //   pi->proto = htons(ETH_P_IP);

    LOGI("Reading thread starts");

    while (1) {
        message_t *msg = (message_t *)read_exact(socket_fd, HEADER_LEN);
        uint32_t msg_length = msg->length;
        uint8_t msg_type = msg->type;

        size_t len = msg_length - HEADER_LEN;
        if (len > 0) {
            uint8_t *body = read_exact(socket_fd, len);
            if (msg_type == 103) {
                // TODO: copy to tun
            } else if (msg_type == 101) {
                // configuration from server
                char buffer[1024];
                memcpy(buffer, body, len);
                buffer[len] = '\0';
                sscanf(buffer, "%s %s %s %s %s", ip, route, dns1, dns2, dns3);
                // wake up sleeping thread
                pthread_mutex_lock(&config_mutex);
                received_configuration = 1;
                pthread_cond_signal(&config_cond);
                pthread_mutex_unlock(&config_mutex);
            } else {
                // unknown type
                LOGE("Unrecognised msg type: %d\n", msg_type);
            }
        }
    }
    return NULL;
}

void *timer_thread(void *args) {
    LOGI("Timer thread starts");
}

int establish_connection(const char *addr_s, int port) {

    struct sockaddr_in6 addr;
    socket_fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        LOGE("error creating socket: %s", strerror(errno));
        return -1;
    }
    LOGI("IPv6 TCP socket created");
    bzero(&addr, sizeof(struct sockaddr_in6));
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(port);

    inet_pton(AF_INET6, addr_s, (struct sockaddr *)&addr.sin6_addr);
    if (connect(socket_fd, (struct sockaddr *)&addr,
                sizeof(struct sockaddr_in6)) < 0) {
        LOGE("failed to connect to server [%s]:%d: %s", addr_s, port, strerror(errno));
        return -1;
    }

    LOGI("Connected to server %s", addr_s);

    // thread for handling incoming messages
    pthread_create(&receive_pid, NULL, receive_thread, NULL);
    // thread for managing keepalive messages
    pthread_create(&timer_pid, NULL, timer_thread, NULL);

    return 0;
}

int request_configuration() {

    // send request
    message_t ip_request = {.length = HEADER_LEN, .type = 100};
    if (write(socket_fd, &ip_request, ip_request.length) < 0) {
        LOGE("failed to send connection request: %s", strerror(errno));
        return -1;
    }

    LOGI("IP request sent");

    // wait for configuration
    pthread_mutex_lock(&config_mutex);
    while (received_configuration == 0) {
        pthread_cond_wait(&config_cond, &config_mutex);
    }
    pthread_mutex_unlock(&config_mutex);

    LOGI("IP request received");

    return 0;

}