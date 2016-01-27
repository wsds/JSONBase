#include "libOpenHttpJni.h"

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

static JavaVM *g_jvm = NULL;
JavaVM * callback_jvm;
_jobject * callback_object;
_jmethodID * callback_method;
_jmethodID * callback_method_log;
_jmethodID * callback_method_command;
JSString * jsStringReturn = NULL;
jmp_buf jump_buffer_null_pointer;
struct sigaction new_sigaction;
struct sigaction old_sigaction;

void (*oldHandler)(int);
void sigroutineJNI(int signal) {
	if (signal == SIGSEGV) {
		longjmp(jump_buffer_null_pointer, 1);
	}
}

int JNI_OnLoad(JavaVM *vm, void *reserved) {
	g_jvm = vm;
	callback_jvm = vm;
	JNIEnv* env = NULL;
	jint result = -1;

	if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
		return -1;
	}
	if (env == NULL) {
		return -1;
	}

	result = JNI_VERSION_1_6;
	return result;
}

Queue * httpResponseQueue;
Queue * errorMessageQueue;
Queue * progressQueue;

void onResponse_JNI(HttpClientSession* httpSession) {

	JNIEnv * env = NULL;
	if (callback_jvm->AttachCurrentThread((JNIEnv **) &env, NULL) != JNI_OK) {
	} else {
	}
	if (callback_jvm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
		return;
	}
	if (env == NULL) {
		return;
	}

	Message * receive_message = httpSession->receive_message;

	HttpResponse * httpResponse = new HttpResponse();
	httpResponse->clientID = httpSession->clientID;
	httpResponse->header = new JSString();
	httpResponse->header->copy(receive_message->header, receive_message->headerLength);
	httpResponse->body = new JSString();
	httpResponse->body->copy(httpSession->receiveDataBuffer, receive_message->contentLength);
	httpResponse->stateCode = parseStringToNubmer(receive_message->stateCode, 4);
	httpResponseQueue->offer(httpResponse);

	env->CallVoidMethod(callback_object, callback_method);
}

void onFailure_JNI(ErrorMessage * errorMessage) {

	JNIEnv * env = NULL;
	if (callback_jvm->AttachCurrentThread((JNIEnv **) &env, NULL) != JNI_OK) {
	} else {
	}
	if (callback_jvm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
		return;
	}
	if (env == NULL) {
		return;
	}
	errorMessageQueue->offer(errorMessage);

	env->CallVoidMethod(callback_object, callback_method);
}

void onUpdateState_JNI(HttpClientSession* httpSession) {

	JNIEnv * env = NULL;
	if (callback_jvm->AttachCurrentThread((JNIEnv **) &env, NULL) != JNI_OK) {
	} else {
	}
	if (callback_jvm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
		return;
	}
	if (env == NULL) {
		return;
	}
	HttpProgress * httpProgress = new HttpProgress();
	httpProgress->clientID = httpSession->clientID;
	httpProgress->receiveProgress = httpSession->receiveProgress;
	httpProgress->sendProgress = httpSession->sendProgress;

	progressQueue->offer(httpProgress);

	env->CallVoidMethod(callback_object, callback_method);
}

JNIEXPORT void Java_com_open_test_openhttp_MyHttpJNI_set(JNIEnv * env, jobject obj, jint max_connecting_Client, jint default_timeout) {
	OpenHttp * openHttp = OpenHttp::getInstance();
	OpenHttpHelper * helper = openHttp->helper;
	helper->max_connecting_Client = max_connecting_Client;
	helper->default_timeout = default_timeout;
}

JNIEXPORT void Java_com_open_test_openhttp_MyHttpJNI_closeFile(JNIEnv * env, jobject, jstring filePath) {
	int filePathlength = env->GetStringUTFLength(filePath);
	const char * filePath_buffer = env->GetStringUTFChars(filePath, 0);

	JSString * filePath_string = new JSString(0);
	filePath_string->char_string = (char *) filePath_buffer;
	filePath_string->length = filePathlength;

	OpenHttp * openHttp = OpenHttp::getInstance();
	openHttp->closeFile(filePath_string);

	env->ReleaseStringUTFChars(filePath, filePath_buffer);
}

JNIEXPORT void Java_com_open_test_openhttp_MyHttpJNI_jsLog(JNIEnv * env, jobject, jstring data) {

	int datalength = env->GetStringUTFLength(data);
	const char * data_buffer = env->GetStringUTFChars(data, 0);

	JSString * jsString = new JSString(0);
	jsString->char_string = (char *) data_buffer;
	jsString->length = datalength;

	JSLog(jsString);

	env->ReleaseStringUTFChars(data, data_buffer);
}

