#include "OpenHttp.h"
//////
Message::Message() {
	this->type = 1;
	this->version = NULL;
	this->stateCode = NULL;
	this->phrase = NULL;
	this->method = NULL;
	this->urlPath = NULL;
	this->headerMap = NULL;
	this->sendFilePath = NULL;
	this->sendFileStart = 0;
	this->sendFileLength = 0;
	this->receiveFilePath = NULL;
	this->data = NULL;
	this->frameStr = NULL;
	this->frameLength = 0;
	this->contentLength = 0;
	this->headerLength = 0;
	this->header = NULL;
	this->followingHeader = NULL;
	this->Host = NULL;
	this->Sec_WebSocket_Key = NULL;
}

SessionStatus::SessionStatus() {
	Connected = 1, KeepAlive = 2, WebSocket = 3, Closed = 7;
	ReceiveWaiting = 11, Receiving = 12, Received = 13;
	SendPending = 21, Sending = 22, Sent = 23;
	Failed = 101, Timeout = 102;

	state = Connected;
	sendState = SendPending;
	receiveState = ReceiveWaiting;

}

ServerStatus::ServerStatus() {
	Starting = 1, Listening = 2, Closed = 3;
	state = Starting;
}

Status::Status() {
	Queueing = 0, Started = 1, Connecting = 2, Connected = 3, Sending = 4, Sent = 5, Waiting = 5, Receiving = 6, Received = 7;
	Failed = 10, Timeout = 11;
	time_queueing = 0, time_started = 0, time_connecting = 0, time_connected = 0, time_sending = 0, time_sent = 0, time_waiting = 0, time_receiving = 0, time_received = 0;
	time_failed = 0, time_timeout = 0;
	state = Queueing;
}

OpenHttp::OpenHttp() {
	helper = new OpenHttpHelper();
	is_initialized = false;
	epollFD = 0;
}

HttpSession::HttpSession() {
	this->status = new SessionStatus();
	this->needAbort = false;

	this->pending_message_queue = new Queue();
	this->pending_message_queue->initialize();

	this->socketFD = -1;
	this->event = NULL;

	this->MAXError = 20;

	this->timeout = 5 * 1000;
	this->min_updata_interval = 100;

	this->sendProgress = 0;
	this->receiveProgress = 0;
	this->last_update_iTimeval = new timeval();

	//*************************receive***********************

	this->receivePackageLength = 0;
	this->received_PackagesNumber = 0;
	this->received_ContentLength = 0;
	this->received_HeaderLength = 0;
	this->receive_ContentLength = 0;

	this->defaultReceiveBuffer = (char *) JSMalloc(1024);
	this->receiveBuffer = this->defaultReceiveBuffer;

	this->receiveFD = -1;
	this->receiveOffset = 0;
	this->receiveFileBuffer = NULL;
	this->receiveDataBuffer = NULL;
	this->receive_percent = 0;
	this->receive_message = NULL;
	this->websocket_frame_message = NULL;

	//*************************receive***********************

	//*************************send**************************
	this->send_HeaderLength = 0;
	this->send_ContentLength = 0;
	this->send_FileLength = 0;

	this->sendHeaderBuffer = (char *) JSMalloc(1024);
	this->sendBuffer = NULL;
	this->sendSize = 0;
	this->sentLength = 0;
	this->linuxQueueLength = 0;
	this->sentPackegeLength = 0;

	this->sendFD = -1;

	this->sendFileBuffer = NULL;
	this->sendFileStart = 0;
	this->sendDataBuffer = NULL;
	this->send_pending_message = NULL;
	this->send_message = NULL;

}
/////

HttpServerSession::HttpServerSession(JSObject * httpServer) {
	this->type = 1;

	this->httpCallback = ((HttpServer *) httpServer)->httpCallback;
}

HttpClientSession::HttpClientSession(JSObject * httpClient) {
	this->type = 2;
	this->clientID = -1;

	this->httpCallback = ((HttpClient *) httpClient)->httpCallback;
}

HttpServer::HttpServer() {
	this->httpSessionMap = new JSON();
	this->httpSessionMap->initialize();
}

HttpClient::HttpClient() {
	this->state = 1;
	this->needAbort = false;

	this->socketFD = -1;
	this->receiveFD = -1;
	this->event = NULL;
	this->httpClientSession = NULL;

	this->pending_message_queue = new Queue();
	this->pending_message_queue->initialize();

	this->timeout = -1;

	this->start_iTimeval = new timeval();

	this->httpClientSession == NULL;
}

HttpCallback::HttpCallback() {
	this->onRequest = (void (*)(HttpServerSession *))&(doNothing);this->onMessage = (void(*)(HttpServerSession *))&(doNothing);
	this->onResponce = (void(*)(HttpClientSession *))&(doNothing);
	this->onFailure = (void(*)(ErrorMessage *))&(doNothing);
	this->onUpdateState = (void(*)(HttpClientSession *))&(doNothing);
}

void doNothing(HttpSession* thiz) {
	JSLog("There is nothing to do.");
}

OpenHttp * OpenHttp::instance = NULL;
OpenHttp * OpenHttp::getInstance() {
	if (instance == NULL) {
		instance = new OpenHttp();
	}

	return instance;
}

bool OpenHttp::initialize() {

	if (this->is_initialized == true) {
		return true;
	}

	this->helper = new OpenHttpHelper();
	this->httpServerMap = new HashTable2();
	this->httpServerMap->initialize();

	this->httpClientMap = new JSON();
	this->httpClientMap->initialize();

	this->httpSessionMap = new JSON();
	this->httpSessionMap->initialize();

//	this->keepAliveHttpSessionMap = new HashTable();
//	this->keepAliveHttpSessionMap->initialize();

	this->clientID_httpClientMap = new HashTable2();
	this->clientID_httpClientMap->initialize();

	this->httpClientQueue = new Queue();
	this->httpClientQueue->initialize();

	this->fileMap = new HashTable();
	this->fileMap->initialize();

	this->epollFD = epoll_create(1024);

	epollLooperPthread = new pthread_t();
	int error = pthread_create(epollLooperPthread, NULL, epollLooperThread, (void *) 1);
	if (error != 0) {
		return false;
	}

	sleep(1);
	this->is_initialized = true;
	for (int i = 0; i < helper->max_connecting_Client; i++) {
		this->startNextClient();
	}

	return true;
}

OpenHttpHelper::OpenHttpHelper() {
	this->portQueueSize = 100;
	this->minPortQueueSize = 50;

	this->start_iTimeval = new timeval();
	this->iTimeval = new timeval();
	gettimeofday(this->start_iTimeval, NULL);

	this->LineEnd = (char *) ("\r\n");
	this->KeyEnd = (char *) " :"; //32||58

	this->EndLine_Mark = ("\r\n");

	this->ContentLength_Mark = "Content-Length";
	this->Connection_Mark = "Connection";

	this->HTTP_Mark = "HTTP";
	this->HTTP_Mark.keyEnd = (char *) "/";

	this->responseTemplate = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: @FBooB7FAkLlXgRSz0BT3v4hq5s@\r\nSec-WebSocket-Origin: null\r\nSec-WebSocket-Location: ws://127.0.0.0.1/\r\n\r\n";
	this->mask = "@@258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	this->mask.keyEnd = this->LineEnd;

	this->keep_alive_Mark = "keep-alive";

	this->sendBuffSize = 1024;
	this->PackegeSize = 1024;
	this->MaxBufflen = 1024;
	this->MAXSOCKET = 1024;

	this->MaxEvent = 100;

	this->max_connecting_Client = 5;
	this->default_timeout = 5 * 1000;

	this->lineKey = (char *) JSMalloc(50 * sizeof(char));
	this->lineValue = (char *) JSMalloc(50 * sizeof(char));

	int BASE64_SHA1_HASH_SIZE = (SHA1_HASH_SIZE / 3 + (SHA1_HASH_SIZE % 3 != 0)) * 4;
	this->Sec_WebSocket_Accept = (char *) JSMalloc(BASE64_SHA1_HASH_SIZE * sizeof(char));

	this->Upgrade_Mark = (char *) ("Upgrade");

	this->E200_Mark = "200";
	this->E200_Mark.keyEnd = (char *) " ";

	this->startPortNumber = STARTPORTNUMBER;

}
ErrorMessage::ErrorMessage() {
	this->stateCode = -1;
	this->errorMessage = new JSString();
	this->clientID = -1;
	this->header = NULL;
	this->body = NULL;
}
ErrorMessage::ErrorMessage(int stateCode) {
	this->stateCode = stateCode;
	this->errorMessage = new JSString();
	this->clientID = -1;
	this->header = NULL;
	this->body = NULL;
}
ErrorMessage::ErrorMessage(int stateCode, char * string) {
	this->stateCode = stateCode;
	this->errorMessage = new JSString();
	(*this->errorMessage) = string;
	this->clientID = -1;
	this->header = NULL;
	this->body = NULL;
}
ErrorMessage::~ErrorMessage() {
	delete (this->errorMessage);
}

