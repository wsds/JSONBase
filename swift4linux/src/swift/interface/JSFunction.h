#ifndef JSFUNCTION_H
#define JSFUNCTION_H

#include "../../data_core/data_core.h"
#include "../FunctionsClosures/Closure.h"
#include "../analyzeCodeLine.h"
#ifndef NULL
#define NULL 0
#endif /* NULL */

class JSFunction: public JSObject {
public:
	JSFunction();

	void free();

	JSString* function_name;
	LIST* variables;
	FunctionDefinition * functionDefinition;

	char * native_function_name;
	char * native_variables;
	JSObject* (*nativeFunction)(ExecuteContext * context);


	Closure* closure;
};

#endif /* JSFUNCTION_H */