void JavaLog(JSString * jsString) {

	JNIEnv * env = NULL;
	if (callback_jvm->AttachCurrentThread((JNIEnv **) &env, NULL) != JNI_OK) {
//		Log((char *) "Failed");
	} else {
	}
	if (callback_jvm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
		return;
	}
	if (env == NULL) {
		return;
	}
	JSString jsStringReturn;
	jbyteArray body = env->NewByteArray(jsString->length);
	env->SetByteArrayRegion(body, 0, jsString->length, (const signed char *) jsString->char_string);

	env->CallVoidMethod(callback_object, callback_method_log, body);

	env->DeleteLocalRef(body);

}
void logviaJava(JSString & jsString) {
	JavaLog(&jsString);
}

void callJNICommand(JSString * jsString) {
	JNIEnv * env = NULL;
	if (callback_jvm->AttachCurrentThread((JNIEnv **) &env, NULL) != JNI_OK) {
	} else {
	}
	if (callback_jvm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
		return;
	}
	if (env == NULL) {
		return;
	}

	jbyteArray commmandBytes = env->NewByteArray(jsString->length);
	env->SetByteArrayRegion(commmandBytes, 0, jsString->length, (signed char *) jsString->char_string);

	env->CallVoidMethod(callback_object, callback_method_command, commmandBytes);

	env->DeleteLocalRef(commmandBytes);
}

int test10006_i;

JNIEXPORT void Java_com_open_test_openhttp_MyHttpJNI_openInitialize(JNIEnv * env, jobject obj, jobject myHttpJNI) {
	env->GetJavaVM(&g_jvm);
	callback_object = env->NewGlobalRef(myHttpJNI);
	callback_method = GetClassMethodID(env);
	callback_method_log = GetClassMethodIDLog(env);
	callback_method_command = GetClassMethodIDCommand(env);
	JSLog("OpenHttp is initializing.");

	httpResponseQueue = new Queue();
	httpResponseQueue->initialize();

	errorMessageQueue = new Queue();
	errorMessageQueue->initialize();

	progressQueue = new Queue();
	progressQueue->initialize();

	OpenHttp * openHttp = OpenHttp::getInstance();
	openHttp->initialize();
	JSLog("OpenHttp is initialized.");

	WebsocketLog * websocketLog = WebsocketLog::getInstance();
	websocketLog->initialize();
	websocketLog->callJNICommand = &callJNICommand;

	jsStringReturn = new JSString(128);

	flag_Java = true;
	log2Java = &logviaJava;

	test10006_i = 600000;
}

JNIEXPORT void Java_com_open_test_openhttp_MyHttpJNI_threadHolder(JNIEnv * env, jobject obj) {
	JSLog("threadHolder in");
	int i = 0;
	while (true) {
		sleep(10);
		i += 10;
	}
	JSLog("threadHolder out");
}
void onResponce2(HttpServerSession* httpSession) {
	JSLog("onResponce2");
	JSLog(httpSession->receiveDataBuffer);
}

JNIEXPORT float Java_com_open_test_openhttp_MyHttpJNI_sendProgress(JNIEnv * env, jobject obj, jint clientID) {
	OpenHttp * openHttp = OpenHttp::getInstance();
	float sendProgress = openHttp->sendProgress(clientID);

	return sendProgress;
}

JNIEXPORT void Java_com_open_test_openhttp_MyHttpJNI_abortRequest(JNIEnv * env, jobject obj, jint clientID) {

	OpenHttp * openHttp = OpenHttp::getInstance();

	HttpClient * httpClient = (HttpClient *) openHttp->clientID_httpClientMap->get(clientID);
	if (httpClient != NULL) {
		httpClient->needAbort = true;
		HttpSession * httpSession = httpClient->httpClientSession;
		if (httpSession != NULL) {
			httpSession->needAbort = true;
		}
	}
}

class NullPointerException {

};

void onResponce_raw(HttpServerSession* httpSession) {
	JSLog(httpSession->receiveDataBuffer);
}
jstring getJstringField(const char * field_name, jclass j_class, jobject o_object, JNIEnv * env) {
	jfieldID nameFieldId;
	nameFieldId = env->GetFieldID(j_class, field_name, "Ljava/lang/String;");
	if (nameFieldId == 0) {
		return NULL;
	}
	jstring javaNameStr = (jstring) env->GetObjectField(o_object, nameFieldId);
	return javaNameStr;
}

void setJstringField(const char * field_name, jclass j_class, jobject j_object, JNIEnv * env, JSString * js_field) {
	jfieldID nameFieldId;
	nameFieldId = env->GetFieldID(j_class, field_name, "Ljava/lang/String;");

	jstring j_field = env->NewStringUTF(js_field->char_string);
	env->SetObjectField(j_object, nameFieldId, j_field);
}

