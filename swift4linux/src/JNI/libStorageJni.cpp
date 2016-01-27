#include "libStorageJni.h"

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void Java_com_open_test_openhttp_Storage_initialize(JNIEnv * env, jobject, jstring f, int length) {
	const char * f_char = env->GetStringUTFChars(f, (jboolean*) 0);
	setMomoryFolderPath((char *) f_char, length);
}

JNIEXPORT jstring Java_com_open_test_openhttp_Storage_loadString(JNIEnv * env, jobject, jstring key) {
	const char* key_char = env->GetStringUTFChars(key, 0);
	int key_length = getLength1((char*) key_char);

	LocalStorage * localStorage = LocalStorage::getInstance();

	DMString * key_dm_string = newDMString(key_length);
	reverse_memcpy1(key_dm_string->char_string, (void *) key_char, key_length);
	env->ReleaseStringUTFChars(key, key_char);

	DMString * data_dm_string = localStorage->loadString(key_dm_string);

	freeDMString(key_dm_string);

	if (data_dm_string == NULL) {
		return NULL;
	}

	char * result_str = data_dm_string->char_string;
	jstring rtstr = env->NewStringUTF(result_str);

	return rtstr;
}

JNIEXPORT void Java_com_open_test_openhttp_Storage_savaString(JNIEnv * env, jobject, jstring key, jstring data) {
	const char* key_char = env->GetStringUTFChars(key, 0);
	int key_length = getLength1((char*) key_char);
	const char* data_char = env->GetStringUTFChars(data, 0);
	int data_length = getLength1((char*) data_char);

	DMString * key_dm_string = newDMString(key_length);
	reverse_memcpy1(key_dm_string->char_string, (void *) key_char, key_length);
	DMString * value_dm_string = newDMString(data_length + 1); //the last '\0' is add to fit the JNI call of env->NewStringUTF(result_str);
	reverse_memcpy1(value_dm_string->char_string, (void *) data_char, data_length);

	env->ReleaseStringUTFChars(key, key_char);
	env->ReleaseStringUTFChars(data, data_char);

	LocalStorage * localStorage = LocalStorage::getInstance();
	localStorage->savaString(key_dm_string, value_dm_string);

}

#ifdef __cplusplus
}
#endif
