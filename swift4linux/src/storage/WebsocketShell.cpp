#include "WebsocketShell.h"

WebsocketShell::WebsocketShell() {
	this->is_initialized = false;
	this->localPort = 8092;
}

WebsocketShell * WebsocketShell::instance = NULL;
WebsocketShell * WebsocketShell::getInstance() {
	if (instance == NULL) {
		instance = new WebsocketShell();
	}
	return instance;
}

bool WebsocketShell::initialize() {
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

	httpCallback->onRequest = (void (*)(HttpServerSession *))&(websocket_shell_onRequest);

httpCallback	->onMessage = (void (*)(HttpServerSession *))&(websocket_shell_onMessage);

httpCallback	->onFailure = &(websocket_shell_onFailure);

	this->favicon = new JSString("/favicon.ico");
	this->favicon->keyEnd = " ";
	this->favicon_file_path = new JSString("/alidata/favicon.ico");

	this->dm_string = newDMString(1024);
	this->jsonbase_request = new JSONBaseRequest();

	this->list_root_nodes = new TraverseCallback();
	this->list_root_nodes->onGetData = &(onGetData_RootNodes);
	this->list_node_elements = new TraverseCallback();
	this->list_node_elements->onGetData = &(onGetData_NodeElements);

	return true;
}

void websocket_shell_onRequest(JSObject* jsObject) {
	WebsocketShell * websocket_shell = WebsocketShell::getInstance();

	HttpServerSession* httpSession = (HttpServerSession*) jsObject;
	bool is_favicon = false;
	if ((*websocket_shell->favicon) == httpSession->receive_message->urlPath) {
		is_favicon = true;
	}
	Message* message = new Message();
	if (is_favicon == false) {
		websocket_shell->jsString.length = 0;

		websocket_shell->jsString + "{\"JSONBase\":\"welcome! " + websocket_shell->i + "\"}";

		message->data = websocket_shell->jsString.char_string;
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

		message->sendFilePath = websocket_shell->favicon_file_path;
		message->contentLength = 5430;
	}

	httpSession->sendResponse(message);
	websocket_shell->i++;
}

void websocket_shell_onFailure(ErrorMessage * errorMessage) {
}

void WebsocketShell::sendMessage(JSString & jsString) {
	char * frameStr = jsString.char_string;
	int frameLength = jsString.length;
	httpServer->httpSessionMap;
	for (int i = 0; i < this->httpServer->httpSessionMap->length; i++) {
		JSKeyValue * jsKeyValue = (JSKeyValue*) this->httpServer->httpSessionMap->find(i);
		HttpServerSession* httpSession = (HttpServerSession*) jsKeyValue->value;
		httpSession->sendMessageFrame(frameStr, frameLength);
	}
}

