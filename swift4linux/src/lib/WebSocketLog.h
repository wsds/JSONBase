#ifndef LOG_H
#define LOG_H

#include "../data_core/JSString.h"
#include "../openHttp/OpenHttp.h"
#include "../swift/interpret_main.h"
#include "JSLog.h"

class WebsocketLog {
public:
	WebsocketLog();

	static WebsocketLog *instance;
	static WebsocketLog * getInstance();

	bool is_initialized;
	bool initialize();

	int localPort;

	HttpServer * httpServer;

	void log(JSString & jsString);

	void sendMessage(JSString & jsString);

	void (*callJNICommand)(JSString * jsString);
	void (*callSubProcessCommand)(JSString * jsString);

	void SwiftCommand(JSString * swiftCommand, int contextID, JSString * code, HttpServerSession* httpSession);

	int i;
};

void websocketLog_onRequest(JSObject* jsObject);
void websocketLog_onFailure(ErrorMessage * errorMessage);
void websocketLog_onMessage(JSObject* jsObject);

void logviaMylog(JSString & jsString);

#endif /* LOG_H */

