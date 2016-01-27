#ifndef WEBSOCKET_SHELL_H
#define WEBSOCKET_SHELL_H

#include "../data_core/JSString.h"
#include "../openHttp/OpenHttp.h"
#include "JSONBase.h"

class JSONBaseRequest {
public:
	JSString * js_operation;

	int connection_id;

	JSString * js_key_chain;
	JSString * js_value_string;

	JSString * js_node_name;
	JSString * js_secret;

	JSString * js_access_key;

	HttpServerSession* httpSession;
};
class WebsocketShell {
public:
	WebsocketShell();

	static WebsocketShell *instance;
	static WebsocketShell * getInstance();

	bool is_initialized;
	bool initialize();

	int localPort;
	HttpServer * httpServer;

	void sendMessage(JSString & jsString);

	void jsonBaseCommand(JSONBaseRequest * jsonbase_request);

	int i;

	JSString * favicon;
	JSString * favicon_file_path;
	JSString jsString;
	DMString * dm_string;

	JSONBaseRequest * jsonbase_request;

	TraverseCallback * list_root_nodes;
	TraverseCallback * list_node_elements;
};

void websocket_shell_onRequest(JSObject* jsObject);
void websocket_shell_onFailure(ErrorMessage * errorMessage);
void websocket_shell_onMessage(JSObject* jsObject);

DMString * formatJSON(DMString * input);

#endif /* WEBSOCKET_SHELL_H */

