#include "SubProcess.h"

Process * thizSubProcess;

void resolveSignal_SubProcess(int signal, siginfo_t * signal_info, void * none) {
	if (signal == 58) {
		thizSubProcess->onCall();
	} else if (signal == SIGTERM) {
		JSLog("SIGTERM");
	}
	return;
}

SubProcess::SubProcess() {
	thizSubProcess = this;
	this->resolveSignal_sub = &resolveSignal_SubProcess;
}

SubProcess * SubProcess::instance = NULL;
SubProcess * SubProcess::getInstance() {
	if (instance == NULL) {
		instance = new SubProcess();

	}
	return instance;
}

void SubProcess::callMainProcess(JSON * json) {
	this->callProcess(this->parent_pid, json, this->parent_SignalHead);
}
void SubProcess::linkMainProcess() {
	this->parent_SignalHead = this->swap_space->getMySignalHead(this->parent_pid);
}
