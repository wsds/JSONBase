#include <jni.h>

#include <signal.h>
#include <setjmp.h>

#include "../libImage/libImage.h"

#include "../data_core/JSON.h"

#ifndef _Included_Test
#define _Included_Test
#ifdef __cplusplus
extern "C" {
#endif
JNIEXPORT int Java_com_open_test_openhttp_MyHttpJNI_test1(JNIEnv *, jobject);

JNIEXPORT jstring Java_com_open_test_openhttp_MyHttpJNI_ResolveImage(JNIEnv *, jobject, jstring json);
#ifdef __cplusplus
}
#endif
#endif

