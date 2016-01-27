#ifndef JSCLASS_H
#define JSCLASS_H

#include "../../data_core/data_core.h"
#include "../FunctionsClosures/Closure.h"

#ifndef NULL
#define NULL 0
#endif /* NULL */

class JSClass: public JSObject {
public:
	JSClass();

	JSString* className;
	JSON* children;

	void free();
	Closure* closure;
};

#endif /* JSCLASS_H */