int OpenHttp::createClient(HttpClient * httpClient) {
	if (httpClient->state == 1) {
		if (httpClient->timeout == -1) {
			httpClient->timeout = helper->default_timeout;
		}
		this->clientID_httpClientMap->set(httpClient->clientID, httpClient);

		if ((this->is_initialized == false || this->httpClientMap->length >= helper->max_connecting_Client) && httpClient->clientID < 10000000) {
			httpClient->state = 2;
			this->httpClientQueue->offer(httpClient);
			return -1;
		} else {
			httpClient->state = 2;
		}
	}

	if (httpClient->state != 2) {
		return -1;
	}

//	if (this->keepAliveHttpSessionMap->length > 0) {
//		JSString * remoteKey = helper->getRemoteKey(httpClient->remotePort, httpClient->remoteIP);
//		HttpClientSession * httpClientSession = (HttpClientSession *) this->keepAliveHttpSessionMap->del((void *)remoteKey->char_string, remoteKey->length);
//		if (httpClientSession != NULL && httpClientSession->status->state == httpClientSession->status->KeepAlive) {
//
//			httpClient->httpClientSession = httpClientSession;
//			this->httpClientMap->set(httpClient->socketFD, httpClient);
//
//			httpClient->socketFD = httpClientSession->socketFD;
//			httpClient->event = httpClientSession->event;
//
//			httpClient->state = 3;
//
//			httpClientSession->receiveBuffer = httpClientSession->defaultReceiveBuffer;
//			httpClientSession->sendFD = -1;
//			httpClientSession->receiveFD = -1;
//
//			httpClientSession->clientID = httpClient->clientID;
//			httpClientSession->httpCallback = httpClient->httpCallback;
//
//			if (httpClient->httpClientSession->status->sendState == httpClient->httpClientSession->status->SendPending) {
//				httpClient->sendRequest(NULL);
//			}
//			return 1;
//		}
//	}
	int error = startConnect(httpClient);
	if (error != -1) {
		this->httpClientMap->set(httpClient->socketFD, httpClient);

		httpClient->state = 3;
		gettimeofday(httpClient->start_iTimeval, NULL);
		return 1;
	}
	return -1;
}

int OpenHttp::clearHttpClient(HttpClient * httpClient) {
	if (httpClient->state == 5 || httpClient->state == 6) {
		return -1;
	}

	HttpClient * oldClient1 = (HttpClient *) this->clientID_httpClientMap->del(httpClient->clientID);

	if (oldClient1 == NULL) {
		return -1;
	}

	if (httpClient->state == 3) {
		epoll_ctl(this->epollFD, EPOLL_CTL_DEL, httpClient->socketFD, httpClient->event);
		close(httpClient->socketFD);
		HttpClient * oldClient = (HttpClient *) this->httpClientMap->del(httpClient->socketFD);
		if (oldClient != NULL) {
			this->startNextClient();
		}
	} else if (httpClient->state == 4) {
		this->clearSession(httpClient->httpClientSession);
	}

	if (httpClient->needAbort == true) {
		httpClient->state = 6;
		JSLog("******************************************************");
		helper->reportError2Client(httpClient, (char *) "client aborted", 10403);
	} else {
		httpClient->state = 5;
	}
	this->startNextClient();

	return 1;
}

int j = 0;
int OpenHttp::clearSession(HttpSession * session) {
	if (session->status->state == session->status->Closed) {
		return -1;
	}
	session->status->state = session->status->Closed;
	HttpSession * oldSession = (HttpSession *) this->httpSessionMap->del(session->socketFD);
	if (oldSession != NULL) {
		epoll_ctl(this->epollFD, EPOLL_CTL_DEL, oldSession->socketFD, oldSession->event);
		close(oldSession->socketFD);

		if (session->sendFileBuffer != NULL || session->sendFileBuffer != (void *) -1) {
			munmap(session->sendFileBuffer, session->send_FileLength);
		}

		HttpServer * oldServer = (HttpServer *) this->httpServerMap->get(oldSession->socketFD);
		if (oldServer != NULL) {
			oldServer->httpSessionMap->del(oldSession->socketFD);
		}
		HttpClient * oldClient = (HttpClient *) this->httpClientMap->del(oldSession->socketFD);
		if (oldClient != NULL) {
			this->clearHttpClient(oldClient);
//			this->keepAliveHttpSessionMap->del(oldSession->remoteKey->char_string, oldSession->remoteKey->length);
		}
//		helper->freePort(this->client_local_port_queue, oldClient->port);
		//todo free the memory
		return 1;
	}
	return -1;
}

float OpenHttp::sendProgress(int clientID) {
	HttpClient * httpClient = (HttpClient *) this->clientID_httpClientMap->get(clientID);
	if (httpClient != NULL) {
		return httpClient->getSendProgress();
	}

	return -1;
}

int OpenHttp::startConnect(HttpClient * httpClient) {
	int socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFD < 0) {
		int systemError = getNewSystemError();
		helper->reportError2Client(httpClient, (char *) "StartConnection socketFD fail", systemError);
		return -1;
	}

	httpClient->socketFD = socketFD;

//	httpClient->port = helper->getAvailablePort(this->client_local_port_queue);
//	httpClient->localPort = httpClient->port->number;

	httpClient->localAddress = (sockaddr_in *) JSMalloc(sizeof(struct sockaddr_in));
	httpClient->localAddress->sin_family = AF_INET;
	httpClient->localAddress->sin_port = 0;
	httpClient->localAddress->sin_addr.s_addr = htonl(INADDR_ANY);

	httpClient->remoteAddress = (sockaddr_in *) JSMalloc(sizeof(struct sockaddr_in));
	httpClient->remoteAddress->sin_family = AF_INET;

	httpClient->remoteAddress->sin_port = htons(httpClient->remotePort);
	httpClient->remoteAddress->sin_addr.s_addr = inet_addr(httpClient->remoteIP->char_string);

	int on = 1;
	setsockopt(httpClient->socketFD, IPPROTO_TCP, TCP_NODELAY, (void *) &(on), sizeof(int)); //disable Nagle Algorithm
	setsockopt(httpClient->socketFD, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)); //reuse the TIME_WAIT socket

	struct timeval timeout = { 1, 0 };
	setsockopt(httpClient->socketFD, SOL_SOCKET, SO_SNDTIMEO, (char *) &timeout, sizeof(timeout));
	setsockopt(httpClient->socketFD, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout));

	int keepAlive = 1;
	int keepIdle = 20;
	int keepInterval = 5;
	int keepCount = 3;

