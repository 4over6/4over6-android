cmake_minimum_required(VERSION 3.4.1)


add_library(4over6 SHARED
             src/main/c/jni_interface.c
             src/main/c/4over6.h
             src/main/c/4over6.c
             )


find_library(log-lib log)


target_link_libraries(4over6 ${log-lib})