void websocket_shell_onMessage(JSObject* jsObject) {
	WebsocketShell * websocket_shell = WebsocketShell::getInstance();
	HttpServerSession* httpSession = (HttpServerSession*) jsObject;

	Message * message = httpSession->websocket_frame_message;

	JSON * message_json = parseJSON(message->data);

	JSString * command = (JSString *) ~(*message_json)[(char *) "command"];

	if ((*command) == "test") {

		JSString jsString;
		jsString * "['command':'test','data':'This is MyLog.i= @@.']" % websocket_shell->i;
		httpSession->sendMessageFrame(jsString.char_string, jsString.length);
	} else if ((*command) == "JSONBase") {
		JSString * operation = (JSString *) ~(*message_json)[(char *) "operation"];
		if (operation == NULL) {
			websocket_shell->dm_string->used_length = 0;
			(*websocket_shell->dm_string) * "['state':650,'message':'error: invalid request!']";
			httpSession->sendMessageFrame(websocket_shell->dm_string->char_string, websocket_shell->dm_string->used_length);

		} else {
			JSONBaseRequest * jsonbase_request = websocket_shell->jsonbase_request;
			jsonbase_request->js_operation = operation;
			jsonbase_request->httpSession = httpSession;

			JSNumber * js_connection_id = (JSNumber *) ~(*message_json)[(char *) "connection_id"];
			jsonbase_request->connection_id = -1;
			if (js_connection_id != NULL) {
				jsonbase_request->connection_id = js_connection_id->number;
			}
			jsonbase_request->js_key_chain = (JSString *) ~(*message_json)[(char *) "key_chain"];
			jsonbase_request->js_value_string = (JSString *) ~(*message_json)[(char *) "value"];

			jsonbase_request->js_node_name = (JSString *) ~(*message_json)[(char *) "node_name"];
			jsonbase_request->js_secret = (JSString *) ~(*message_json)[(char *) "secret"];

			jsonbase_request->js_access_key = (JSString *) ~(*message_json)[(char *) "access_key"];
			JSString * empty = new JSString(0);

			if (jsonbase_request->js_key_chain == NULL) {
				jsonbase_request->js_key_chain = empty;
			}
			if (jsonbase_request->js_value_string == NULL) {
				jsonbase_request->js_value_string = empty;
			}
			if (jsonbase_request->js_access_key == NULL) {
				jsonbase_request->js_access_key = empty;
			}
			if (jsonbase_request->js_node_name == NULL) {
				jsonbase_request->js_node_name = empty;
			}
			if (jsonbase_request->js_secret == NULL) {
				jsonbase_request->js_secret = empty;
			}

			websocket_shell->jsonBaseCommand(jsonbase_request);
		}
	}

	delete (message_json);
	websocket_shell->i++;
}