//	unsigned int timeout1 = 10000; // 10s
//	setsockopt(httpClient->socketFD, IPPROTO_TCP, TCP_USER_TIMEOUT, &timeout1, sizeof(timeout1));

	setsockopt(httpClient->socketFD, SOL_SOCKET, SO_KEEPALIVE, (void *) &keepAlive, sizeof(keepAlive));
	setsockopt(httpClient->socketFD, SOL_TCP, TCP_KEEPIDLE, (void*) &keepIdle, sizeof(keepIdle));
	setsockopt(httpClient->socketFD, SOL_TCP, TCP_KEEPINTVL, (void *) &keepInterval, sizeof(keepInterval));
	setsockopt(httpClient->socketFD, SOL_TCP, TCP_KEEPCNT, (void *) &keepCount, sizeof(keepCount));

	while (-1 == bind(httpClient->socketFD, (sockaddr *) httpClient->localAddress, sizeof(sockaddr_in))) {
		int systemError = getNewSystemError();
		helper->reportError2Client(httpClient, (char *) "bind fail ", systemError);
		return -1;
	}
//	helper->log("bind ok !");

	//set socket to Non Block mode
	int flags = fcntl(httpClient->socketFD, F_GETFL, 0);
	flags |= O_NONBLOCK;
	fcntl(httpClient->socketFD, F_SETFL, flags);

	httpClient->event = new epoll_event();
	httpClient->event->data.fd = (httpClient->socketFD);
	httpClient->event->events = EPOLLIN | EPOLLOUT | EPOLLET; //(ET/LT) set ET, epoll triggered at the edge of the state change.

	epoll_ctl(this->epollFD, EPOLL_CTL_ADD, httpClient->socketFD, httpClient->event);

	int status = connect(httpClient->socketFD, (sockaddr *) httpClient->remoteAddress, sizeof(sockaddr_in));

	if (status != 0) {
		int systemError = getNewSystemError();
		if (systemError == EINPROGRESS) {
		} else { //EADDRNOTAVAIL 99
			helper->reportError2Client(httpClient, (char *) "Connect server fail", systemError);
			return -1;
		}
	}
	return 0;
}

int OpenHttp::createServer(HttpServer * httpServer) {
	int error = startListen(httpServer);
	if (error != -1) {
		this->httpServerMap->set(httpServer->socketFD, httpServer);
	}
	return 1;
}

int OpenHttp::startListen(HttpServer * httpServer) {

	int socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFD < 0) {
		//todo reportError2Server
		return -1;
	}

	httpServer->socketFD = socketFD;

	httpServer->localAddress = (sockaddr_in *) JSMalloc(sizeof(struct sockaddr_in));
	httpServer->localAddress->sin_family = AF_INET;
	httpServer->localAddress->sin_port = htons(httpServer->localPort);
	httpServer->localAddress->sin_addr.s_addr = htonl(INADDR_ANY);

	int on = 1;
	setsockopt(httpServer->socketFD, IPPROTO_TCP, TCP_NODELAY, (void *) &(on), sizeof(int)); //disable Nagle Algorithm
	setsockopt(httpServer->socketFD, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)); //reuse the TIME_WAIT socket

	if (-1 == bind(httpServer->socketFD, (sockaddr *) httpServer->localAddress, sizeof(sockaddr_in))) {
		JSLog("bind fail !");
		//todo reportError2Server
		return -1;
	}
	JSLog("bind ok !");

	//set socket to Non Block mode
	int flags = fcntl(httpServer->socketFD, F_GETFL, 0);
	flags |= O_NONBLOCK;
	fcntl(httpServer->socketFD, F_SETFL, flags);

	httpServer->event = new epoll_event();
	httpServer->event->data.fd = (httpServer->socketFD);
	httpServer->event->events = EPOLLIN | EPOLLOUT | EPOLLET; //(ET/LT) set ET, epoll triggered at the edge of the state change.

	epoll_ctl(this->epollFD, EPOLL_CTL_ADD, httpServer->socketFD, httpServer->event);

	listen(httpServer->socketFD, helper->MAXSOCKET);
	//todo reportError2Server

	return 1;
}
float OpenHttp::getSendProgress(HttpSession * session) {
	float sendProgress = -1;
	if (session->status->state != session->status->KeepAlive) {
		sendProgress = -2;
	} else if (session->status->sendState == session->status->SendPending) {
		sendProgress = 0;
	} else if (session->status->sendState == session->status->Sending) {
		ioctl(session->socketFD, SIOCOUTQ, &(session->linuxQueueLength));
		sendProgress = (float) (session->sentLength - session->linuxQueueLength) / (float) (session->send_HeaderLength + session->send_ContentLength);
	} else if (session->status->sendState == session->status->Sent) {
		ioctl(session->socketFD, SIOCOUTQ, &(session->linuxQueueLength));
		sendProgress = (float) (session->sentLength - session->linuxQueueLength) / (float) (session->send_HeaderLength + session->send_ContentLength);
	}
	return sendProgress;
}

float OpenHttp::getReceiveProgress(HttpSession * session) {
	float receiveProgress = -1;
	if (session->status->state != session->status->KeepAlive) {
		receiveProgress = -2;
	} else if (session->status->receiveState == session->status->ReceiveWaiting) {
		receiveProgress = 0;
	} else if (session->status->receiveState == session->status->Receiving) {
		receiveProgress = (float) (session->received_ContentLength) / (float) (session->receive_ContentLength);
	} else if (session->status->receiveState == session->status->Received) {
		receiveProgress = 1;
	}

	return receiveProgress;
}

void OpenHttp::closeFile(JSString * filePath) {
	JSNumber * js_fd = (JSNumber *) this->fileMap->del((void *) filePath->char_string, filePath->length);
	if (js_fd != NULL) {
		int FD = js_fd->number;
		if (FD > 0) {
			close(FD);
		}
	}
}

void OpenHttp::sendMessage(HttpSession * session) {
	if (session->status->sendState != session->status->SendPending) {
		//put message into a waiting queue
		session->pending_message_queue->offer(session->send_pending_message);
		session->send_pending_message = NULL;
		return;
	}

	if (session->send_pending_message != NULL) {
		session->send_message = session->send_pending_message;
		session->send_pending_message = NULL;
	} else {
		session->send_message = (Message *) session->pending_message_queue->take();
	}

	if (session->send_message == NULL) {
		this->sendComplete(session);
		return;
	}

	session->status->sendState = session->status->Sending;
	session->sentLength = 0;

	Message * message = session->send_message;
	if (message->type == 1 || message->type == 2) {
		message->header = session->sendHeaderBuffer;
		helper->stringifyMessageHeader(message);

		session->sendHeaderBuffer = message->header;
		session->send_HeaderLength = message->headerLength;

	} else if (message->type == 3) {
		unsigned char* encodeBuffer;
		unsigned int encodeLength;
		ws_encode(WS_FRAME_TYPE_TEXT, (unsigned char*) message->frameStr, message->frameLength, &encodeBuffer, &encodeLength);

		session->send_HeaderLength = 0;
		message->data = (char *) encodeBuffer;
		message->contentLength = encodeLength;
	}

	if (message->sendFilePath == NULL) {
		session->sendDataBuffer = message->data;
		session->send_ContentLength = message->contentLength;
		session->sendFD = -1;
	} else {
		//TO DO files buffer pool

		JSNumber * js_fd = (JSNumber *) this->fileMap->get((void *) message->sendFilePath->char_string, message->sendFilePath->length);
		if (js_fd != NULL) {
			session->sendFD = js_fd->number;
		} else {
			session->sendFD = open(message->sendFilePath->char_string, O_RDWR, 777);
			if (session->sendFD < 0) {
				helper->reportError2Session(getNewSystemError(), (char *) "Upload File,Can not open", session);
				JSLog("Upload File,Can not open !");
				return;
			}

			JSNumber * js_fd = new JSNumber(session->sendFD);
			this->fileMap->set((void *) message->sendFilePath->char_string, message->sendFilePath->length, js_fd);
		}

		session->sendFileStart = message->sendFileStart;
		session->send_FileLength = message->contentLength;

		session->send_ContentLength = message->contentLength;

		int n = errno;

		session->sendFileBuffer = (char *) mmap(NULL, session->send_FileLength, PROT_READ, MAP_SHARED, session->sendFD, session->sendFileStart);
		if ((session->sendFileBuffer) == (void *) -1) {
			int n = errno;
			JSString jsString;
			JSLog(jsString * "httpEntity->sendFileBuffer mmap Failed   errno=@@  session->sendFileStart=@@ session->send_FileLength=@@ " % n % session->sendFileStart % session->send_FileLength);
			helper->reportError2Session(getNewSystemError(), (char *) "SendFileBuffer mmap file failed", session);
			return;
		}
	}
	if (message->receiveFilePath == NULL) {
		session->receiveFD = -1;
	} else {
		session->receiveFD = open(message->receiveFilePath->char_string, O_CREAT | O_RDWR, 777);

		if (session->receiveFD < 0) {
			helper->reportError2Session(getNewSystemError(), (char *) "Download File,Can not open,file path may not exists", session);
			return;
		}

		ftruncate(session->receiveFD, 1);
	}

	if (session->status->state == session->status->KeepAlive || session->status->state == session->status->WebSocket) {
		this->sendPackege(session);
	}

}

