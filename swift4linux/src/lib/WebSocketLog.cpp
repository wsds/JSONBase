#include "WebSocketLog.h"

WebsocketLog::WebsocketLog() {
	this->is_initialized = false;
	this->localPort = 8091;
}

WebsocketLog * WebsocketLog::instance = NULL;
WebsocketLog * WebsocketLog::getInstance() {
	if (instance == NULL) {
		instance = new WebsocketLog();
	}
	return instance;
}
JSString * favicon;
JSString * favicon_file_path;

bool WebsocketLog::initialize() {
	if (this->is_initialized == true) {
		return true;
	}
	this->is_initialized = true;

	this->i = 0;

	this->httpServer = NULL;

	OpenHttp * openHttp = OpenHttp::getInstance();
	openHttp->initialize();

	this->httpServer = new HttpServer();
	this->httpServer->localPort = this->localPort;

	openHttp->createServer(this->httpServer);

	HttpCallback * httpCallback = new HttpCallback();
	this->httpServer->httpCallback = httpCallback;
	httpCallback->onRequest = (void (*)(HttpServerSession *))&(websocketLog_onRequest);httpCallback
	->onMessage = (void (*)(HttpServerSession *))&(websocketLog_onMessage);httpCallback
	->onFailure = &(websocketLog_onFailure);

	this->callJNICommand = NULL;

	flag_MyLog = true;
	log2MyLog = logviaMylog;

	favicon = new JSString("/favicon.ico");
	favicon->keyEnd = " ";
	favicon_file_path = new JSString("/alidata/favicon.ico");

	return true;
}
JSString jsString;

void websocketLog_onRequest(JSObject* jsObject) {
	WebsocketLog * websocketLog = WebsocketLog::getInstance();

	HttpServerSession* httpSession = (HttpServerSession*) jsObject;
	bool is_favicon = false;
	if ((*favicon) == httpSession->receive_message->urlPath) {
		is_favicon = true;
	}
	Message* message = new Message();
	if (is_favicon == false) {
		jsString.length=0;

		jsString + "{\"text\":\"hello" + websocketLog->i + "\"}";

		message->data = jsString.char_string;
		message->contentLength = strlen(message->data);
		message->headerMap = new JSON();
		message->headerMap->initialize();
		(*message->headerMap)[(char *) "Content-Type"] = (char *) "application/json; charset=UTF-8";

	} else {
		message->headerMap = new JSON();
		message->headerMap->initialize();
		(*message->headerMap)[(char *) "Accept-Ranges"] = (char *) "bytes";
		(*message->headerMap)[(char *) "Content-Type"] = (char *) "image/x-icon";
		(*message->headerMap)[(char *) "Cache-Control"] = (char *) "public, max-age=999999";

		message->sendFilePath = favicon_file_path;
		message->contentLength = 5430;
	}
	httpSession->sendResponse(message);
	websocketLog->i++;
//	websocketLog->sendMessage(jsString);

}

void websocketLog_onFailure(ErrorMessage * errorMessage) {
	JSString jsString;
	jsString + "{\"websocketLog_onFailure\":\"errorMessage: ";
	jsString + errorMessage->errorMessage->char_string;
	jsString + "\"}";
	JSLog(jsString);
}

void WebsocketLog::sendMessage(JSString & jsString) {
	char * frameStr = jsString.char_string;
	int frameLength = jsString.length;
	httpServer->httpSessionMap;
	for (int i = 0; i < this->httpServer->httpSessionMap->length; i++) {
		JSKeyValue * jsKeyValue = (JSKeyValue*) this->httpServer->httpSessionMap->find(i);
		HttpServerSession* httpSession = (HttpServerSession*) jsKeyValue->value;
		httpSession->sendMessageFrame(frameStr, frameLength);
	}
}