void WebsocketShell::jsonBaseCommand(JSONBaseRequest * jsonbase_request) {
	DMString * operation = newDMString(0);
	operation->char_string = jsonbase_request->js_operation->char_string;
	operation->max_length = jsonbase_request->js_operation->length;
	operation->used_length = operation->max_length;

	int connection_id = jsonbase_request->connection_id;

	DMString * key_chain = newDMString(0);
	key_chain->char_string = jsonbase_request->js_key_chain->char_string;
	key_chain->max_length = jsonbase_request->js_key_chain->length;
	key_chain->used_length = key_chain->max_length;

	DMString * value_string = newDMString(0);
	value_string->char_string = jsonbase_request->js_value_string->char_string;
	value_string->max_length = jsonbase_request->js_value_string->length;
	value_string->used_length = value_string->max_length;

	DMString * node_name = newDMString(0);
	node_name->char_string = jsonbase_request->js_node_name->char_string;
	node_name->max_length = jsonbase_request->js_node_name->length;
	node_name->used_length = node_name->max_length;

	DMString * secret = newDMString(0);
	secret->char_string = jsonbase_request->js_secret->char_string;
	secret->max_length = jsonbase_request->js_secret->length;
	secret->used_length = secret->max_length;

	DMString * connection_access_key = newDMString(0);
	connection_access_key->char_string = jsonbase_request->js_access_key->char_string;
	connection_access_key->max_length = jsonbase_request->js_access_key->length;
	connection_access_key->used_length = connection_access_key->max_length;

	this->dm_string->used_length = 0;

	JSONBase * json_base = JSONBase::getInstance();
	if ((*operation) == "create") {
		//todo check the developer_access_key

		RootNode * new_root_node = json_base->create(node_name, secret);
		if (new_root_node != NULL) {
			(*this->dm_string) * "['state':200,'message':'done!']";
		} else {
			(*this->dm_string) * "['state':608,'message':'specified json has been already existed!!']";
		}

	} else if ((*operation) == "open") {
		DMString * node_access_key = secret;
		JSONBaseConnection * new_connection = json_base->open(node_name, node_access_key);
		if (new_connection != NULL) {
			(*this->dm_string) * "['state':200,'message':'done!','connection_id':@@,'root_node_name':'@@','current_path':'@@', 'connection_access_key':'@@']";
			(*this->dm_string) % new_connection->id % new_connection->root_node->node_name % new_connection->current_path;
			(*this->dm_string) % new_connection->access_key;
		} else {
			(*this->dm_string) * "['state':601,'message':'specified json cannot be opened!!']";
		}
	} else if ((*operation) == "close") {
		if (connection_id < 0) {
			(*this->dm_string) * "['state':602,'message':'this operation cannot be done, before the connection is established!']";
		} else {
			bool is_done = json_base->close(connection_id, connection_access_key);
			if (is_done == false) {
				(*this->dm_string) * "['state':605,'message':'error when close json']";
			} else {
				(*this->dm_string) * "['state':200,'message':'done!']";
			}
		}
	} else if ((*operation) == "cd") {
		if (connection_id < 0) {
			(*this->dm_string) * "['state':602,'message':'this operation cannot be done, before the connection is established!']";
		} else {
			JSONBaseConnection * connection = json_base->getConnection(connection_id, connection_access_key);
			if (connection == NULL) {
				(*this->dm_string) * "['state':620,'message':'invalid connection!!']";
			} else {
				bool is_done = connection->cd(key_chain);
				if (is_done == false) {
					(*this->dm_string) * "['state':603,'message':'specified path cannot be entered!']";
				} else {
					(*this->dm_string) * "['state':200,'message':'done!','current_path':'@@']";
					(*this->dm_string) % connection->current_path;
				}
			}
		}
	} else if ((*operation) == "set" || (*operation) == "put") {
		if (connection_id < 0) {
			(*this->dm_string) * "['state':602,'message':'this operation cannot be done, before the connection is established!']";
		} else {
			JSONBaseConnection * connection = json_base->getConnection(connection_id, connection_access_key);
			if (connection == NULL) {
				(*this->dm_string) * "['state':620,'message':'invalid connection!!']";
			} else {
				MemorySpace * value = NULL;
				if (value == NULL) {
					bool is_number = isNumber(value_string->char_string, value_string->max_length);
					if (is_number == true) {
						DMInt * dm_number = newDMInt();
						dm_number->number = parseStringToNubmer(value_string->char_string, value_string->max_length);
						value = (MemorySpace *) dm_number;
					}
				}

				if (value == NULL) {
					bool is_json = isJSON(value_string->char_string, value_string->max_length);
					if (is_json == true) {
						JSString * js_value_string = jsonbase_request->js_value_string;
						js_value_string = js_value_string->replaceAll("\\\"", "\"");

						value_string->char_string = js_value_string->char_string;
						value_string->max_length = js_value_string->length;
						value_string->used_length = value_string->max_length;

						DMJSON * dm_json = parseDMJSON(value_string->char_string, value_string->max_length);
						value = dm_json->self;
					}
				}

				if (value == NULL) {
					DMString * dm_string = newDMString(value_string->max_length);
					dm_string->copy(value_string->char_string, value_string->max_length);
					value = (MemorySpace *) dm_string;
				}

				bool is_done = false;
				if ((*operation) == "set") {
					is_done = connection->set(key_chain, value);
				} else {
					is_done = connection->put(key_chain, value);
				}

				if (is_done == false) {
					(*this->dm_string) * "['state':604,'message':'error when set|put!']";
				} else {
					(*this->dm_string) * "['state':200,'message':'done!']";
				}
			}

		}

	} else if ((*operation) == "get" || (*operation) == "del") {
		if (connection_id < 0) {
			(*this->dm_string) * "['state':602,'message':'this operation cannot be done, before the connection is established!']";
		} else {
			JSONBaseConnection * connection = json_base->getConnection(connection_id, connection_access_key);
			if (connection == NULL) {
				(*this->dm_string) * "['state':620,'message':'invalid connection!!']";
			} else {
				MemorySpace* value = NULL;
				if ((*operation) == "get") {
					value = connection->get(key_chain);
				} else {
					value = connection->del(key_chain);
				}
				(*this->dm_string) * "@@";
				formatString(this->dm_string, value);

				if (value != NULL && value->type == TYPE_JSON) {
					DMString * format_string = formatJSON(this->dm_string);
					this->dm_string->used_length = 0;
					(*this->dm_string) * "@@";
					(*this->dm_string) % format_string;
				}
			}

		}
	} else if ((*operation) == "update") {
		if (connection_id < 0) {
			(*this->dm_string) * "['state':620,'message':'invalid connection!!'']";
		} else {
			JSONBaseConnection * connection = json_base->getConnection(connection_id, connection_access_key);

			if (connection != NULL) {
				(*this->dm_string) * "['state':200,'message':'done!','connection_id':@@,'root_node_name':'@@','current_path':'@@']";
				(*this->dm_string) % connection->id % connection->root_node->node_name % connection->current_path;
			} else {
				(*this->dm_string) * "['state':620,'message':'invalid connection!!']";
			}
		}
	} else if ((*operation) == "list") {
		if (connection_id == -1) {
			json_base->list_output = this->dm_string;
			json_base->list(this->list_root_nodes);
		} else {
			JSONBaseConnection * connection = json_base->getConnection(connection_id, connection_access_key);
			if (connection == NULL) {
				(*this->dm_string) * "['state':620,'message':'invalid connection!!']";
			} else {
				MemorySpace* value = connection->list();
				if (value == NULL) {
					(*this->dm_string) * "NULL";
				} else {
					if (value->type == TYPE_STRING) {
						(*this->dm_string) * "str";
					} else if (value->type == TYPE_NUMBER) {
						(*this->dm_string) * "num";
					} else if (value->type == TYPE_KEY_VALUE) {
						(*this->dm_string) * "K/V";
					} else if (value->type == TYPE_JSON) {
						(*this->dm_string) * "[";
						DMJSON * dm_json = (DMJSON *) value->pointer;
						json_base->list_output = this->dm_string;
						dm_json->traverse(this->list_node_elements);
						(*this->dm_string) * "]";
					}
				}
			}

		}
	} else {
		(*this->dm_string) * "['state':700,'message':'invalid operation!!']";
	}

	jsonbase_request->httpSession->sendMessageFrame(this->dm_string->char_string, this->dm_string->used_length);

	freeDMString(operation);

	freeDMString(key_chain);
	freeDMString(value_string);

	freeDMString(node_name);
	freeDMString(secret);

	freeDMString(connection_access_key);
}
DMString * format_string;

