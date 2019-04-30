#include <jni.h>
#include <stdlib.h>

#include "4over6.h"

int socket_fd = -1;

JNIEXPORT jboolean JNICALL
Java_xyz_harrychen_thu4over6_MainActivity_establishConnection(
    JNIEnv *env, jobject _this, jobject server_info) {

    jclass info_cls = (*env)->GetObjectClass(env, server_info);

    jfieldID addr_id =
        (*env)->GetFieldID(env, info_cls, "addr", "Ljava/lang/String;");
    jstring jaddr = (*env)->GetObjectField(env, server_info, addr_id);
    const char *addr_s = (*env)->GetStringUTFChars(env, jaddr, NULL);

    jfieldID port_id =
        (*env)->GetFieldID(env, info_cls, "port", "Ljava/lang/String;");
    jstring jport = (*env)->GetObjectField(env, server_info, port_id);
    const char *port_s = (*env)->GetStringUTFChars(env, jport, NULL);
    int port = atoi(port_s);

    int result = establish_connection(addr_s, port);

    if (result < 0)
        return (jboolean)0;

    return (jboolean)1;
}

JNIEXPORT jobject JNICALL
Java_xyz_harrychen_thu4over6_MainActivity_requestConfiguration(JNIEnv *env,
                                                               jobject _this) {

    int result = request_configuration();

    if (result < 0)
        return NULL;

    jclass vpn_cls = (*env)->FindClass(env, "xyz.harychen.thu4over6.bean.VpnInfo");
    jmethodID vpn_ctor = (*env)->GetMethodID(env, vpn_cls, "<init>", "void(V)");
    jobject vpn_obj = (*env)->NewObject(env, vpn_cls, vpn_ctor);

    jfieldID id;
    jstring js;

    id = (*env)->GetFieldID(env, vpn_cls, "ipv4", "Ljava/lang/String;");
    js = (*env)->NewStringUTF(env, ip);
    (*env)->SetObjectField(env, vpn_cls, id, js);

    id = (*env)->GetFieldID(env, vpn_cls, "route", "Ljava/lang/String;");
    js = (*env)->NewStringUTF(env, route);
    (*env)->SetObjectField(env, vpn_cls, id, js);

    id = (*env)->GetFieldID(env, vpn_cls, "dns1", "Ljava/lang/String;");
    js = (*env)->NewStringUTF(env, dns1);
    (*env)->SetObjectField(env, vpn_cls, id, js);

    id = (*env)->GetFieldID(env, vpn_cls, "dns2", "Ljava/lang/String;");
    js = (*env)->NewStringUTF(env, dns2);
    (*env)->SetObjectField(env, vpn_cls, id, js);

    id = (*env)->GetFieldID(env, vpn_cls, "dns3", "Ljava/lang/String;");
    js = (*env)->NewStringUTF(env, dns3);
    (*env)->SetObjectField(env, vpn_cls, id, js);

    return vpn_obj;

}