#include "test1.h"

#include "openHttp/OpenHttp.h"
#include "lib/Base64.h"
#include "lib/LibSha1.h"

void test1001() {
	std::cout << "This is test1." << std::endl;
//	Log((char*) "openInitialize");
	std::cout << "OpenHttp is initializing." << std::endl;
	OpenHttp * openHttp = OpenHttp::getInstance();
	openHttp->initialize();
	std::cout << "OpenHttp is initialized." << std::endl;
}
int i = 0;

void onRequest(HttpServerSession* httpSession) {

	Message* message = new Message();
	JSString jsString;
	jsString + "{\"text\":\"hello" + i + "}";

	message->data = jsString.char_string;
	message->contentLength = strlen(message->data);
	message->headerMap = new JSON();
	message->headerMap->initialize();
	message->headerMap->set((char *) "Content-Type", (char *) "application/json; charset=UTF-8");

	std::cout << "sendResponse" << std::endl;
	httpSession->sendResponse(message);
	i++;
}

void onRequestFile(HttpServerSession* httpSession) {
	std::cout << "onRequest:" << i << std::endl;
	i++;

	Message* message = new Message();
	message->sendFilePath = new JSString("/alidata/swift/web/123.jpg");
	message->sendFileStart = 0;
	message->contentLength = 30675 - 0;

	message->headerMap = new JSON();
	message->headerMap->initialize();
	message->headerMap->set("Content-Type", "image/jpeg");

	std::cout << "sendResponse" << std::endl;
	httpSession->sendResponse(message);
}
void onRequestApk(HttpServerSession* httpSession) {
	std::cout << "onRequest:" << i << std::endl;
	i++;

	Message* message = new Message();
	message->sendFilePath = new JSString("/alidata/swift/web/wxsq2.2.apk");
	message->sendFileStart = 0;
	message->contentLength = 6684541;

	message->headerMap = new JSON();
	message->headerMap->initialize();
	message->headerMap->set("Content-Type", "application/vnd.android.package-archive");

	std::cout << "sendResponse" << std::endl;
	httpSession->sendResponse(message);
}

void onMessage(HttpServerSession* httpSession) {
	char * frameStr = "['command':'log','data':'abc']";
//	httpSession->sendMessageFrame(frameStr);
}

void test1002() {

	int a = 112;
	int b = 23123 + a;
	std::cout << b;
	std::cout << "OpenHttp is initializing." << std::endl;
	OpenHttp * openHttp = OpenHttp::getInstance();
	openHttp->initialize();
	std::cout << "OpenHttp is initialized." << std::endl;

	HttpServer * httpserver = new HttpServer();
	httpserver->localPort = 8091;

	openHttp->createServer(httpserver);
//	httpserver->onRequest = onRequest;
//	httpserver->onMessage = onMessage;
	int i = 0;
	while (true) {
//		std::cout << "OpenHttp is running@" << i << std::endl;
		sleep(10);
		i += 10;
	}
}
//sN9cRrP/n9NdMgdcy2VJFQ==
//fFBooB7FAkLlXgRSz0BT3v4hq5s=
//S0VwmOM+KraApujmHm5bE0Efc9k=
void test1003() {
	char * key = "sN9cRrP/n9NdMgdcy2VJFQ==";
	int key_length = strlen(key);
	char * mask = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	int mask_length = strlen(mask);
	char * value = JSMalloc(SHA1_HASH_SIZE * 2);

	char * mergedKey = JSMalloc(key_length + mask_length + 1);
	memcpy(mergedKey, key, key_length);
	memcpy(mergedKey + key_length, mask, mask_length);

	char * sha1_result = sha1(mergedKey, key_length + mask_length);

	base64_encode(value, (char *) sha1_result, SHA1_HASH_SIZE);

	std::cout << value << std::endl;

}
timeval * iTimeval;
timeval * start_iTimeval;

long getCurrentMillisecond() {
	gettimeofday(iTimeval, NULL);

	long millisecond = (iTimeval->tv_sec - start_iTimeval->tv_sec) * 1000 + (iTimeval->tv_usec - start_iTimeval->tv_usec) / 1000;
	return millisecond;
}

