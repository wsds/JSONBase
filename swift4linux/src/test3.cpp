#include "test3.h"

void logviaCout3(JSString & jsString) {
	std::cout << jsString.char_string << std::endl;
}
void test3001() {
	JSON * json = new JSON();
	json->initialize();

	JSKeyValue * jskeyValue = new JSKeyValue();
	JSNumber * jsNumber = new JSNumber();
	jsNumber->number = 99;
	jskeyValue->key = "error";
	jskeyValue->value = jsNumber;
	json->push(jskeyValue);

	JSString * jsString = new JSString("qiaoxiaosongssssssssssssssssssssssssssssssssssssssssssssssqiaoxiaosongssssssssssssssssssssssssssssssssssssssssssssss");

	JSKeyValue * jsKeyValue2 = new JSKeyValue();
	jsKeyValue2->key = "nickName";
	jsKeyValue2->value = jsString;
	json->push(jsKeyValue2);

	flag_cout = true;
	log2cout = logviaCout3;

	char * jsonString = stringifyJSON(json);

	JSLog(jsonString);

	JSLog("----------end");
}

class TestC {
public:
	int i;
	int j;
};
class Exception {
};

bool if_SIGSEGV = false;
jmp_buf buff123;

void sigroutine(int dunno) {

	switch (dunno) {

	case SIGSEGV:

//		printf("Get a signal -- SIGSEGV  ");
//		std::cout << "sigroutine 1111" << std::endl;
//		Exception exception;
//		throw exception;
//		if_SIGSEGV = true;
//		int i = 1213;
//		char * test = JSMalloc(64);
//		char test1[64] = "hello";
		longjmp(buff123, 1);
		break;

	}

	return;
}

void test3002() {

	signal(SIGSEGV, sigroutine);

	JSString jsString1 = "hello SIGSEGV   old in heap";

	char test1[64] = "hello SIGSEGV   old in stack";

	try {
		std::cout << "test3002.1" << std::endl;
		TestC * testc = NULL;
		std::cout << "test3002.2" << std::endl;

		if (!setjmp(buff123)) {
			std::cout << "test3002.3" << std::endl;
			int i = 1;
			i++;
			//Trying to dereference a null pointer will cause a segmentation fault,
			//which is handled by our magic_handler now.
			testc->i = 1;

		} else {
			std::cout << "test3002.4" << std::endl;
			int i = 1;
			i++;
			std::cout << "Successfully recovered! Welcome back in main!" << std::endl;

			JSString jsString = "hello SIGSEGV";

			std::cout << jsString.char_string << std::endl;

			std::cout << jsString1.char_string << std::endl;
			std::cout << test1 << std::endl;

			Exception exception;
			throw exception;
		}

		std::cout << "test3002.5" << std::endl;
	} catch (...) {
		std::cout << "test3002.6" << std::endl;
	};
}

void test3003() {
	struct sigaction new_sigaction;
	struct sigaction old_sigaction;


    sigemptyset(&new_sigaction.sa_mask);
    new_sigaction.sa_handler=sigroutine;
    new_sigaction.sa_flags=0;


	int sigaction(SIGSEGV, &new_sigaction, &old_sigaction);
}
//void testException() {
//	try {
//		JSLog("sendRequestJNI 1000");
//		if (!setjmp(jump_buffer_null_pointer)) {
//			OpenHttp * openHttp1 = NULL;
//
//			openHttp1->httpServerMap->is_initialized = false;
//			JSLog("sendRequestJNI 1002.1");
//		} else {
//			JSLog("sendRequestJNI 1003");
////			NullPointerException nullPointerException;
////			throw nullPointerException;
//		}
//	} catch (...) {
//		JSLog("testException");
//	};
//}


static int MAX_NUMBER_STRING_SIZE = 32;
void test3004(){
	 int i;
	    char s[MAX_NUMBER_STRING_SIZE];
	    double d[] = {
	        0.0,
	        42.0,
	        1234567.89012345,
	        0.000000000000018,
	        555555.55555555555555555,
	        -888888888888888.8888888,
	        111111111111111111111111.2222222222,
	        56.456865
	    };
	    for (i = 0; i < 8; i++) {
	        printf("%d: printf: %.14g, dtoa: %s\n", i+1, d[i], Double2CharStr(s, d[i]));
	    }


}
