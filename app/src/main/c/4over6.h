//
// Created by Harry Chen on 2019/5/1.
//

#ifndef INC_4OVER6_ANDROID_4OVER6_H
#define INC_4OVER6_ANDROID_4OVER6_H

#include <stdint.h>

extern int socket_fd;
extern char ip[20], route[20], dns1[20], dns2[20], dns3[20];

typedef struct {
  uint32_t length;
  uint8_t type;
  uint8_t data[4096];
} message_t;

int establish_connection(const char *addr, int port);
int request_configuration();

#define HEADER_LEN (sizeof(uint32_t) + sizeof(uint8_t))

#endif //INC_4OVER6_ANDROID_4OVER6_H
