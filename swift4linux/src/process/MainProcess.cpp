#include "MianProcess.h"

Process * thizProcess;

void resolveSignal_MainProcess(int signal, siginfo_t * signal_info, void * none) {
	if (signal == 58) {
		thizProcess->onCall();
	} else if (signal == SIGSEGV) {
	}

	return;
}

MainProcess::MainProcess() {
	thizProcess = this;
	this->resolveSignal_sub = &resolveSignal_MainProcess;
//	this->needThreadHolder = false;
}

MainProcess * MainProcess::instance = NULL;
MainProcess * MainProcess::getInstance() {
	if (instance == NULL) {
		instance = new MainProcess();

	}
	return instance;
}

void MainProcess::callSubProcess(JSON * json) {
	this->callProcess(this->sub_pid, json, this->sub_SignalHead);
}

void MainProcess::linkSubProcess() {
	this->sub_SignalHead = this->swap_space->getMySignalHead(this->sub_pid);
	this->send_heart_beat_to_pid = this->sub_pid;
	this->send_heart_beat_to_SignalHead = this->sub_SignalHead;
}

MainProcess * mainProcess;
SubProcess * subProcess;

void foldProcess6() {
	int maxRetry = 5;
	while (maxRetry > 0) {
		maxRetry--;
		int sub_pid = fork();

		if (sub_pid < 0) {
		} else if (sub_pid > 0) {
			JSLog(sub_pid);
//			int errorNumber = errno;
//			if (errorNumber != EAGAIN) {
			mainProcess->sub_pid = sub_pid;

			mainProcess->linkSubProcess();
			mainProcess->initialize();
			break;
//			}
		} else {
			subProcess->linkMainProcess();
			subProcess->initialize();
			break;
		}
	}
}

int times = 0;
void mainProcessCommand(JSString * jsString) {
	JSLog("Call to Sub");

	const char * json_str1 = "[times:132,b:'ProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandhello']";
	JSON * json = parseJSON(json_str1);

	for (int i = 0; i < 10; i++) {
		times++;
		(*json)["times"] = times;
		mainProcess->callSubProcess(json);
	}
	delete (json);
}
void subProcessCommand(JSString * jsString) {

	JSLog("Call to Main");

	const char * json_str1 = "[times:132,b:'ProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandProcessCommandhello']";
	JSON * json = parseJSON(json_str1);

	for (int i = 0; i < 2; i++) {
		times++;
		(*json)["times"] = times;
		subProcess->callMainProcess(json);
	}
	delete (json);
}

void onStart_mainProcess() {
	WebsocketLog * websocketLog = WebsocketLog::getInstance();
	websocketLog->initialize();
	websocketLog->callSubProcessCommand = &mainProcessCommand;
	sleep(2);
	JSLog("onStart_mainProcess");

}

void onStart_subProcess() {
	WebsocketLog * websocketLog = WebsocketLog::getInstance();
	websocketLog->localPort = 8092;
	websocketLog->initialize();
	websocketLog->callSubProcessCommand = &subProcessCommand;
	sleep(2);
	JSLog("onStart_subProcess");
}
int called_times = 0;
void onCall(JSON * json) {
	JSLog(json);
	called_times++;
}

void onLostHeartBeat(int heart_beat) {
	JSLog("onLostHeartBeat+++++++++++++++++");
	JSLog(heart_beat);

//	LocalStorage * localStorage = LocalStorage::getInstance();
//
//	if (localStorage != NULL) {
//		localStorage->closeMMap();
//	}
	subProcess->is_running = false;

	raise(56);

}

void initProcess() {

	WebsocketLog * websocketLog = WebsocketLog::getInstance();
	websocketLog->initialize();
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
}
