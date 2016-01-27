#include "test2.h"

void onResponce2001(HttpServerSession* httpSession) {
	JSString jsString = "[200 ok]\r\n";
	JSLog(jsString + httpSession->receiveDataBuffer);
}

void onFailure2001(HttpServerSession* httpSession) {
	JSString jsString = "[onFailure]\r\n";
	JSLog(jsString + httpSession->receiveBuffer);
}

void logviaCout2(JSString & jsString) {
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

void test2001() {
	OpenHttp * openHttp = OpenHttp::getInstance();
	openHttp->initialize();

	WebsocketLog * websocketLog = WebsocketLog::getInstance();
	websocketLog->initialize();

	flag_cout = true;
	log2cout = logviaCout2;

	sleep(5);
	JSLog("test2001");

	HttpClient * httpClient = new HttpClient();

	httpClient->remotePort = 80;
	httpClient->remoteIP = new JSString("192.168.0.4");

	openHttp->createClient(httpClient);

	httpClient->httpCallback = new HttpCallback();
	httpClient->httpCallback->onResponce = &(onResponce2001);
	httpClient->httpCallback->onFailure = &(onFailure2001);

	Message* message = new Message();
	message->urlPath = "/";
	message->method = "GET";
	message->data = "";
	message->contentLength = strlen(message->data);
	message->headerMap = new JSON();
	message->headerMap->initialize();

	(*message->headerMap)["Host"] = "192.168.0.4";

	httpClient->sendRequest(message);

	int i = 0;
	while (true) {
		sleep(10);
//		std::cout << i += 10;
	}
}
void test2002() {

	WebsocketLog * websocketLog = WebsocketLog::getInstance();
	websocketLog->initialize();

	flag_cout = true;
	log2cout = logviaCout2;

	char * JSONStr = "[\"clientID\":\"abc\"]";

	JSLog(JSONStr);
	JSON* requestData_JSON = parseJSON((char*) JSONStr);

	JSLog("aaa");

	int i = 0;
	while (true) {
		sleep(10);
		i += 10;
	}

}

void test2003() {

	flag_cout = true;
	log2cout = logviaCout2;

	JSON* json1 = new JSON();
	json1->initialize();

	JSObject* number = new JSObject();
	number->number = 123456;

	(*json1)["a"] = number;

	JSObject* number1 = ~(*json1)["a"];

	JSLog((~(*json1)["a"])->number);

//	int j = 35;
//	(*json1)((char *) &j, 4) = number;

	int i = 0;
	i++;
}

void test2004() {

	flag_cout = true;
	log2cout = logviaCout2;

	JSLog("test2004");
	JSON* json = NULL;

	const char * json_str1 = "[123,[1,12,13,[123,[1,12],456],14,15,16,17],456,789,101112]";
	json = parseJSON(json_str1);
	JSLog(json);

	const char * json_str2 = "[123,[1,'''abc'''],456,\"def\",'''b''','''abc''']";
	json = parseJSON(json_str2);
	JSLog(json);

	const char * json_str3 = "[123,[1,'''abc'''],\"a\":\"\"\"456\"\"\",\"def\"]";
	json = parseJSON(json_str3);
	JSLog(json);

	const char * json_str4 = "[123,567:[012,456,'123123\"\"123'],567]";
	json = parseJSON(json_str4);
	JSLog(json);

	const char * json_str5 = "[123,567:[012,[\"hello\"],456,'''''',123:[123,'123123'],123],567]";
	json = parseJSON(json_str5);
	JSLog(json);

	const char * json_str6 = "[[a_zero:-0,b:[-123,[1,12],456],123],'a':2,'b~3':\"123456\"]";
	json = parseJSON(json_str6);
	JSLog(json);

	const char * json_str = "[abc:'w123',   ,a  :-26654551,'''a''',\r\n\r\n  -6666123   ,[1,2,[a_zero:-0,b:[-123,[1,12],456],123],'a':2,'b:3':\"12345678\",,,,],'''b'c'c'c\r\n\"\"\r\nThis is String!!!\r\n\"\"'d'a''', '''168''',,,,]";
	json = parseJSON(json_str);
	JSLog(json);

	JSObject* jsObject = ~(*json)["abc"];
	JSLog(jsObject);

	(*json)["abc"] = "122323ww";

	JSObject* jsObject1 = ~(*json)["abc"];
	JSLog(jsObject1);
	JSLog(json);

	const char * json_str7 = "[A:'123',b:[-123, c:456],c:[[]]]";
	JSON* json1 = parseJSON(json_str7);
	JSLog(json1);

	(*json1)["dddd"] = "6677";
	JSLog(json1);
	JSLog(~(*json1)["dddd"]);
	JSLog(~(*json1)["eeee"]);

	(*json1)["d"] = "999888";
	JSLog(json1);

	(*json1)["d"] = 555555;
	JSLog(json1);

	(*json1)["e"] = "\"hello\"";
	JSLog(json1);

}

void test2005() {

	flag_cout = true;
	log2cout = logviaCout2;

	JSLog("test2005");

	const char * json_str7 = "[ABC:76523,b:[-123, c:456],c:[[],c1:[c2:[1,2]]]]";

	JSON* json1 = parseJSON(json_str7);
	JSLog(json1);

	(*json1)["c"]["d1"]["d22"] = "6677";
	(*json1)["c"]["c1"]["c2"] = "this is string";
	JSLog(json1);

	JSObject* jsObject1 = ~(*json1)["ABC"];
	JSLog(jsObject1);

	JSObject* jsObject2 = ~(*json1)["c"]["c1"]["c2"];
	JSLog(jsObject2);

	(*json1)["ABC"] = 9999;
	JSObject* jsObject3 = ~(*json1)["ABC"];
	JSLog(jsObject3);
	JSLog(json1);

	const char * json_str8 = "[ABC:555666,   \r \nb:    \r\n   [,,,,,\r\n   \n-666666   , c:-456],c:'abc']";
	JSON* json8 = parseJSON(json_str8);
	JSLog(json8);

	const char * json_str9 = "[ABC:[1,2]3,b:[-666666,c:-456]c:'abcd']";
	JSON* json9 = parseJSON(json_str9);
	JSLog(json9);

	const char * json_str10 = "[[[[bbb,132,[ABC:[1,2]3,b: [-888888,c:-456]c:'abcd']6,9,\"aaaa\"]]]]   ]]]]]]";
	JSON* json10 = parseJSON(json_str10);
	JSLog(json10);
}

void test2006_1() {
	flag_cout = true;
	log2cout = logviaCout2;

	JSLog("test2006");

	const char * json_str9 = "[ABC:'','':3,b:[-666666,c:-456]c:'abcd']";

	JSON* json9 = parseJSON(json_str9);
	JSLog(json9);

	(*json9)["ABC"] = 9999;
	JSLog(json9);
	(*json9)["b"]["c"] = -8888888;
	(*json9)["B1"]["B2"]["b3"] = "this is a string";
	JSLog(json9);

}

void test2006() {
	flag_cout = true;
	log2cout = logviaCout2;

	JSLog("test2006");

	const char * json_str9 = "[ABC:'',b:8888]";

	JSON* json9 = parseJSON(json_str9);
	JSLog(json9);


	(*json9)["ABC"] = 9999;
	JSLog(json9);

	JSString jsString="Hello";
	JSString * js_string=NULL;
	JSKeyValue * js_KeyValue;

	json9->set(1, &jsString);
	JSLog(json9);

	js_string=json9->get(1);
	JSLog(js_string);

	js_string=json9->del(1);

	JSString jsString1="1111111";
	json9->set(1, &jsString1);

	js_string=json9->get(1);
	JSLog(js_string);

	js_KeyValue = (JSKeyValue *) json9->find(2);
	JSLog(js_KeyValue->value);



}

void test2007() {
	flag_cout = true;
	log2cout = logviaCout2;

	JSLog("test2007");

	int errorCount = 0;
	JSString * js_string = new JSString(64);
//	JSString * js_string1 = new JSString(64);
	JSString jsLogString;
	for (int i = 0; i < 10000; i++) {
		int clientID = i % 10;

		(*js_string) = "{\"type\":@1@,\"clientID\":@2@,\"sendProgress\":@3@}";

		(*js_string) % 1 % clientID % 0;

		JSString * js_string1 = new JSString(16);
		(*js_string1) = "{\"type\":1,\"clientID\":@2@,\"sendProgress\":0}";
		(*js_string1) % clientID;

		if ((*js_string1) == (*js_string)) {

		} else {

			jsLogString = "ERROR!!!!!!!!!!!!!!errorCount=@@, i=@@, n=@@";
			JSLog(jsLogString % errorCount % i % ((i - 27) / 32));
			errorCount++;

			JSLog(js_string);
			JSLog(js_string1);
		}
	}

	JSLog("NONE ERROR");




	/*
	 * the difference is caused by the different result given by the following functions.
	 *
	 *
	 *
	 *

*void obverse_memcpy(void * destination, void * source, int count) {
	memcpy(destination, source, (unsigned int) count);
}

void obverse_memcpy_slow(void * destination, void * source, int count) {
	if (destination == NULL || source == NULL || count <= 0) {
		return;
	}

	char * destination_char = (char *) destination;
	char * source_char = (char *) source;
	for (int i = 0; i < count; i++) {
		*(destination_char + i) = *(source_char + i);
	}
}
	 */
}

void test2008() {
	flag_cout = true;
	log2cout = logviaCout2;

	JSLog("test2008");

	char * test = "1234567890123456789012345678901234567890";

	char * test1 = JSMalloc(40);
	char * test2 = JSMalloc(40);

	JSString string1;
	JSString string2;

	int errorCount = 0;
	JSString jsLogString;
	for (int i = 0; i < 10000; i++) {
		reverse_memcpy(test1, test, 40);
		reverse_memcpy(test2, test, 40);

		obverse_memcpy_slow(test1 + 10, test1 + 13, 40 - 13);
		obverse_memcpy(test2 + 10, test2 + 13, 40 - 13);

		string1 = test1;
		string2 = test2;
		if (string1 == string2) {

		} else {

			jsLogString = "ERROR!!!!!!!!!!!!!!errorCount=@@, i=@@";
			JSLog(jsLogString % errorCount % i);
			errorCount++;

			JSLog(string1);
			JSLog(string2);
		}

		if (i == 100) {
			JSLog(string1);
			JSLog(string2);
		}
	}

	JSLog("NONE ERROR");
}