void setJintField(const char * field_name, jclass j_class, jobject j_object, JNIEnv * env, int number) {
	jfieldID nameFieldId;
	nameFieldId = env->GetFieldID(j_class, field_name, "I");
	env->SetIntField(j_object, nameFieldId, number);
}

void setJdoubleField(const char * field_name, jclass j_class, jobject j_object, JNIEnv * env, double number) {
	jfieldID nameFieldId;
	nameFieldId = env->GetFieldID(j_class, field_name, "D");
	env->SetDoubleField(j_object, nameFieldId, number);
}

int getJintField(const char * field_name, jclass j_class, jobject j_object, JNIEnv * env) {
	jfieldID nameFieldId;
	nameFieldId = env->GetFieldID(j_class, field_name, "I");
	if (nameFieldId == NULL) {
		return 0;
	}
	jint number = env->GetIntField(j_object, nameFieldId);
	return number;
}

long getJlongField(const char * field_name, jclass j_class, jobject j_object, JNIEnv * env) {
	jfieldID nameFieldId;
	nameFieldId = env->GetFieldID(j_class, field_name, "J");
	if (nameFieldId == NULL) {
		return 0;
	}
	jlong number = env->GetLongField(j_object, nameFieldId);
	return number;
}
JSString * getJSStringFromJclass(const char * field_name, jclass j_class, jobject j_object, JNIEnv * env) {

	jstring j_field = getJstringField(field_name, j_class, j_object, env);

	if (j_field == NULL) {
		return NULL;
	}
	int datalength = env->GetStringUTFLength(j_field);
	const char * data_buffer = env->GetStringUTFChars(j_field, 0);

	if (datalength <= 0) {
		return NULL;
	}

	JSString * js_field = new JSString(datalength);
	js_field->copy((char *) data_buffer, datalength);

	env->ReleaseStringUTFChars(j_field, data_buffer);
	return js_field;
}

void sendRequestJNI(JNIEnv * env, jobject obj, jobject request) {

	jclass j_class = env->GetObjectClass(request);

	OpenHttp * openHttp = OpenHttp::getInstance();

	HttpClient * httpClient = new HttpClient();

	httpClient->remotePort = getJintField("port", j_class, request, env);

	JSString * js_remoteIP = getJSStringFromJclass("ip", j_class, request, env);
	httpClient->remoteIP = js_remoteIP;

	httpClient->clientID = getJintField("clientID", j_class, request, env);

	int timeout = getJintField("timeout", j_class, request, env);
	if (timeout != -1) {
		httpClient->timeout = timeout;
	}

	httpClient->httpCallback = new HttpCallback();
	httpClient->httpCallback->onResponce = &(onResponse_JNI);
	httpClient->httpCallback->onFailure = &(onFailure_JNI);
	httpClient->httpCallback->onUpdateState = &(onUpdateState_JNI);
	openHttp->createClient(httpClient);

	Message* message = new Message();

	JSString * js_urlPath = getJSStringFromJclass("urlPath", j_class, request, env);
	message->urlPath = js_urlPath->char_string;

	JSString * js_method = getJSStringFromJclass("method", j_class, request, env);
	message->method = js_method->char_string;

	JSString * js_followingHeader = getJSStringFromJclass("header", j_class, request, env);
	message->followingHeader = js_followingHeader->char_string;

	JSString * js_sendfilePath = getJSStringFromJclass("sendfilePath", j_class, request, env);

	if (js_sendfilePath != NULL) {
		message->sendFilePath = js_sendfilePath;

		message->sendFileStart = getJlongField("sendFileStart", j_class, request, env);
		message->sendFileLength = getJlongField("sendFileLength", j_class, request, env);

		message->contentLength = message->sendFileLength;

	} else {
		JSString * js_body = getJSStringFromJclass("body", j_class, request, env);
		if (js_body == NULL) {
			message->data = NULL;
			message->contentLength = 0;
		} else {
			message->data = js_body->char_string;
			message->contentLength = js_body->length;
		}
	}

	JSString * js_receivefilePath = getJSStringFromJclass("receivefilePath", j_class, request, env);

	if (js_receivefilePath != NULL) {
		message->receiveFilePath = js_receivefilePath;
	}

	httpClient->sendRequest(message);
}

HttpResponse::~HttpResponse() {
	delete (this->body);
	delete (this->header);
}

