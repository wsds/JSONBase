#include "test_swift.h"

void logviaCout106(JSString & jsString) {
	char * char_string = jsString.char_string;
	int length = getLength(char_string);
	if (length != jsString.length) {
		char_string = JSMalloc(jsString.length);
		obverse_memcpy(char_string, jsString.char_string, jsString.length);
		std::cout << char_string << std::endl;
		JSFree(char_string);
	} else {
		std::cout << char_string << std::endl;

	}
}

void test106001() {
	WebsocketLog * websocketLog = WebsocketLog::getInstance();
	websocketLog->initialize();

	flag_cout = true;
	log2cout = logviaCout106;

	JSLog("test6001");

////	interpret_main();
//
//	JSString * codelineStr = new JSString();
//
//	(*codelineStr) = "var n=12241";
//
//	CodeLine* codeLine = resolveCodeLine(codelineStr);
//
//	Context * context = new Context();
//
//	Executable* executable = analyzeCodeLine(codeLine, context);
//
//	Closure * rootClosure = new Closure();
//	rootClosure->initialize();

	int i = 0;
	while (true) {
		sleep(10);
	}
}
