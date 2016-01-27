#include <jni.h>

#include "../openHttp/OpenHttp.h"
#include "../lib/WebSocketLog.h"
#include "../data_core/JSON.h"

#include <signal.h>
#include <setjmp.h>

#ifndef _Included_Test
#define _Included_Test
#ifdef __cplusplus
extern "C" {
#endif
int JNI_OnLoad(JavaVM *vm, void *reserved);
void onResponse_JNI(HttpClientSession* httpSession);
void sendRequestJNI(JNIEnv * env, jobject obj, jobject request);

JNIEXPORT void Java_com_open_test_openhttp_MyHttpJNI_openInitialize(JNIEnv *, jobject, jobject myHttpJNI);
JNIEXPORT void Java_com_open_test_openhttp_MyHttpJNI_jsLog(JNIEnv *, jobject, jstring data);

JNIEXPORT void Java_com_open_test_openhttp_MyHttpJNI_sendRequest(JNIEnv *, jobject, jobject request);
JNIEXPORT jobject Java_com_open_test_openhttp_MyHttpJNI_getResponse(JNIEnv *, jobject);
JNIEXPORT jobject Java_com_open_test_openhttp_MyHttpJNI_getErrorData(JNIEnv *, jobject);
JNIEXPORT jobject Java_com_open_test_openhttp_MyHttpJNI_getProgressData(JNIEnv *, jobject);

JNIEXPORT float Java_com_open_test_openhttp_MyHttpJNI_sendProgress(JNIEnv * env, jobject obj, jint clientID);
JNIEXPORT void Java_com_open_test_openhttp_MyHttpJNI_set(JNIEnv * env, jobject obj, jint max_connecting_Client, jint default_timeout);

JNIEXPORT void Java_com_open_test_openhttp_MyHttpJNI_closeFile(JNIEnv * env, jobject, jstring filePath);
JNIEXPORT void Java_com_open_test_openhttp_MyHttpJNI_abortRequest(JNIEnv * env, jobject obj, jint clientID);
JNIEXPORT void Java_com_open_test_openhttp_MyHttpJNI_threadHolder(JNIEnv * env, jobject obj);
_jmethodID * GetClassMethodID(JNIEnv* env);
_jmethodID * GetClassMethodIDLog(JNIEnv* env);
_jmethodID * GetClassMethodIDCommand(JNIEnv* env);
void JavaLog(JSString * jsString);

void testException();

#ifdef __cplusplus
}

class HttpResponse {
public:
	~HttpResponse();
	int clientID;
	int stateCode;
	JSString* header;
	JSString* body;
};

class HttpProgress {
public:
	int clientID;
	double sendProgress;
	double receiveProgress;
};

#endif
#endif

