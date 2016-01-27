#ifndef JSINSTANCE_H
#define JSINSTANCE_H


#include "../../data_core/data_core.h"

#ifndef NULL
#define NULL 0
#endif /* NULL */

class JSInstance : JSObject
{
public:
	JSInstance();
	char* name;
	JSON* children;
	void test();
	void(*function)(JSObject* parameter, JSObject* result);
};


#endif /* JSINSTANCE_H */

