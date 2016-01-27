#ifndef OPENHTTP_H
#define OPENHTTP_H

#include "../data_core/base/JSObject.h"
#include "../data_core/base/MemoryManagement.h"
#include "../data_core/base/List.h"
#include "../data_core/base/HashTable2.h"
#include "../data_core/base/Queue.h"
#include "../data_core/JSON.h"
#include "../data_core/JSCallback.h"

#include "../lib/Base64.h"
#include "../lib/LibSha1.h"

#include "../lib/wsproto.h"

#include <sys/socket.h>
#include <string.h>

#include <netinet/in.h>
#include <arpa/inet.h>

//#include <linux/tcp.h>

#include <linux/sockios.h>
#include <netinet/tcp.h>
#include <sys/time.h>

#include <fcntl.h>
#include <sys/ioctl.h>

#include <sys/epoll.h>
#include <pthread.h>

#include <errno.h>
#include <iostream>

#include "../lib/JSLog.h"

#include "../data_core/temp_string.h"

#ifndef NULL
#define NULL 0
#endif /* NULL */
#define STARTPORTNUMBER 10110

class Message: public JSObject {
public:
	Message();
	int type; //[1:request, 2:response, 3: message, 4:websocket, -1:illegal]
	//response
	char * version; //end by [\r,\n,32,0], the same to below
	char * stateCode;
	char * phrase;
	//request
	char * method;
	char * urlPath;

	JSON * headerMap; //key end by [32, 58], value end by [\r,\n]
	JSString * sendFilePath;
	long sendFileStart;
	long sendFileLength;

	JSString * receiveFilePath;

	char * data;

	char * frameStr;
	int frameLength;
	//for convenience
	int contentLength;
	int headerLength;
	char * header;
	char * followingHeader;
	char * Host;
	JSString * Sec_WebSocket_Key;
	//for receiving status

};

class SessionStatus {
public:
	SessionStatus();
	int Connected, KeepAlive, WebSocket, Closed;
	int ReceiveWaiting, Receiving, Received;
	int SendPending, Sending, Sent;
	int Failed, Timeout;
	int state;
	int sendState;
	int receiveState;
};

class HttpSession: public JSObject {
public:
	HttpSession();

	int type; //[1:ServerSession, 2:ClientSession]
	SessionStatus * status;
	bool needAbort;

	int socketFD;

	JSString * remoteKey;

	epoll_event * event;

	int MAXError;

	int timeout;
	int min_updata_interval;

	float sendProgress;
	float receiveProgress;

	timeval * last_update_iTimeval;
	int updata_interval;

	//*************************receive***********************

	int receivePackageLength;

	int received_PackagesNumber;
	int received_ContentLength;
	int received_HeaderLength;

	int receive_ContentLength;

	char * defaultReceiveBuffer;
	char * receiveBuffer; // recv data from socket

	int receiveFD; //file descriptor
	int receiveOffset; //file offset
	char * receiveFileBuffer; //mmap file buffer

	char * receiveDataBuffer; //raw data buffer stores content

	float receive_percent;
	Message * receive_message;

	Message * websocket_frame_message;

	//*************************receive***********************

	//*************************send**************************
	int send_HeaderLength;

	int send_ContentLength;
	int send_FileLength;

	char * sendHeaderBuffer; //1024;

	char * sendBuffer; // send data to socket
	unsigned int sendSize; //size of package next send

	int sentLength; //total sent length

	int linuxQueueLength;

	int sentPackegeLength; //size of package just sent

	int sendFD;
	char * sendFileBuffer;
	int sendFileStart;

	char * sendDataBuffer;

	Queue * pending_message_queue;

	Message * send_pending_message;
	Message * send_message;
//	void send();

	//*************************send**************************
	JSCallback * httpCallback;
};

class HttpServerSession: public HttpSession {
public:
	HttpServerSession(JSObject * httpServer);
	void sendResponse(Message * message);
	void sendMessageFrame(char * frameStr, int frameLength);

};

class HttpClientSession: public HttpSession {
public:
	HttpClientSession(JSObject * httpClient);
	int clientID;

	void sendRequest(Message * message);
	float getSendProgress();
};
class ErrorMessage {
public:
	ErrorMessage();
	ErrorMessage(int stateCode);
	ErrorMessage(int stateCode, char * string);
	~ErrorMessage();
	JSString *errorMessage;
	int stateCode;
	int clientID;
	char * header;
	int headerLength;
	char * body;
	int bodyLength;
};
class HttpCallback: public JSCallback {

public:
	HttpCallback();
	void (*onRequest)(HttpServerSession* thiz);
	void (*onMessage)(HttpServerSession* thiz);

	void (*onResponce)(HttpClientSession* thiz);
	void (*onFailure)(ErrorMessage * errorMessage);
	void (*onUpdateState)(HttpClientSession* thiz);
};

void doNothing(HttpSession* thiz);
class ServerStatus {
public:
	ServerStatus();
	int Starting, Listening, Closed;
	int state;
};