void HttpClient::sendRequest(Message * message) {
	OpenHttp * openHttp = OpenHttp::getInstance();
	if (this->socketFD == -1) {
		this->pending_message_queue->offer(message);
	} else if (this->httpClientSession == NULL) {
		this->pending_message_queue->offer(message);
	} else {
		int flag = 1;
		while (flag == 1) {
			Message * send_pending_message = (Message *) this->pending_message_queue->take();
			if (send_pending_message == NULL) {
				break;
			}

			this->httpClientSession->sendRequest(send_pending_message);
		}
		if (message != NULL) {
			this->httpClientSession->sendRequest(message);
		}
	}
}

float HttpClient::getSendProgress() {
	OpenHttp * openHttp = OpenHttp::getInstance();
	if (this->httpClientSession != NULL) {
		return this->httpClientSession->getSendProgress();
	}
	return -1;
}
float HttpClientSession::getSendProgress() {
	OpenHttp * openHttp = OpenHttp::getInstance();
	float sendProgress = openHttp->getSendProgress(this);
	return sendProgress;
}
void HttpClientSession::sendRequest(Message * message) {
	OpenHttp * openHttp = OpenHttp::getInstance();
	if (message == NULL) {
		return;
	}
	message->type = 1;
	this->send_pending_message = message;
	openHttp->sendMessage(this);
}

void HttpServerSession::sendResponse(Message * message) {
	message->type = 2;
	this->send_pending_message = message;
	OpenHttp * openHttp = OpenHttp::getInstance();
	openHttp->sendMessage(this);
}

void HttpServerSession::sendMessageFrame(char * frameStr, int frameLength) {

	Message * message = new Message();
	message->type = 3;
	message->frameStr = frameStr;
	message->frameLength = frameLength;
	this->send_pending_message = message;

	OpenHttp * openHttp = OpenHttp::getInstance();
	openHttp->sendMessage(this);
}

void OpenHttp::sendPackege(HttpSession * session) {

	if (session->send_message == NULL) {
		return;
	}
	int flag = 1;
	while (flag == 1) {
		this->resolveSendPackage(session);
		if (session->sendSize == 0) {
			if (session->sentLength >= session->send_HeaderLength + session->send_ContentLength) {
				session->status->sendState = session->status->Sent;

				if (session->send_message->type == 3) {
					session->status->sendState = session->status->SendPending;
				}

				if (session->type == 1) { //when server sent the response, reset the status for next message.
					if (session->status->receiveState == session->status->Received) {
						session->status->sendState = session->status->SendPending;
						session->status->receiveState = session->status->ReceiveWaiting;
						this->sendComplete(session);
					}
				}

				break;
			} else {
				JSLog("continue @@ error");
//				break;
			}
		}

		session->sentPackegeLength = send(session->socketFD, session->sendBuffer, session->sendSize, 0);
		if (session->sentPackegeLength <= 0) {
			int systemError = getNewSystemError();
			if (systemError == EAGAIN) {
				break;
			} else if (systemError == 0) {
				JSLog("Break @@ errno=0");
				break;
			} else {
				JSLog("Break @@ sendPackege");
				helper->reportError2Session(systemError, NULL, session);
				break;
			}
//			} else {
//				if (session->MAXError > 0) {
//					session->MAXError--;
//					continue;
//				} else {
//					JSLog("Send Break @ MAXError @ errno");
//					break;
//				}
//			}
		} else {
			session->sentLength += session->sentPackegeLength;
		}
	}
}

int lastSystemError = 0;
bool isLastError = false;

int getNewSystemError() {
	int newSystemError = errno;
	isLastError = false;
	errno = 0;
	if (newSystemError == lastSystemError) {
		isLastError = true;
	}

	return newSystemError;
}

void OpenHttp::receivePackage(HttpSession * session) {

	int flag = 1;
	while (flag == 1) {
		if (session->status->state == session->status->Closed) {
			break;
		}
		session->receivePackageLength = recv(session->socketFD, session->receiveBuffer, helper->MaxBufflen, 0);
		if (session->receivePackageLength < 0) {
			int systemError = getNewSystemError();
			if (systemError == EAGAIN) {
				break;
			} else if (systemError == 0) {
				JSLog("---break@-----0");
				break;
			} else {
				helper->reportError2Session(systemError, NULL, session);
				break;
			}
//			} else {
//				if (session->MAXError > 0) {
//					session->MAXError--;
//					continue;
//				} else {
//					JSLog("Receive Break @ MAXError @ errno");
//					break;
//				}
//			}
		}

		else if (session->receivePackageLength == 0) { ///* Connection closed by peer */
			if (session->status->receiveState != session->status->Received) {
//				JSLog("/* Connection closed by peer unexpected */");
				int systemError = getNewSystemError();
				if (systemError == 0) {
					systemError = 10400;
				}
				helper->reportError2Session(systemError, NULL, session);
			} else {
				helper->reportError2Session(10808, (char *) "Connection closed by peer", session);
			}

			break;
		}
		this->resolveReceivePackage(session);
	}

}

void OpenHttp::resolveSendPackage(HttpSession * session) {

	Message * message = session->send_message;

	if (session->sentLength < session->send_HeaderLength) { //send header
		session->sendBuffer = session->sendHeaderBuffer + session->sentLength;
		session->sendSize = helper->PackegeSize;
		if ((helper->PackegeSize + session->sentLength) > session->send_HeaderLength) {
			session->sendSize = session->send_HeaderLength % helper->PackegeSize;
			//todo if helper->PackegeSize-session->sendSize>session->send_ContentLength, merge the sendHeaderBuffer and sendDataBuffer into one package.
		}
	} //send header
	else if (session->sentLength < session->send_HeaderLength + session->send_ContentLength) { //send content
		session->sendSize = helper->PackegeSize;

		if (session->sendFD == -1) {
			session->sendBuffer = session->sendDataBuffer + session->sentLength - session->send_HeaderLength;
		} else {
			session->sendBuffer = session->sendFileBuffer + session->sentLength - session->send_HeaderLength;
		}
		session->sendSize = helper->PackegeSize;
		if ((helper->PackegeSize + session->sentLength) > session->send_HeaderLength + session->send_ContentLength) {
			session->sendSize = session->send_HeaderLength + session->send_ContentLength - session->sentLength;
		}
	} //send content
	else {
		session->sendSize = 0;
	}
}

