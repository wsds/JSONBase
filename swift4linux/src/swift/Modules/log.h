#ifndef MYLOG_H
#define MYLOG_H
#include "module.h"
#include "../../lib/JSLog.h"
#include "../execute.h"

class JSLogModule : public Module {
public:
	virtual JSFunction * importFunction();
};

void myLog(JSObject* object);
#endif /* LOG_H */