JNIEXPORT jobject Java_com_open_test_openhttp_MyHttpJNI_getResponse(JNIEnv * env, jobject) {
	HttpResponse * httpResponse = (HttpResponse *) httpResponseQueue->take();
	if (httpResponse == NULL) {
		return NULL;
	}

	jclass j_class = env->FindClass("com/open/test/openhttp/data/HttpResponseData");
	jmethodID m_mid = env->GetMethodID(j_class, "<init>", "()V");
	jobject j_object = env->NewObject(j_class, m_mid);

	setJstringField("header", j_class, j_object, env, httpResponse->header);
	setJstringField("body", j_class, j_object, env, httpResponse->body);
	setJintField("clientID", j_class, j_object, env, httpResponse->clientID);
	setJintField("stateCode", j_class, j_object, env, httpResponse->stateCode);

	delete (httpResponse);
	return j_object;
}
JNIEXPORT jobject Java_com_open_test_openhttp_MyHttpJNI_getErrorData(JNIEnv * env, jobject) {
	ErrorMessage * errorMessage = (ErrorMessage *) errorMessageQueue->take();
	if (errorMessage == NULL) {
		return NULL;
	}

	jclass j_class = env->FindClass("com/open/test/openhttp/data/HttpErrorData");
	jmethodID m_mid = env->GetMethodID(j_class, "<init>", "()V");
	jobject j_object = env->NewObject(j_class, m_mid);

	setJstringField("errorMessage", j_class, j_object, env, errorMessage->errorMessage);
	setJintField("stateCode", j_class, j_object, env, errorMessage->stateCode);
	setJintField("clientID", j_class, j_object, env, errorMessage->clientID);

	delete (errorMessage);
	return j_object;
}

JNIEXPORT jobject Java_com_open_test_openhttp_MyHttpJNI_getProgressData(JNIEnv * env, jobject) {
	HttpProgress * httpProgress = (HttpProgress *) progressQueue->take();
	if (httpProgress == NULL) {
		return NULL;
	}

	jclass j_class = env->FindClass("com/open/test/openhttp/data/HttpProgressData");
	jmethodID m_mid = env->GetMethodID(j_class, "<init>", "()V");
	jobject j_object = env->NewObject(j_class, m_mid);

	setJdoubleField("sendProgress", j_class, j_object, env, httpProgress->sendProgress);
	setJdoubleField("receiveProgress", j_class, j_object, env, httpProgress->receiveProgress);
	setJintField("clientID", j_class, j_object, env, httpProgress->clientID);

	delete (httpProgress);
	return j_object;
}

JNIEXPORT void Java_com_open_test_openhttp_MyHttpJNI_sendRequest(JNIEnv * env, jobject obj, jobject request) {
	sendRequestJNI(env, obj, request);
//	try {
//		sigemptyset(&new_sigaction.sa_mask);
//		new_sigaction.sa_handler = sigroutineJNI;
//		new_sigaction.sa_flags = SA_RESTART;
//		sigaction(SIGSEGV, &new_sigaction, &old_sigaction);
//		if (!setjmp(jump_buffer_null_pointer)) {
//			sendRequestJNI(env, obj, request);
//
////			test10006_i++;
////			sendAPITest(test10006_i);
//
//		} else {
//			NullPointerException nullPointerException;
//			throw nullPointerException;
//		}
//		sigaction(SIGSEGV, &old_sigaction, &new_sigaction);
//	} catch (...) {
//		jclass newExcCls = env->FindClass("com/open/test/openhttp/JNIException");
//		env->ThrowNew(newExcCls, "thrown from C code");
//		sigaction(SIGSEGV, &old_sigaction, &new_sigaction);
//	};
}

_jmethodID * GetClassMethodIDLog(JNIEnv* env) {
	jclass clazz = env->FindClass("com/open/test/openhttp/MyHttpJNI");
	if (clazz == NULL) {
		return NULL;
	}

	_jmethodID * jcallback = env->GetMethodID(clazz, "onLog", "([B)V"); //"(I[BIF)V"
	if (jcallback == NULL) {
		return NULL;
	}

	return jcallback;
}
_jmethodID * GetClassMethodIDCommand(JNIEnv* env) {
	jclass clazz = env->FindClass("com/open/test/openhttp/MyHttpJNI");
	if (clazz == NULL) {
		return NULL;
	}

	_jmethodID * jcallback = env->GetMethodID(clazz, "command", "([B)V"); //"(I[BIF)V"
	if (jcallback == NULL) {
		return NULL;
	}

	return jcallback;
}
_jmethodID * GetClassMethodID(JNIEnv* env) {
	jclass clazz = env->FindClass("com/open/test/openhttp/MyHttpJNI");
	if (clazz == NULL) {
		return NULL;
	}

	_jmethodID * jcallback = env->GetMethodID(clazz, "callback", "()V"); //"(I[BIF)V"
	if (jcallback == NULL) {
		return NULL;
	}

	return jcallback;
}
#ifdef __cplusplus
}
#endif