void OpenHttp::resolveReceivePackage(HttpSession * session) {

	if (session->status->state == session->status->KeepAlive) { //HTTP request
		if (session->status->receiveState == session->status->ReceiveWaiting) { //first package
			this->resolveFirstPackage(session);
		} //first package

		else if (session->status->receiveState == session->status->Receiving) { //following packages
			this->resolvefollowingPackages(session);
		} //following packages
	} //HTTP request
	else if (session->status->state == session->status->WebSocket) { //WebSocket request
		Message * message = helper->parseMessageHeader(session->receiveBuffer, session->receivePackageLength);
		session->websocket_frame_message = message;

		if (message->type == 3) { //3: message
			char * frameStr = (char *) "['command':'log','data':'abc']";
			ws_ctx_t * ws = ws_create();
			unsigned char* decodeBuffer;
			unsigned int decodeLength;
			unsigned int handledLength;
			ws_decode(ws, (unsigned char*) message->data, message->contentLength, &decodeBuffer, &decodeLength, &handledLength);
			if (decodeLength > 0) {
				message->data = (char *) decodeBuffer;
				HttpCallback * httpCallback = (HttpCallback *) session->httpCallback;
				httpCallback->onMessage((HttpServerSession *) session);
			}

		} else if (message->type == 4) { //4: websocket
			//todo close the websocketWebSocket
		}
	} //WebSocket request

	if (session->receive_message != NULL && session->receive_message->type == 4) { //4: websocket create connection
		if (session->type == 1) { //ServerSession
			if (session->status->state == session->status->KeepAlive) { //HTTP request
				if (session->status->receiveState == session->status->ReceiveWaiting) { //first package
					JSString * clientKey = session->receive_message->Sec_WebSocket_Key;
					helper->webSocketAcceptKey(clientKey->char_string, clientKey->length, helper->Sec_WebSocket_Accept);

					JSString response = (helper->responseTemplate %= helper->Sec_WebSocket_Accept);

					int sentPackegeLength = send(session->socketFD, response.char_string, response.length, 0);

					session->status->state = session->status->WebSocket;
				} //4: websocket
			} //first package
		} //HTTP request
	} //ServerSession

}

void OpenHttp::resolveFirstPackage(HttpSession * session) {
	Message * message = helper->parseMessageHeader(session->receiveBuffer, session->receivePackageLength);
	session->receive_message = message;
	if (message->type != 1 && message->type != 2) { //1:request//1:response
		return;
	}
	if (message->contentLength == -1) {
		session->status->receiveState = session->status->Receiving;
		this->receiveComplete(session);
	} else if (message->contentLength + message->headerLength <= session->receivePackageLength) {
		session->status->receiveState = session->status->Receiving;

		session->receive_ContentLength = message->contentLength;
		session->received_HeaderLength = message->headerLength;
		session->received_ContentLength = session->receivePackageLength - session->received_HeaderLength;
		if (session->receiveFD == -1) {
			helper->mapReceiveData(session);
		} else {
			helper->mapReceiveFile(session);
		}
		this->receiveComplete(session);
	} else {
		session->status->receiveState = session->status->Receiving;

		session->receive_ContentLength = message->contentLength;
		session->received_HeaderLength = message->headerLength;

		session->received_PackagesNumber = 1;
		session->received_ContentLength = session->receivePackageLength - session->received_HeaderLength;
		if (session->receiveFD == -1) {
			helper->mapReceiveData(session);
		} else {
			helper->mapReceiveFile(session);
		}

		session->receiveBuffer = session->receiveBuffer + session->receivePackageLength - message->headerLength;
	}

}

void OpenHttp::resolvefollowingPackages(HttpSession * session) {
	if (session->received_ContentLength + session->receivePackageLength < session->receive_ContentLength) {
		session->received_PackagesNumber++;
		session->received_ContentLength += session->receivePackageLength;

		session->receiveBuffer = session->receiveBuffer + session->receivePackageLength;
	} else {
		this->receiveComplete(session);
	}
}

void OpenHttp::receiveComplete(HttpSession * session) {
	if (session->receiveFD == -1) {
	} else {
		helper->unMapReceiveFile(session);
		session->receiveFD = -1;
	}
	session->status->receiveState = session->status->Received;

	if (session->type == 1) {
		HttpServerSession* httpServerSession = (HttpServerSession*) session;

		HttpCallback * httpCallback = (HttpCallback *) session->httpCallback;
		httpCallback->onRequest((HttpServerSession *) session);

	} else if (session->type == 2) {
		HttpCallback * httpCallback = (HttpCallback *) session->httpCallback;
		Message * receive_message = session->receive_message;
		if (helper->E200_Mark == receive_message->stateCode) {
			httpCallback->onResponce((HttpClientSession *) session);
		} else {

			int stateCode = parseStringToNubmer(receive_message->stateCode, 3);
			ErrorMessage * errorMessage = new ErrorMessage(stateCode);

			(*errorMessage->errorMessage) = "statteCode != 200";
			errorMessage->clientID = ((HttpClientSession *) session)->clientID;
			errorMessage->header = receive_message->header;
			errorMessage->headerLength = receive_message->headerLength;
			errorMessage->body = receive_message->data;
			errorMessage->bodyLength = receive_message->contentLength;
			httpCallback->onFailure(errorMessage);

		}

	}

	memset(session->defaultReceiveBuffer, 0, 1024);
	session->receiveBuffer = session->defaultReceiveBuffer;
	session->receiveDataBuffer = NULL;
	session->receiveFileBuffer = NULL;

	session->received_PackagesNumber = 0;

	if (session->type == 2) { //when client has received the response, reset the status for the next message.
		if (session->status->sendState == session->status->Sent) {
			session->status->sendState = session->status->SendPending;
			session->status->receiveState = session->status->ReceiveWaiting;

			session->sentLength = 0;
			session->sentPackegeLength = 0;

			if (session->receive_message != NULL) {
				JSON* headerMap = session->receive_message->headerMap;
				JSString * connection = (JSString *) headerMap->get(helper->Connection_Mark);
				if (connection != NULL && connection->type == JSSTRING && (*connection) == "keep-alive-4") {
					//send the messages in the pending_message_queue
					this->sendMessage(session);
				} else {
//					JSString jsString;
//					JSLog(jsString * "clearSession 2, session->socketFD=@@  clientID=@@.\r\n" % session->socketFD % ((HttpClientSession *) session)->clientID);
					this->clearSession(session);
				}
			}
		}
	}
}

void OpenHttp::sendComplete(HttpSession * session) {
	if (session->type == 1) {
		bool needClearSession = true;
		if (session->receive_message != NULL) {
			JSON* headerMap = session->receive_message->headerMap;
			JSString * connection = (JSString *) ~(*headerMap)[helper->Connection_Mark];
			if ((*connection) == helper->keep_alive_Mark) {
//					needClearSession = false;
			}
		}
		if (needClearSession == true) {
			this->clearSession(session);
		}
		//end the session when server has sent the response.
	} else if (session->type == 2) {
		//put the keepAlive session into the keepAliveHttpSessionMap reuse
		if (session->status->state == session->status->KeepAlive) {
			JSLog("put the keepAlive session into the keepAliveHttpSessionMap reuse");
//			this->keepAliveHttpSessionMap->set(session->remoteKey->char_string, session->remoteKey->length, session);
			this->httpClientMap->del(session->socketFD);
			startNextClient();
		} else {
			JSLog("clearSession 3");
			this->clearSession(session);
		}
	}
}

