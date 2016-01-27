#ifndef MAINPROCESS_H
#define MAINPROCESS_H
#include "SubProcess.h"
#include "../lib/WebSocketLog.h"

class MainProcess: public Process {
public:
	MainProcess();

	int sub_pid;
	MySignalHead * sub_SignalHead;

	static MainProcess *instance;
	static MainProcess * getInstance();

	void linkSubProcess();

	void callSubProcess(JSON * json);

};

void initProcess();


#endif /* MAINPROCESS_H */
