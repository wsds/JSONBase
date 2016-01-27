#ifndef PROCESS_H
#define PROCESS_H

#include <signal.h>
#include <sys/mman.h>
#include "fcntl.h"
#include <errno.h>
#include "unistd.h"
#include "../data_core/JSON.h"
#include "../lib/JSLog.h"

class MySignal {
public:
	int pid;
	int sid;
	int value;
	MySignal * next;
};

class MySignalHead{
public:
	int pid;
	int count;
	int head;
	int tail;
};

class Space {
public:
	Space();
	char* thiz;

	char * base;
	int length;
	int head;
	int tail;
	int max_size;
	int threshold;


	bool is_initialized;

	//API
	void * get(int start);
	int getLength(int start);

	int take(int length);
	void free(int start);

	bool initialize();
	bool free();

	void obverse_memset_slow(int start, char value, int count);

	void obverse_memcpy_slow(int start, void * source, int count);

	MySignalHead * mySignalHeadMap;
	MySignal * mySignalQueueBase;
	int signal_queue_head;

	MySignalHead *  getMySignalHead(int pid);
	MySignal *  getNewSignal(MySignalHead * signalHead);
};

Space * newSpace();

class ProcessCallback {
public:
	ProcessCallback();
	void (*onCall)(JSON * json);
	void (*onCommand)(int command);
	void (*onLostHeartBeat)(int heart_beat);
	void (*onStart)();
};

class Process {
public:
	Process();

	bool is_initialized;
	bool initialize();

	int pid;

	int send_heart_beat_to_pid;
	MySignalHead * send_heart_beat_to_SignalHead;
	int remote_heart_beat;

	MySignalHead * mySignalHead;
	void callProcess(int pid, int value, MySignalHead * mySignalHead);
	void callProcess(int pid, void * data, int length, MySignalHead * mySignalHead);
	void callProcess(int pid, JSON * json, MySignalHead * mySignalHead);

	Space * swap_space;
	void (*resolveSignal_sub)(int signal, siginfo_t * signal_info, void * none);

	bool is_onCall;
	void onCall();
	void onCall(int value);

	void regeditSignal();

	bool needThreadHolder;
	bool is_running;
	void threadHolder();

	ProcessCallback * processCallback;

};

#endif /* PROCESS_H */