void OpenHttp::startNextClient() {
	HttpClient * httpClient = (HttpClient *) this->httpClientQueue->take();

	if (httpClient != NULL) {
		if (httpClient->state == 2 && httpClient->needAbort == false) {
			this->createClient(httpClient);
		} else {
			if (httpClient->needAbort == true) {
				helper->reportError2Client(httpClient, (char *) "HttpClient need Abort", 10807);
			} else if (httpClient->state != 2) {
				helper->reportError2Client(httpClient, (char *) "HttpClient state error", 10808);
				JSLog("httpClient->state != 2");
				JSLog(httpClient->state);
			}
		}
	}
}

void OpenHttpHelper::mapReceiveData(HttpSession * session) {
	if (session->receiveDataBuffer == NULL) {
		session->receiveDataBuffer = (char *) JSMalloc(session->receive_ContentLength);
		memcpy(session->receiveDataBuffer, session->receiveBuffer + session->received_HeaderLength, session->received_ContentLength);
	}
	session->receiveBuffer = session->receiveDataBuffer;
}

void OpenHttpHelper::unMapReceiveFile(HttpSession * session) {
	if (session->receiveFileBuffer != NULL) {
		JSFree(session->receiveFileBuffer, session->receive_ContentLength, session->receiveFD, session->receive_ContentLength);
		session->receiveFileBuffer = NULL;
	}
}

void OpenHttpHelper::mapReceiveFile(HttpSession * session) {
	if (session->receiveFileBuffer == NULL) {
		session->receiveFileBuffer = (char *) JSMalloc(session->receive_ContentLength, session->receiveFD, session->receiveOffset);
		memcpy(session->receiveFileBuffer, session->receiveBuffer + session->received_HeaderLength, session->received_ContentLength);
	}
	session->receiveBuffer = session->receiveFileBuffer;
}

void OpenHttpHelper::parseErrorMessage(ErrorMessage * errorMessage, int error) {
	//EINTR ETIMEOUT EAGAIN EPIPE ECONNREFUSED ECONNRESET ECONNABORTED EHOSTTUNREACH ENETUNREACH ENETDOWN
	JSString *jsString = errorMessage->errorMessage;
	if (error == EINTR) {
		errorMessage->stateCode = 701;
		*(jsString) = "Interrupted system call";
		//Seems this error will not happen in the modern linux with socket works at Non Block mode
	} else if (error == ETIMEDOUT) {
		errorMessage->stateCode = 702;
		*(jsString) = "Connection timed out";
	} else if (error == EPIPE) {
		errorMessage->stateCode = 703;
		(*jsString) = "Broken pipe";
	} else if (error == ECONNREFUSED) {
		errorMessage->stateCode = 704;
		(*jsString) = "Connection refused";
	} else if (error == ECONNRESET) {
		errorMessage->stateCode = 705;
		(*jsString) = "Connection reset by peer";
	} else if (error == ECONNABORTED) {
		errorMessage->stateCode = 706;
		(*jsString) = "Software caused connection abort";
	} else if (error == EHOSTUNREACH) {
		errorMessage->stateCode = 707;
		(*jsString) = "No route to host";
	} else if (error == ENETUNREACH) {
		errorMessage->stateCode = 708;
		(*jsString) = "Network is unreachable";
	} else if (error == ENETDOWN) {
		errorMessage->stateCode = 801;
		(*jsString) = "Network is down";
	} else if (error == EBADF) {
		errorMessage->stateCode = 802;
		(*jsString) = "Bad file number";
	} else if (error == 0) {
		errorMessage->stateCode = 803;
		(*jsString) = "zero error";
	} else if (error == 10400) {
		errorMessage->stateCode = 804;
		(*jsString) = "Connection closed by peer unexpected";
	} else if (error == 10401) {
		errorMessage->stateCode = 805;
		(*jsString) = "Socket connect timeout";
	} else if (error == 10402) {
		errorMessage->stateCode = 806;
	} else if (error == 10403) {
		errorMessage->stateCode = 901;
		(*jsString) = "Socket connect aborted";
	} else {
		errorMessage->stateCode = 850;
		(*jsString) = "Other error:";
		(*jsString) + error;
//			JSString errString;
//			errString + "other error, error:" + error;
//			JSLog(errString);
	}
}

void OpenHttpHelper::reportError2Client(HttpClient * httpClient, char *errorMessageStr, int systemError) {
	ErrorMessage * errorMessage = new ErrorMessage();
	errorMessage->clientID = httpClient->clientID;

	this->parseErrorMessage(errorMessage, systemError);

	if (errorMessageStr != NULL) {
		(*errorMessage->errorMessage) * " [@@] " % errorMessageStr;
	}

	HttpCallback * httpCallback = httpClient->httpCallback;

	httpCallback->onFailure(errorMessage);

	OpenHttp * openHttp = OpenHttp::getInstance();
	openHttp->clearHttpClient(httpClient);
}

void OpenHttpHelper::reportError2Session(int systemError, char *errorMessageStr, HttpSession * session) {

	ErrorMessage * errorMessage = new ErrorMessage();
	errorMessage->clientID = ((HttpClientSession *) session)->clientID;

	this->parseErrorMessage(errorMessage, systemError);

	if (errorMessageStr != NULL) {
		(*errorMessage->errorMessage) * " [@@] " % errorMessageStr;
	}

	HttpCallback * httpCallback = (HttpCallback *) session->httpCallback;
	httpCallback->onFailure(errorMessage);

	OpenHttp * openHttp = OpenHttp::getInstance();
	openHttp->clearSession(session);
}

void * epollLooperThread(void *arg) {
	OpenHttp *openHttp = OpenHttp::getInstance();
	openHttp->epollLooper(openHttp->epollFD);
	return (void *) 1;
}

