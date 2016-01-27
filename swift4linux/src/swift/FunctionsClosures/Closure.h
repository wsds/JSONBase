#ifndef CLOSURE_H
#define CLOSURE_H



#include "../../data_core/JSON.h"

#ifndef NULL
#define NULL 0
#endif /* NULL */

class Closure
{
public:
	Closure* previous;
	JSON* variables;
	Closure* next;

	bool initialize();
	bool free();

	Closure* forward();
	Closure* backward();

	JSObject* get(char* key);
	bool set(JSString* key, JSObject* jsObject);
	bool set(char* key,int keyLength, JSObject* jsObject);

	JSObject* lookup(JSString* key);
	JSKeyValue* lookupKeyValue(JSString* key);
};

class ExecuteContext {
public:
	ExecuteContext();

	JSObject * none_object;

	Closure * rootClosure;
	Closure * importClosure;
	Closure * currentClosure;
	bool skipElse;
};

#endif /* CLOSURE_H */

