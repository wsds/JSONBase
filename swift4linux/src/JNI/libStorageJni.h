#include <jni.h>

#include "../storage/Storage.h"
#include "../data_mmap/DMString.h"
#include "../lib/WebSocketLog.h"
#ifndef _Included_Test
#define _Included_Test
#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void Java_com_open_test_openhttp_Storage_initialize(JNIEnv * env, jobject, jstring f, int length);
JNIEXPORT jstring Java_com_open_test_openhttp_Storage_loadString(JNIEnv *, jobject, jstring key);

JNIEXPORT void Java_com_open_test_openhttp_Storage_savaString(JNIEnv *, jobject, jstring key, jstring data);

#ifdef __cplusplus
}
#endif
#endif