DMString * formatJSON(DMString * input) {
	if (format_string == NULL) {
		format_string = newDMString(1024);
	}
	DMString * output = format_string;
	char * output_char_str = output->char_string;
	int input_index = 0;
	int output_index = 0;
	int indent = 0;
	char local_char;

	for (output_index = 0; output_index < output->max_length; output_index++) {
		output_char_str[output_index] = ' ';
	}
	output_index = 0;

	for (input_index = 0; input_index < input->used_length; input_index++) {
		local_char = input->char_string[input_index];
		if (local_char == '[') {
			indent = indent + 2;
			output_char_str[output_index++] = local_char;
			output_char_str[output_index++] = '\n';
			output_index = output_index + indent;
		} else if (local_char == ']') {
			indent = indent - 2;
			output_char_str[output_index++] = '\n';
			output_index = output_index + indent;
			output_char_str[output_index++] = local_char;
		} else if (local_char == ':') {
//			output_index++;
			output_char_str[output_index++] = local_char;
//			output_index++;
		} else if (local_char == ',') {
			output_char_str[output_index++] = local_char;
			output_char_str[output_index++] = '\n';
			output_index = output_index + indent;
		} else {
			output_char_str[output_index++] = local_char;
		}
	}
	output->used_length = output_index;

	return output;
}