void OpenHttp::epollLooper(int epollFD) {

	this->epoll_events = (epoll_event*) JSMalloc(helper->MaxEvent * sizeof(epoll_event));
	int numEvents = 0;
	JSLog("epollLooper started ! ");
	while (true) {

		int wait_timeout = 1000;
		if (this->httpClientMap->length > 0 || this->httpSessionMap->length > 0) {
			wait_timeout = 100;
		}
		numEvents = epoll_wait(this->epollFD, this->epoll_events, helper->MaxEvent, wait_timeout);

		for (int i = 0; i < numEvents; ++i) {
			epoll_event * event = this->epoll_events + i;
			if (event->events & EPOLLHUP) {
				JSLog("**************************event: EPOLLHUP**************************");
				int systemError = getNewSystemError();
				JSLog(systemError);
				//todo end the session when target end the session
			}
			if (event->events & EPOLLERR) {
				JSLog("**************************event: EPOLLERR**************************");
				int systemError = getNewSystemError();
				JSLog(systemError);
				//todo end the session when error has happened
			}

			if (event->events & EPOLLIN) { //EPOLLIN
				HttpServer * httpServer = (HttpServer *) this->httpServerMap->get(event->data.fd);
				HttpSession * session = (HttpSession *) this->httpSessionMap->get(event->data.fd);
				if (session != NULL) {
					if (session->needAbort == true) {
						helper->reportError2Session(10806, (char *) "httpSession need Abort", session);
					}
					if (session->status->state == session->status->KeepAlive || session->status->state == session->status->WebSocket) {
						if (session->status->receiveState == session->status->ReceiveWaiting || session->status->receiveState == session->status->Receiving) {
							this->receivePackage(session);
							this->checkProgress(session);
						}
					}
				} else if (httpServer != NULL) { //session is not NULL
					int connectFD = 88888;
					while (connectFD > 0) {
						connectFD = accept(httpServer->socketFD, NULL, NULL);

						if (connectFD < 0) {
							int systemError = getNewSystemError();
							if (systemError == EAGAIN) {
								continue;
							} else {
								JSLog("accept fail!");
								JSLog(systemError);
							}
						}

//						JSLog("accept ok!");
						HttpServerSession * newSession = new HttpServerSession(httpServer);

						newSession->socketFD = connectFD;

						//set socket to Non Block mode
						int flags = fcntl(newSession->socketFD, F_GETFL, 0);
						flags |= O_NONBLOCK;
						fcntl(newSession->socketFD, F_SETFL, flags);

						newSession->event = new epoll_event();
						newSession->event->data.fd = (newSession->socketFD);
						newSession->event->events = EPOLLIN | EPOLLOUT | EPOLLET; //(ET/LT) set ET, epoll triggered at the edge of the state change.

						this->httpSessionMap->set(newSession->socketFD, newSession);
						httpServer->httpSessionMap->set(newSession->socketFD, newSession);
						epoll_ctl(this->epollFD, EPOLL_CTL_ADD, newSession->socketFD, newSession->event);
						newSession->status->state = newSession->status->KeepAlive;
					}

				} else {
				}
			} //EPOLLIN
			if (event->events & EPOLLOUT) { //EPOLLOUT

				HttpClient * httpClient = (HttpClient *) this->httpClientMap->get(event->data.fd);
				HttpSession * session = (HttpSession *) this->httpSessionMap->get(event->data.fd);

				if (session != NULL) {
					if (session->needAbort == true) {
						helper->reportError2Session(10806, (char *) "httpSession need Abort", session);
					}

					if (session->status->state == session->status->KeepAlive || session->status->state == session->status->WebSocket) {
						if (session->status->sendState == session->status->Sending) {
							this->sendPackege(session);
							this->checkProgress(session);
						}
					}
				} else { //session is NULL
					if (httpClient != NULL) {

						int error = 0;
						socklen_t ilen = sizeof(error);
						int ret = getsockopt(event->data.fd, SOL_SOCKET, SO_ERROR, &error, &ilen);
						if (ret < 0) {
							JSLog("连接失败");
							helper->reportError2Client(httpClient, (char *) "Socket connect failed1", error);
						} else if (error != 0) {
							JSString jsString;
							JSLog(jsString * "连接失败: @@" % error);
							helper->reportError2Client(httpClient, (char *) "Socket connect failed2", error);
						} else if (httpClient->needAbort == true) {
							helper->reportError2Client(httpClient, (char *) "HttpClient need Abort", 10807);
						} else {
							JSString jsString = (char *) "连接成功 @@";
							JSLog(jsString % httpClient->clientID);

							HttpClientSession * newSession = new HttpClientSession(httpClient);

							newSession->socketFD = httpClient->socketFD;
							newSession->clientID = httpClient->clientID;
							newSession->timeout = httpClient->timeout;
							newSession->remoteKey = helper->getRemoteKey(httpClient->remotePort, httpClient->remoteIP);
							gettimeofday(newSession->last_update_iTimeval, NULL);

							this->httpSessionMap->set(newSession->socketFD, newSession);

							newSession->status->state = newSession->status->KeepAlive;

							httpClient->state = 4;

							httpClient->httpClientSession = newSession;

							if (httpClient->httpClientSession->status->sendState == httpClient->httpClientSession->status->SendPending) {
								httpClient->sendRequest(NULL);
							}
						}

					} else { //httpClient is NULL
//						Log((char *) ("error"));
					}
				}
			} //EPOLLOUT
		} //for

		if (numEvents == 0) {
			gettimeofday(helper->iTimeval, NULL);

			for (int i = 0, offset = 0; i < this->httpClientMap->length; i++) {
				JSKeyValue * js_KeyValue = (JSKeyValue *) this->httpClientMap->find(i - offset);
				HttpClient * httpClient = (HttpClient *) js_KeyValue->value;

				if (httpClient->needAbort == true) {
					helper->reportError2Client(httpClient, (char *) "HttpClient need Abort", 10807);
					offset++;
				}

				if (httpClient->state == 3) {
					long millisecond = (helper->iTimeval->tv_sec - httpClient->start_iTimeval->tv_sec) * 1000 + (helper->iTimeval->tv_usec - httpClient->start_iTimeval->tv_usec) / 1000;
					if (millisecond > httpClient->timeout) {
						JSString jsString = (char *) "clientID:@@ , timeout.@@";
						JSLog(jsString % httpClient->clientID % httpClient->timeout);
						helper->reportError2Client(httpClient, (char *) "Socket connect timeout", 10401);
						offset++;
					}
				}
			}

			struct tcp_info info;
			unsigned int tcp_info_length = sizeof(info);

//			JSString jsString = (char *) "httpSessionMap->length:@@";
//			JSLog(jsString % this->httpSessionMap->length);

			for (int i = 0, offset = 0; i < this->httpSessionMap->length; i++) {
				JSKeyValue * js_KeyValue = (JSKeyValue *) this->httpSessionMap->find(i - offset);
				HttpSession * httpSession = (HttpSession *) js_KeyValue->value;
				if (httpSession->status->state == httpSession->status->KeepAlive || httpSession->status->state == httpSession->status->WebSocket) {
					if (httpSession->needAbort == true) {
						helper->reportError2Session(10806, (char *) "httpSession need Abort", httpSession);
						offset++;
						continue;
					}
					getsockopt(httpSession->socketFD, SOL_TCP, TCP_INFO, (void *) &info, (socklen_t *) &tcp_info_length);
					if (info.tcpi_state > 3) { //!= TCP_ESTABLISHED
						JSLog("not TCP_ESTABLISHED");
						JSString jsString = (char *) "socketFD:@@ ,  not TCP_ESTABLISHED. tcpi_state=";
						jsString + info.tcpi_state;
						JSLog(jsString % httpSession->socketFD);
						int systemError = getNewSystemError();
						helper->reportError2Session(systemError, (char *) "KeepAlive session is not TCP_ESTABLISHED", httpSession);
						offset++;
					} else {
						bool isUpdated = this->checkProgress(httpSession);
						if (isUpdated != true) {
							if (httpSession->status->sendState != httpSession->status->SendPending) {
								if (httpSession->updata_interval > httpSession->timeout) {
									JSString jsString = (char *) "socketFD:@@ , keep_alive_timeout.clientID:@@, httpSession->timeout: @@";
									JSLog(jsString % httpSession->socketFD % ((HttpClientSession *) httpSession)->clientID % httpSession->timeout);
									helper->reportError2Session(10402, (char *) "KeepAlive session is timeout", httpSession);
									offset++;
								}
							}
						}
					}
				}
			}
		}
		///
	} //while
}

bool OpenHttp::checkProgress(HttpSession * httpSession) {

	if (httpSession->type == 1) {
		return false;
	}

	bool isUpdated = false;

	float sendProgress = this->getSendProgress(httpSession);
	float receiveProgress = this->getReceiveProgress(httpSession);

	if (httpSession->sendProgress != sendProgress) {
		isUpdated = true;
	}
	if (httpSession->receiveProgress != receiveProgress) {
		isUpdated = true;
	}

	long interval = (helper->iTimeval->tv_sec - httpSession->last_update_iTimeval->tv_sec) * 1000 + (helper->iTimeval->tv_usec - httpSession->last_update_iTimeval->tv_usec) / 1000;

	if (isUpdated == true) {
//		if (interval > httpSession->min_updata_interval) {
		if (httpSession->status->state == httpSession->status->KeepAlive) {

			httpSession->sendProgress = sendProgress;
			httpSession->receiveProgress = receiveProgress;

			obverse_memcpy_slow(httpSession->last_update_iTimeval, helper->iTimeval, sizeof(timeval));

			HttpCallback * httpCallback = (HttpCallback *) httpSession->httpCallback;
			httpCallback->onUpdateState((HttpClientSession*) httpSession);
		}
//		}
	} else {
		httpSession->updata_interval = interval;
	}

	return isUpdated;
}