class HttpServer: public JSObject {
public:
	HttpServer();
	int localPort;
	sockaddr_in * localAddress;
	int socketFD;

	epoll_event * event;

	JSON * httpSessionMap;

	HttpCallback * httpCallback;
};

class HttpClient: public JSObject {
public:
	HttpClient();
	int state; //1:create 2:queuing 3:connecting 4:connected 5:closed 6:abort
	bool needAbort;
	JSObject * port;

	int timeout;

	timeval * start_iTimeval;

	int localPort;
	sockaddr_in * localAddress;
	int remotePort;
	JSString * remoteIP;
	sockaddr_in * remoteAddress;
	int socketFD;

	int receiveFD;

	epoll_event * event;

	HttpClientSession *httpClientSession;
	Queue * pending_message_queue;

	HttpCallback * httpCallback;
	int clientID;

	void sendRequest(Message * message);
	float getSendProgress();

//	void sendMessageFrame(char * frameStr);
};

class Status {
public:
	Status();
	int Queueing, Started, Connecting, Connected, Sending, Sent, Waiting, Receiving, Received;
	int Failed, Timeout;
	long time_queueing, time_started, time_connecting, time_connected, time_sending, time_sent, time_waiting, time_receiving, time_received;
	long time_failed, time_timeout;
	int state;
};

class OpenHttpHelper {
public:
	OpenHttpHelper();

	int startPortNumber;
	int portQueueSize;
	int minPortQueueSize;

	JSString ContentLength_Mark;
	JSString Connection_Mark;
	JSString HTTP_Mark;
	JSString EndLine_Mark;
	JSString Upgrade_Mark;
	JSString keep_alive_Mark;

	JSString E200_Mark;

	char * Sec_WebSocket_Accept;

	char * LineEnd;
	char * KeyEnd;

	int sendBuffSize;
	int PackegeSize;
	int MaxBufflen;
	int MAXSOCKET;
	int MaxEvent;

	int max_connecting_Client;
	int default_timeout;

	timeval * iTimeval;
	timeval * start_iTimeval;

	char * lineKey;
	char * lineValue;

	JSString responseTemplate;
	JSString mask;

	JSString * getRemoteKey(int remotePort, JSString * remoteIP);

	long getCurrentMillisecond();
	Message * parseMessage(char * buffer, int length);
	Message * parseMessageHeader(char * buffer, int length);
	char * stringifyMessageHeader(Message * message);
	void resolveFirstLine(char * start, int length, Message * message);
	void resolveLine(char * start, int length, int lineNumber, Message * message);

	void mapReceiveData(HttpSession * session);
	void mapReceiveFile(HttpSession * session);
	void unMapReceiveFile(HttpSession * session);

	bool checkSend(HttpSession * session, int sentPackegeLength);
	bool checkReceive(HttpSession * session, int receivePackegeLength);

	void webSocketAcceptKey(char * clientKey, int clientKeyLength, char * serverKey);

	void parseErrorMessage(ErrorMessage * errorMessage, int error);
	void reportError2Client(HttpClient * httpClient, char *errorMessageStr, int systemError);
	void reportError2Session(int systemError, char *errorMessageStr, HttpSession * session);

};

void *epollLooperThread(void *arg);

class OpenHttp {
public:
	OpenHttp();
	static OpenHttp *instance;
	OpenHttpHelper * helper;

	static OpenHttp * getInstance();

	bool is_initialized;

	//API----------------------------------------------------------------------------------------
	bool initialize();

	int createServer(HttpServer * httpServer);
	int createClient(HttpClient * httpClient);

	float sendProgress(int clientID);
	//API----------------------------------------------------------------------------------------

	int startListen(HttpServer * httpServer);
	int startConnect(HttpClient * httpClient);

	void startNextClient();
	int clearHttpClient(HttpClient * httpClient);
	int clearSession(HttpSession * session);

	HashTable2 *httpServerMap;
	JSON *httpClientMap;

	JSON *httpSessionMap;

	HashTable *keepAliveHttpSessionMap;
	HashTable2 *clientID_httpClientMap;

	Queue * httpClientQueue;

	HashTable *fileMap;
	void closeFile(JSString * filePath);

	void sendMessage(HttpSession * session);
	float getSendProgress(HttpSession * session);
	float getReceiveProgress(HttpSession * session);

	bool checkProgress(HttpSession * session);

	void sendPackege(HttpSession * session);
	void receivePackage(HttpSession * session);

	void resolveSendPackage(HttpSession * session);
	void resolveReceivePackage(HttpSession * session);

	void resolveFirstPackage(HttpSession * session);
	void resolvefollowingPackages(HttpSession * session);
	void receiveComplete(HttpSession * session);
	void sendComplete(HttpSession * session);

	int epollFD;
	epoll_event * epoll_events;
	pthread_t * epollLooperPthread;

	void epollLooper(int epollFD);
};

int getNewSystemError();

extern "C" {

extern void CallBack(int id, int type, const signed char * responseInfo, int partId);
}
#endif /* OPENHTTP_H */

