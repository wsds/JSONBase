#include "libImageJni.h"

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT int Java_com_open_test_openhttp_MyHttpJNI_test1(JNIEnv *, jobject) {

	return 6666666;
}

JNIEXPORT jstring Java_com_open_test_openhttp_MyHttpJNI_resolveImage(JNIEnv * env, jobject, jstring json_jstring) {
	const char* json_char = env->GetStringUTFChars(json_jstring, 0);
	JSON* image_info = parseJSON((char*) json_char);
	env->ReleaseStringUTFChars(json_jstring, json_char);

//	char * data = (char *) "{\"fileName\":\"8EC09EFD0A8EFFDFF368CAA07648F3128D88887E.osp\",\"fileLength\":173186}";

	JSON * result = resolveImage(image_info);
	char * result_str = stringifyJSON(result);
	jstring rtstr = env->NewStringUTF(result_str);
	return rtstr;
}

#ifdef __cplusplus
}
#endif
