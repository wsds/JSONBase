#ifndef SUBPROCESS_H
#define SUBPROCESS_H
#include "Process.h"
#include "../lib/JSLog.h"

class SubProcess: public Process {
public:
	SubProcess();

	int parent_pid;
	MySignalHead * parent_SignalHead;

	static SubProcess *instance;
	static SubProcess * getInstance();

	void callMainProcess(JSON * json);

	void linkMainProcess();
};

#endif /* SUBPROCESS_H */