void onResponce1(HttpServerSession* httpSession) {
	std::cout << "onResponce1:" << i << std::endl;
	i++;
	std::cout << "server says:" << httpSession->receiveDataBuffer << std::endl;
	std::cout << "onResponce :" << i << ":" << getCurrentMillisecond() << "\r\n";
}
void onResponce2(HttpServerSession* httpSession) {
	JSLog(httpSession->receiveDataBuffer);
}
void onResponce3(HttpServerSession* httpSession) {
	std::cout << "onResponce3:" << i << std::endl;
	i++;
	std::cout << "server says:" << httpSession->received_ContentLength << std::endl;
	std::cout << "server says  1:" << httpSession->received_HeaderLength << std::endl;
}

void onResponce(HttpClientSession* httpSession) {
	std::cout << "onResponce:" << i << std::endl;
	i++;
	std::cout << "server says:" << httpSession->receiveDataBuffer << std::endl;

	std::cout << "onResponce 1" << ":" << getCurrentMillisecond() << "\r\n";

	Message* message1 = new Message();
	message1->urlPath = "/api2/bug/send?aa=22222";
	message1->method = "GET";
//	message->data = "{\"text\":\"hello\"}";
	message1->data = "";
	message1->contentLength = strlen(message1->data);
	message1->headerMap = new JSON();
	message1->headerMap->initialize();
	message1->headerMap->set("Host", "192.168.0.2");
	message1->headerMap->set("Content-Type", "application/json; charset=UTF-8");

	httpSession->httpCallback = new HttpCallback();
	((HttpCallback *) (httpSession->httpCallback))->onResponce = &(onResponce1);

	httpSession->sendRequest(message1);

	Message* message2 = new Message();
	message2->urlPath = "/api2/bug/send?aa=33333";
	message2->method = "GET";
//	message->data = "{\"text\":\"hello\"}";
	message2->data = "";
	message2->contentLength = strlen(message2->data);
	message2->headerMap = new JSON();
	message2->headerMap->initialize();
	message2->headerMap->set("Host", "192.168.0.2");
	message2->headerMap->set("Content-Type", "application/json; charset=UTF-8");
	httpSession->sendRequest(message2);

	Message* message3 = new Message();
	message3->urlPath = "/api2/bug/send?aa=44444";
	message3->method = "GET";
//	message->data = "{\"text\":\"hello\"}";
	message3->data = "";
	message3->contentLength = strlen(message1->data);
	message3->headerMap = new JSON();
	message3->headerMap->initialize();
	message3->headerMap->set("Host", "192.168.0.2");
	message3->headerMap->set("Content-Type", "application/json; charset=UTF-8");
	httpSession->sendRequest(message3);
}

void test1004() {
	start_iTimeval = new timeval();
	iTimeval = new timeval();
	gettimeofday(start_iTimeval, NULL);
	std::cout << "start time:" << getCurrentMillisecond() << "\r\n";
	int a = 112;
	int b = 23123 + a;
	std::cout << b;
	std::cout << "OpenHttp is initializing." << std::endl;
	OpenHttp * openHttp = OpenHttp::getInstance();
	openHttp->initialize();
	std::cout << "OpenHttp is initialized." << std::endl;

	HttpClient * httpClient = new HttpClient();

	httpClient->remotePort = 80;
	httpClient->remoteIP = new JSString("192.168.0.2");

	openHttp->createClient(httpClient);

	httpClient->httpCallback = new HttpCallback();
	httpClient->httpCallback->onResponce = &(onResponce);

	std::cout << "createClient time:" << getCurrentMillisecond() << "\r\n";

	Message* message = new Message();
	message->urlPath = "/api2/bug/send?aa=11111";
	message->method = "GET";
//	message->data = "{\"text\":\"hello\"}";
	message->data = "";
	message->contentLength = strlen(message->data);
	message->headerMap = new JSON();
	message->headerMap->initialize();
	message->headerMap->set("Host", "192.168.0.2");
	message->headerMap->set("Content-Type", "application/json; charset=UTF-8");

	httpClient->sendRequest(message);
	std::cout << "sendRequest time:" << getCurrentMillisecond() << "\r\n";

	int i = 0;
	while (true) {
//		std::cout << "OpenHttp is running@" << i << std::endl;
		sleep(10);
		i += 10;
	}
}
void test100611111() {
	int a = 112;
	int b = 23123 + a;
	std::cout << b;
	std::cout << "OpenHttp is initializing." << std::endl;
	OpenHttp * openHttp = OpenHttp::getInstance();
	openHttp->initialize();
	std::cout << "OpenHttp is initialized." << std::endl;

	HttpClient * httpClient = new HttpClient();

	httpClient->remotePort = 80;
	httpClient->remoteIP = new JSString("192.168.0.2");

	openHttp->createClient(httpClient);

	httpClient->httpCallback = new HttpCallback();
	httpClient->httpCallback->onResponce = &(onResponce2);

	Message* message = new Message();
	message->urlPath = "/test/test.jpg?uploads";
	message->method = "POST";
	message->data = "Expires=1431331583&OSSAccessKeyId=dpZe5yUof6KSJ8RM&Signature=/PZJF3BupzY8IU+jAGeypiJWITI=";
	message->contentLength = strlen(message->data);
	message->headerMap = new JSON();
	message->headerMap->initialize();
	message->headerMap->set("Host", "images2.we-links.com");
	message->headerMap->set("Content-Type", "application/json; charset=UTF-8");

	httpClient->sendRequest(message);

	int i = 0;
	while (true) {
//		std::cout << "OpenHttp is running@" << i << std::endl;
		sleep(10);
		i += 10;
	}
}