char * OpenHttpHelper::stringifyMessageHeader(Message * message) {
	int index = 0;
	int length = 0;
	char * firstLine;
	if (message->type == 1) {
		length = strlen(message->method);
		memcpy(message->header + index, message->method, length);
		index += length;
		*(message->header + index) = 32;
		index += 1;

		length = strlen(message->urlPath);
		memcpy(message->header + index, message->urlPath, length);
		index += length;
		*(message->header + index) = 32;
		index += 1;

		firstLine = (char *) "HTTP/1.1\r\n";
	} else if (message->type == 2) {
		firstLine = (char *) "HTTP/1.1 200 OK\r\n";
	}

	length = strlen(firstLine);
	memcpy(message->header + index, firstLine, length);
	index += length;

	char * ContentLengthLine = (char *) "Content-Length:";
	length = strlen(ContentLengthLine);
	memcpy(message->header + index, ContentLengthLine, length);
	index += length;

	char * ContentLengthNumber = (char *) JSMalloc(16);
	parseNubmerToString(message->contentLength, ContentLengthNumber);
	length = strlen(ContentLengthNumber);
	memcpy(message->header + index, ContentLengthNumber, length);
	index += length;

	*(message->header + index) = '\r';
	*(message->header + index + 1) = '\n';
	index += 2;

	if (message->headerMap != NULL) {
		message->headerMap->del(this->ContentLength_Mark);
		for (int i = 0; i < message->headerMap->length; i++) {
			JSKeyValue * jsKeyValue = (JSKeyValue *) message->headerMap->find(i);
			int length = jsKeyValue->keyLength;
			memcpy(message->header + index, jsKeyValue->key, length);
			index += length;

			*(message->header + index) = 58;
			index += 1;

			JSString * js_string = (JSString *) jsKeyValue->value;
			length = js_string->length;
			memcpy(message->header + index, js_string->char_string, length);
			index += length;

			*(message->header + index) = '\r';
			*(message->header + index + 1) = '\n';
			index += 2;
		}
	}
	if (message->followingHeader != NULL) {
		int followingHeaderLength = strlen(message->followingHeader);
		memcpy(message->header + index, message->followingHeader, followingHeaderLength);
		index += followingHeaderLength;
	}

	*(message->header + index) = '\r';
	*(message->header + index + 1) = '\n';
	*(message->header + index + 2) = 0;
	index += 2;
	message->headerLength = index;
	return message->header;

}

Message * OpenHttpHelper::parseMessage(char * buffer, int length) {
	Message * message = new Message();
	message->type = 3;
	message->data = buffer;
	message->contentLength = length;
	return message;
}
Message * OpenHttpHelper::parseMessageHeader(char * buffer, int length) {
	Message * message = new Message();
	char * point = buffer; //todo buffer is not under memory management, while its char values are hold by the message->headerMap.
	message->type = 3;
	for (int i = 0; i < 10; i++) {
		point++;
		if (*point == 32) {
			message->type = -1;
			break;
		}
	}
	if (message->type == 3) {
		message->data = buffer;
		message->contentLength = length;
		return message;
	}
	message->headerMap = new JSON();
	message->headerMap->initialize();
	message->headerMap->setKeyEnd(this->KeyEnd);
	char * lastLine = buffer;
	point = buffer;
	int lineNumber = 0;
	for (int i = 0; i < length - 1; i++) {
		point++;
		if (this->EndLine_Mark >= (point - 1)) {
			if (lineNumber == 0) {
				this->resolveFirstLine(lastLine, point - lastLine, message);
				if (message->type == -1) {
					return message;
				}
			} else {
				if (point - lastLine == 1) {
					message->headerLength = i + 2;
					message->data = message->header + message->headerLength;

					JSString * js_string = (JSString *) (message->headerMap->get(this->ContentLength_Mark));

					if (js_string != NULL && js_string->type == JSSTRING) {
						message->contentLength = parseStringToNubmer(js_string->char_string, js_string->length);
					} else {
						message->contentLength = -1;
						JSString * js_key = (JSString *) ~(*message->headerMap)[(char *) "Sec-WebSocket-Key"];
						if (js_key != NULL && js_key->type == JSSTRING) {
							message->type = 4;
							message->Sec_WebSocket_Key = js_key;
						}
					}
					break;
				} else {
					this->resolveLine(lastLine, point - lastLine - 2, lineNumber, message);
					if (message->type == -1) {
						return message;
					}
				}
			}

			lineNumber++;
			lastLine = point + 1;
		}
	}
	if (lineNumber <= 0) {
		message->data = buffer;
		message->contentLength = length;
		return message;
	}
	return message;
}

void OpenHttpHelper::resolveFirstLine(char * start, int length, Message * message) {
	char * point = start;
	if (this->HTTP_Mark == point) {
		message->type = 2;
		message->header = start;
	} else {
		message->type = 1;
	}
	int sectionNumber = 0;
	char * lastPoint = start;
	for (int i = 0; i < length; i++) {
		point++;
		if (*point == 32 || *point == '\r') {
			if (message->type == 2) {
				if (sectionNumber == 0) {
					message->version = lastPoint;
				} else if (sectionNumber == 1) {
					message->stateCode = lastPoint;
				} else if (sectionNumber == 2) {
					message->phrase = lastPoint;
				} else if (sectionNumber == 5) {
					message->type = -1;
					return;
				}
			} else if (message->type == 1) {
				if (sectionNumber == 0) {
					message->method = lastPoint;
				} else if (sectionNumber == 1) {
					message->urlPath = lastPoint;
				} else if (sectionNumber == 2) {
					message->version = lastPoint;
				} else if (sectionNumber == 5) {
					message->type = -1;
					return;
				}
			}

			lastPoint = point + 1;
			sectionNumber++;
		}
	}

	point = message->version;
	if (this->HTTP_Mark == point) {

	} else {
		message->type = -1;
	}
}

void OpenHttpHelper::resolveLine(char * start, int length, int lineNumber, Message * message) {
	JSON * headerMap;
	if (message->headerMap == NULL) {
		headerMap = new JSON();
		headerMap->initialize();
		headerMap->setKeyEnd(this->KeyEnd);

		message->headerMap = headerMap;
	} else {
		headerMap = message->headerMap;
	}

	char * point = start;
	bool isKeyValue = false;
	for (int i = 0; i < length; i++) {
		point++;
		if (*point == 58) {
			isKeyValue = true;
			JSString * js_string = new JSString(0);

			if (*(point + 1) == 32) {
				js_string->char_string = point + 2;
				js_string->length = length - i - 2;
			} else {
				js_string->char_string = point + 1;
				js_string->length = length - i - 1;
			}
			(*headerMap)[start] = js_string;
			break;
		}
	}
	if (isKeyValue == false) {
		message->type = -1;
	}
}
long OpenHttpHelper::getCurrentMillisecond() {
	gettimeofday(iTimeval, NULL);

	long millisecond = (iTimeval->tv_sec - start_iTimeval->tv_sec) * 1000 + (iTimeval->tv_usec - start_iTimeval->tv_usec) / 1000;
	return millisecond;
}

JSString * OpenHttpHelper::getRemoteKey(int remotePort, JSString * remoteIP) {
//	char * remotekey = (char*) JSMalloc(remoteIPLength + 5);
//
//	memcpy(remotekey, remoteIP, remoteIPLength);
//	memcpy(remotekey + remoteIPLength, &remotePort, 4);

	return NULL;
}

void OpenHttpHelper::webSocketAcceptKey(char * clientKey, int clientKeyLength, char * serverKey) {

	JSString * mergedKey = new JSString();
	*mergedKey ^= this->mask;

	mergedKey->format(clientKey, clientKeyLength);

	Sha1 * sha1 = Sha1::getInstance();
	sha1->initialize();
	char * sha1_result = (char *) sha1->encode(mergedKey->char_string, mergedKey->length);

	base64_encode(serverKey, sha1_result, SHA1_HASH_SIZE);

}
