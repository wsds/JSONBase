//#include "test4.h"
//
//void logviaCout4(JSString & jsString) {
//	char * char_string = jsString.char_string;
//	int length = getLength(char_string);
//	if (length != jsString.length) {
//		char_string = JSMalloc(jsString.length);
//		obverse_memcpy(char_string, jsString.char_string, jsString.length);
//		std::cout << char_string << std::endl;
//		JSFree(char_string);
//	} else {
//		std::cout << char_string << std::endl;
//
//	}
//}
//
//MainProcess * mainProcess;
//SubProcess * subProcess;
//
//void foldProcess6() {
//	int maxRetry = 5;
//	while (maxRetry > 0) {
//		maxRetry--;
//		int sub_pid = fork();
//
//		if (sub_pid < 0) {
//		} else if (sub_pid > 0) {
//			int errorNumber = errno;
//			if (errorNumber != EAGAIN) {
//				mainProcess->sub_pid = sub_pid;
//				mainProcess->send_heart_beat_to_pid = sub_pid;
//				mainProcess->linkSubProcess();
//				mainProcess->initialize();
//				break;
//			}
//		} else {
//			subProcess->linkMainProcess();
//			subProcess->initialize();
//			break;
//		}
//	}
//}
//
//int times = 0;
//void mainProcessCommand(JSString * jsString) {
////	JSLog("ProcessCommand");
//
//	const char * json_str1 = "[times:132,b:'ProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandhello']";
//	JSON * json = parseJSON(json_str1);
//
//	for (int i = 0; i < 10; i++) {
//		times++;
//		(*json)["times"] = times;
//		mainProcess->callSubProcess(json);
//	}
//	delete (json);
//}
//void subProcessCommand(JSString * jsString) {
////	JSLog("ProcessCommand");
//
//	const char * json_str1 = "[times:132,b:'ProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandhello']";
//	JSON * json = parseJSON(json_str1);
//
//	for (int i = 0; i < 2; i++) {
//		times++;
//		(*json)["times"] = times;
//		subProcess->callMainProcess(json);
//	}
//	delete (json);
//}
//
//void onStart_mainProcess() {
//	WebsocketLog * websocketLog = WebsocketLog::getInstance();
//	websocketLog->initialize();
//	websocketLog->callSubProcessCommand = &mainProcessCommand;
//	sleep(2);
//	JSLog("onStart_mainProcess");
//}
//
//void onStart_subProcess() {
//	WebsocketLog * websocketLog = WebsocketLog::getInstance();
//	websocketLog->localPort = 8092;
//	websocketLog->initialize();
//	websocketLog->callSubProcessCommand = &subProcessCommand;
//	sleep(2);
//	JSLog("onStart_subProcess");
//}
//int called_times = 0;
//void onCall(JSON * json) {
//	JSLog(json);
//	called_times++;
//}
//
//void onLostHeartBeat(int heart_beat) {
//	JSLog("onLostHeartBeat");
//	JSLog(heart_beat);
//}
//
//void test4006() {
//	flag_cout = true;
//	log2cout = logviaCout4;
//	JSLog("test4006");
//
//	Space * swap_space = newSpace();
//
//	mainProcess = MainProcess::getInstance();
//	mainProcess->swap_space = swap_space;
//	mainProcess->processCallback->onStart = &onStart_mainProcess;
//	mainProcess->processCallback->onCall = &onCall;
//
//	subProcess = SubProcess::getInstance();
//	subProcess->parent_pid = getpid();
//	subProcess->swap_space = swap_space;
//	subProcess->processCallback->onStart = &onStart_subProcess;
//	subProcess->processCallback->onCall = &onCall;
//	subProcess->processCallback->onLostHeartBeat = &onLostHeartBeat;
//
//	foldProcess6();
//}
//
//void test4007() {
//	flag_cout = true;
//	log2cout = logviaCout4;
//	JSLog("test4007");
//
//	LocalStorage * localStorage = createLocalStorage(NULL);
//
//	JSString key1 = "hello123";
//	JSString value1 = "987643145464aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
//	localStorage->savaString(&key1, &value1);
//
//	JSString key2 = "asdsad";
//	JSString value2 = "123456bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb132456";
//	localStorage->savaString(&key2, &value2);
//
//	JSString * value101 = localStorage->loadString(&key1);
//	JSLog(value101);
//
//	JSString * value102 = localStorage->loadString(&key2);
//	JSLog(value102);
//
//	JSString * value3 = new JSString(20000);
//	memset(value3->char_string, 'w', 20000);
//	value3->length = 20000;
//	JSString key3 = "long_string1";
//
//	localStorage->savaString(&key3, value3);
//
//	JSString * value103 = localStorage->loadString(&key3);
////	JSLog(value101);
//
//	localStorage->closeMMap();
//
//	int i = 0;
//	i++;
//
//}
//
//void test4008() {
//
//	flag_cout = true;
//	log2cout = logviaCout4;
//	JSLog("test4008");
//
//	LocalStorage * localStorage = createLocalStorage(NULL);
//
//	JSString key1 = "hello123";
//
//	JSString key2 = "asdsad";
//
//	JSString * value101 = localStorage->loadString(&key1);
//	JSLog(value101);
//
//	JSString * value102 = localStorage->loadString(&key2);
//	JSLog(value102);
//
//	JSString key3 = "long_string1";
//	JSString * value103 = localStorage->loadString(&key3);
//
//	int i = 0;
//	i++;
//}
//
//void test4009(){
//	int size1=sizeof(MySignalHead);
//	int size2=sizeof(MySignal);
//
//	int i = 0;
//	i++;
//}
//
void test4006(){

}
void test4007(){

}

void test4008(){

}

void test4009(){

}