void test1007_out() {
	int a = 112;
	int b = 23123 + a;
	std::cout << b;
	std::cout << "OpenHttp is initializing." << std::endl;
	OpenHttp * openHttp = OpenHttp::getInstance();
	openHttp->initialize();
	std::cout << "OpenHttp is initialized." << std::endl;

	HttpClient * httpClient = new HttpClient();

	httpClient->remotePort = 80;
	httpClient->remoteIP = new JSString("192.168.0.2");

	openHttp->createClient(httpClient);

	httpClient->httpCallback = new HttpCallback();
	httpClient->httpCallback->onResponce = &(onResponce3);

	Message* message = new Message();
//	message->receiveFilePath = "/alidata/web.zip";
	message->urlPath = "/web.zip";
	message->method = "GET";
//	message->data = "{\"text\":\"hello\"}";
	message->data = "";
	message->contentLength = strlen(message->data);
	message->headerMap = new JSON();
	message->headerMap->initialize();
	message->headerMap->set("Host", "192.168.0.2");
	//message->headerMap->set("Content-Type", "application/octet/stream");

//	httpClient->receiveFD = open("/alidata/node.msi", O_CREAT | O_RDWR, 777);
	httpClient->sendRequest(message);

	int i = 0;
	while (true) {
//		std::cout << "OpenHttp is running@" << i << std::endl;
		sleep(10);
		i += 10;
	}
}

void test1007() {
	std::cout << "OpenHttp is initializing." << std::endl;
	OpenHttp * openHttp = OpenHttp::getInstance();
	openHttp->initialize();
	std::cout << "OpenHttp is initialized." << std::endl;

	HttpClient * httpClient = new HttpClient();

	httpClient->remotePort = 8071;
	httpClient->remoteIP = new JSString("192.168.0.2");

	openHttp->createClient(httpClient);
	httpClient->httpCallback = new HttpCallback();
	httpClient->httpCallback->onResponce = &(onResponce3);

	Message* message = new Message();
//	message->receiveFilePath = "/alidata/web.zip";
	message->urlPath = "/v/add?a=456&b=789";
	message->method = "POST";
	message->data = "";
	message->contentLength = strlen(message->data);
	message->headerMap = new JSON();
	message->headerMap->initialize();
	message->headerMap->set("Host", "192.168.0.2");
	httpClient->sendRequest(message);

	int i = 0;
	while (true) {
//		std::cout << "OpenHttp is running@" << i << std::endl;
		sleep(10);
		i += 10;
	}
}

void test1005() {
	start_iTimeval = new timeval();
	iTimeval = new timeval();
	gettimeofday(start_iTimeval, NULL);
	std::cout << "iTimeval->tv_usec1:" << iTimeval->tv_usec / 1000 << "\r\n";
	std::cout << "start time:" << getCurrentMillisecond() << "\r\n";
	std::cout << "iTimeval->tv_usec1:" << iTimeval->tv_usec / 1000 << "\r\n";
}

void test1008() {

	float a = 0.1;
	int b = 200640;
	int c = 262144;

	a = (float) b / (float) c;
	std::cout << a;

}

