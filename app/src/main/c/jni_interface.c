#include <jni.h>
#include <stdlib.h>

#include "4over6.h"

JNIEXPORT jboolean JNICALL
Java_xyz_harrychen_thu4over6_MainActivity_establishConnection(JNIEnv *env, jobject _this, jstring jaddr, jstring jport) {

    const char *addr_s = (*env)->GetStringUTFChars(env, jaddr, NULL);
    const char *port_s = (*env)->GetStringUTFChars(env, jport, NULL);

    int port = atoi(port_s);
    int result = establish_connection(addr_s, port);

    if (result < 0) {
        return (jboolean)0;
    }

    return (jboolean)1;
}


JNIEXPORT void JNICALL
Java_xyz_harrychen_thu4over6_MainActivity_tearupConnection(JNIEnv *env, jobject _this) {
    tearup_connection();
}


JNIEXPORT void JNICALL
Java_xyz_harrychen_thu4over6_MainActivity_setupTun(JNIEnv *env, jobject _this, jint tun_fd) {
    setup_tun(tun_fd);
}


JNIEXPORT jobject JNICALL
Java_xyz_harrychen_thu4over6_MainActivity_requestConfiguration(JNIEnv *env, jobject _this, jobject obj) {

    int result = request_configuration();

    if (result < 0) {
        return NULL;
    }

    jclass cls = (*env)->GetObjectClass(env, obj);
    // jclass cls = (*env)->FindClass(env, "xyz/harrychen/thu4over6/bean/VpnInfo");
    // jmethodID vpn_ctor = (*env)->GetMethodID(env, cls, "<init>", "(V)");
    // jobject obj = (*env)->NewObject(env, cls, vpn_ctor);

    jfieldID id;
    jstring js;

    id = (*env)->GetFieldID(env, cls, "ipv4", "Ljava/lang/String;");
    js = (*env)->NewStringUTF(env, ip);
    (*env)->SetObjectField(env, obj, id, js);

    id = (*env)->GetFieldID(env, cls, "route", "Ljava/lang/String;");
    js = (*env)->NewStringUTF(env, route);
    (*env)->SetObjectField(env, obj, id, js);

    id = (*env)->GetFieldID(env, cls, "dns1", "Ljava/lang/String;");
    js = (*env)->NewStringUTF(env, dns1);
    (*env)->SetObjectField(env, obj, id, js);

    id = (*env)->GetFieldID(env, cls, "dns2", "Ljava/lang/String;");
    js = (*env)->NewStringUTF(env, dns2);
    (*env)->SetObjectField(env, obj, id, js);

    id = (*env)->GetFieldID(env, cls, "dns3", "Ljava/lang/String;");
    js = (*env)->NewStringUTF(env, dns3);
    (*env)->SetObjectField(env, obj, id, js);

    id = (*env)->GetFieldID(env, cls, "socketFd", "I");
    (*env)->SetIntField(env, obj, id, (jint) socket_fd);

    return obj;

}


JNIEXPORT jobject JNICALL
Java_xyz_harrychen_thu4over6_MainActivity_getStatistics(JNIEnv *env, jobject _this, jobject obj) {

    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID id;

    id = (*env)->GetFieldID(env, cls, "state", "Z");
    (*env)->SetBooleanField(env, obj, id, (jboolean) socket_fd != -1);
    id = (*env)->GetFieldID(env, cls, "uploadTotalPkt", "I");
    (*env)->SetIntField(env, obj, id, (jint) out_pkt);
    id = (*env)->GetFieldID(env, cls, "uploadTotalByte", "I");
    (*env)->SetIntField(env, obj, id, (jint) out_byte);
    id = (*env)->GetFieldID(env, cls, "downloadTotalPkt", "I");
    (*env)->SetIntField(env, obj, id, (jint) in_pkt);
    id = (*env)->GetFieldID(env, cls, "downloadTotalByte", "I");
    (*env)->SetIntField(env, obj, id, (jint) in_byte);

    return obj;
}