JSON * checkHttpClientMap() {

	JSON * result = parseJSON("[]");

	(*result)[(char *) "command"] = (char *) "httpClientMap";

	OpenHttp * openHttp = OpenHttp::getInstance();
	(*result)[(char *) "ClientQueueSize"] = openHttp->httpClientQueue->length;

	JSON * httpClientMap = openHttp->httpClientMap;

	for (int i = 0; i < httpClientMap->length; i++) {

		JSKeyValue * js_KeyValue = (JSKeyValue *) httpClientMap->find(i);

		HttpClient *httpClient = (HttpClient *) js_KeyValue->value;

		JSON * httpClient_json = parseJSON("[]");

		char * clientIDKey = numberToString(httpClient->clientID);
		result->set(clientIDKey, httpClient_json);

//		JSLog((int) httpClient);
//		JSLog(httpClient->clientID);

		HttpClientSession *httpClientSession = httpClient->httpClientSession;

		if (openHttp != NULL && httpClientSession != NULL) {
			struct tcp_info info;
			unsigned int tcp_info_length = sizeof(info);
			getsockopt(httpClientSession->socketFD, SOL_TCP, TCP_INFO, (void *) &info, (socklen_t *) tcp_info_length);
			(*httpClient_json)[(char *) "rtt"] = info.tcpi_rtt;

			float sendProgress = openHttp->sendProgress(httpClientSession->clientID);
			int sendProgress_int = int(sendProgress * (float) 10000);
			(*httpClient_json)[(char *) "sendProgress"] = sendProgress_int;

			(*httpClient_json)[(char *) "clientID"] = httpClientSession->clientID;
			(*httpClient_json)[(char *) "sentLength"] = httpClientSession->sentLength;
			(*httpClient_json)[(char *) "linuxQueueLength"] = httpClientSession->linuxQueueLength;
		} else {
			(*httpClient_json)[(char *) "status"] = (char *) "NONE httpClientSession";
		}
	}

	return result;

}

void websocketLog_onMessage(JSObject* jsObject) {
	WebsocketLog * websocketLog = WebsocketLog::getInstance();
	HttpServerSession* httpSession = (HttpServerSession*) jsObject;

	Message * message = httpSession->websocket_frame_message;

	JSON * message_json = parseJSON(message->data);

	JSString * command = (JSString *) ~(*message_json)[(char *) "command"];

	if ((*command) == "test") {

		JSString jsString;
		jsString * "['command':'test','data':'This is MyLog.i= @@.']" % websocketLog->i;
		httpSession->sendMessageFrame(jsString.char_string, jsString.length);
	} else if ((*command) == "httpClientMap") {

		JSON * result = checkHttpClientMap();

		JSString* js_string = stringifyJSON2JSString(result);

		JSLog(js_string);

//		httpSession->sendMessageFrame(js_string->char_string, js_string->length);
	} else if ((*command) == "JNICommand") {

		JSString *jni_command = (JSString *) ~(*message_json)[(char *) "JNICommand"];
		if (websocketLog->callJNICommand != NULL) {
			websocketLog->callJNICommand(jni_command);
		}
	} else if ((*command) == "SubProcess") {
		JSString *sub_command = (JSString *) ~(*message_json)[(char *) "Command"];
		if (websocketLog->callSubProcessCommand != NULL) {
			websocketLog->callSubProcessCommand(sub_command);
		}
	} else if ((*command) == "swift") {

		JSString * swiftCommand = (JSString *) ~(*message_json)[(char *) "SwiftCommand"];
		JSString * code = (JSString *) ~(*message_json)[(char *) "code"];
		JSNumber * js_contextID = (JSNumber *) ~(*message_json)[(char *) "contextID"];
		int contextID = js_contextID->number;
		websocketLog->SwiftCommand(swiftCommand, contextID, code, httpSession);
	}

	delete (message_json);
	websocketLog->i++;
}

void WebsocketLog::SwiftCommand(JSString * swiftCommand, int contextID, JSString * code, HttpServerSession* httpSession) {
	Interpreter * interpreter = Interpreter::getInstance();

	if ((*swiftCommand) == "new_context") {
		int contextID = interpreter->newContext();
		JSString jsString;
		jsString * "{'command':'swift','swift_command':'contextID','contextID':@@}" % contextID;
		JSON * responce = parseJSON(&jsString);
		JSString* responceStr = stringifyJSON2JSString(responce);
		httpSession->sendMessageFrame(responceStr->char_string, responceStr->length);
	} else if ((*swiftCommand) == "code") {
		code = code->replaceAll("\\\"", "\"");
		code = code->replaceAll("\\n", "\n");
		code = code->replaceAll("\\r", "\n");
		code = code->replaceAll("\\t", "  ");
		code = code->replaceAll("}", "}\n");
		code = code->replaceAll("{", "{\n");
		code = code->replaceAll("\n\n", "\n");
		code = code->replaceAll("\n\n", "\n");

		interpreter->inputCode(contextID, code);
	} else if ((*swiftCommand) == "run") {
		interpreter->run(contextID);
	} else if ((*swiftCommand) == "clear") {
		interpreter->clear(contextID);
	} else if ((*swiftCommand) == "closure") {
		interpreter->logClosure(contextID);
	} else if ((*swiftCommand) == "runcode") {
		interpreter->runCode(contextID, code);
	}

}

void WebsocketLog::log(JSString & jsString) {
	this->sendMessage(jsString);
}

void logviaMylog(JSString & jsString) {
	WebsocketLog * mylog = WebsocketLog::getInstance();
	if (mylog->is_initialized == true) {
		mylog->log(jsString);
	}
}