void traceLog(char * string) {

	int length = strlen(string);

	int receiveFD = open("/alidata/test/test10001.txt", O_CREAT | O_RDWR, 0777);

	if (receiveFD < 0) {
		return;
	}
	struct stat sb;
	fstat(receiveFD, &sb);

	int fileLength = sb.st_size;

	if (fileLength == 0) {
		ftruncate(receiveFD, 1);
	} else {
		ftruncate(receiveFD, fileLength + length);
	}

	void *receiveFileBuffer = mmap(NULL, fileLength + length, PROT_READ | PROT_WRITE, MAP_SHARED, receiveFD, 0);

	ftruncate(receiveFD, fileLength + length);

	memcpy(receiveFileBuffer + fileLength, string, length);

	munmap(receiveFileBuffer, length);
	close(receiveFD);
}

void test1009() {
	char * string = "I am in the sub process.\r\n";
	traceLog(string);

}

void logJSString(JSString * jsString) {
	std::cout << jsString->char_string << "\r\n";
}

void test1012() {
	JSString * jsString = new JSString();
	*jsString = "Hello String";

//	*jsString = *jsString + "  we are added!" + "  add more!" + -366553364;
	*jsString + " add 3!";
//	*jsString + 366555444;

	JSString * jsString1 = new JSString();
	*jsString1 = " i am string1 ";
//	*jsString + *jsString1;
	logJSString(jsString);

//	JSString1 * jsString2 = "I am string2";
}

void test1011() {
	WebsocketLog * myLog = WebsocketLog::getInstance();
	myLog->initialize();
	int i = 0;
	while (true) {
//		std::cout << "OpenHttp is running@" << i << std::endl;
		sleep(10);
		i += 10;
	}
}

#include <string>
#include<iostream>
void test1013() {
//	std::string strinfo = "Please input your name:";

	JSString jsString = "Please input your name:";

//	JSString1 * jsString1 = &jsString;
//
//	jsString1 = &(*jsString1 + "wsds");
//	logJSString(jsString1);
//	*jsString1 + 1;
//	logJSString(jsString1);
//
	jsString + "wsds" + 1;
	logJSString(&jsString);

	if (jsString == "wsds") {
		std::cout << "same as wsds" << "\r\n";
	}

	if (jsString == "Please input your name:wsds1") {
		std::cout << "same as Please input your name:wsds1" << "\r\n";
	}
	jsString = 35565;
	if (jsString == "35565") {
		std::cout << "same as \"35565\"" << "\r\n";
	}

	if (jsString == 35565) {
		std::cout << "same as 35565" << "\r\n";
	}
}

void test1010() {

	JSString jsString;
	jsString + "Please input your name:**@355653556535565355653556535565355653556535565@** and your name is **@@**";
	jsString % "wsds66955558";
	jsString % "wsds888";
	logJSString(&jsString);

	JSString jsString1;
	jsString1 * "Please input your name:**@153@** and your name is **@@**" % "wsds+++++" % "wsds888888";
	logJSString(&jsString1);

	JSString jsString2;
	jsString2 * "test number:**@153@** and your name is **@@**ooo" * "we are here:" % 13555888 % "wsds888@@888" * 88888 % "abc";
	jsString2 * "ppp" + 666999;
	logJSString(&jsString2);

}

void logviaCout(JSString & jsString) {
	std::cout << jsString.char_string << std::endl;
}

void test1014() {
//

	WebsocketLog * websocketLog = WebsocketLog::getInstance();
	websocketLog->initialize();

	flag_cout = true;
	log2cout = logviaCout;

	sleep(2);
	JSString jsString1;
	jsString1 * "Please input your name:**@153@** and your name is **@@**" % "wsds+++++" % "wsds888888";
	JSLog(jsString1);

	int i = 1;
	while (true) {
		sleep(2);

		jsString1 = "this is 2s,  *@@* times";
		JSLog(jsString1 % i);
		i += 10;
	}
}

void onFailure_test1015(ErrorMessage * errorMessage) {
	JSString * jsStringReturn = new JSString(128);
	(*jsStringReturn) = "{\"type\":@1@,\"clientID\":@2@,\"stateCode\":@3@,\"errorMessage\":\"@4@\"}";

	if (errorMessage->stateCode < 600) {
		(*jsStringReturn) % -1 % errorMessage->clientID % errorMessage->stateCode % *(errorMessage->errorMessage);

	} else if (errorMessage->stateCode > 600) {
		(*jsStringReturn) % -2 % errorMessage->clientID % errorMessage->stateCode % *(errorMessage->errorMessage);
	}
	JSString jsString = (char *) "[onFailure]\r\n";
	jsString + (*jsStringReturn);
	JSLog(jsString);

}

void sendAPITest(int i) {
	OpenHttp * openHttp = OpenHttp::getInstance();
	HttpClient * httpClient = new HttpClient();

	JSString * urlPath = new JSString();
	(*urlPath) + "/api2/bug/send?aa=" + i;

	httpClient->remotePort = 9092;
	httpClient->remoteIP = new JSString("192.168.1.4");
	httpClient->clientID = i + 10000;
	openHttp->createClient(httpClient);

	httpClient->httpCallback = new HttpCallback();
	httpClient->httpCallback->onResponce = &(onResponce2);
	httpClient->httpCallback->onFailure = &(onFailure_test1015);

	Message* message = new Message();
	message->urlPath = urlPath->char_string;
	message->method = "GET";
	//	message->data = "{\"text\":\"hello\"}";
	message->data = "";
	message->contentLength = strlen(message->data);
	message->headerMap = new JSON();
	message->headerMap->initialize();
	message->headerMap->set("Host", "192.168.1.4");
	message->headerMap->set("Content-Type", "application/json; charset=UTF-8");

	httpClient->sendRequest(message);
}

void test1015() {

	WebsocketLog * websocketLog = WebsocketLog::getInstance();
	websocketLog->initialize();

	flag_cout = true;
	log2cout = logviaCout;

	sleep(2);
	for (int i = 0; i < 1500; i++) {
		sendAPITest(i);
	}

	int i = 0;
	while (true) {
//		std::cout << "OpenHttp is running@" << i << std::endl;
		sleep(10);
		i += 10;
	}
}
//static int test_sum;
//static pthread_t * testMemPthread;
//int index_memory;
//void testMemLooper() {
//	while (true) {
//
//	}
//}
//void * testMemThread(void *arg) {
//	testMemLooper();
//	return (void *) 1;
//}
//
//void initializeTestMem()) {
////	testMemEnv = env;
////
////	testMemQueue = new Queue();
////	testMemQueue->initialize();
//	test_sum = 0;
//	index_memory = 1;
//	testMemPthread = new pthread_t();
//	int error = pthread_create(testMemPthread, NULL, testMemThread, (void *) 1);
//}

void test1016() {
	WebsocketLog * websocketLog = WebsocketLog::getInstance();
	websocketLog->initialize();

	flag_cout = true;
	log2cout = logviaCout;

	sleep(2);
	for (int i = 0; i < 1500; i++) {
		if (i % 10 == 0) {
			usleep(1000 * 10);
		}
		sendAPITest(i);
	}

	int i = 0;
	while (true) {
//		std::cout << "OpenHttp is running@" << i << std::endl;
		sleep(10);
		i += 10;
	}
}
void test1017() {
	OpenHttp * openHttp = OpenHttp::getInstance();
	openHttp->initialize();
	HttpClient * httpClient = new HttpClient();
	WebsocketLog * websocketLog = WebsocketLog::getInstance();
	websocketLog->initialize();

	flag_cout = true;
	log2cout = logviaCout;

	JSString * urlPath = new JSString();
	(*urlPath) + "/api2/relation/getfans?phone=15210721344&accessKey=lejoying";

	httpClient->remotePort = 80;
	httpClient->remoteIP = new JSString("112.126.71.180");
	httpClient->clientID = 10;
	openHttp->createClient(httpClient);

	httpClient->httpCallback = new HttpCallback();
	httpClient->httpCallback->onResponce = &(onResponce2);
	httpClient->httpCallback->onFailure = &(onFailure_test1015);

	Message* message = new Message();
	message->urlPath = urlPath->char_string;
	message->method = "GET";
	//	message->data = "{\"text\":\"hello\"}";
	message->data = "";
	message->contentLength = strlen(message->data);
	message->headerMap = new JSON();
	message->headerMap->initialize();
	message->headerMap->set("Host", "112.126.71.180");
	message->headerMap->set("Content-Type", "application/json; charset=UTF-8");

	httpClient->sendRequest(message);

	int i = 0;
	while (true) {
//		std::cout << "OpenHttp is running@" << i << std::endl;
		sleep(10);
		i += 10;
	}
}

