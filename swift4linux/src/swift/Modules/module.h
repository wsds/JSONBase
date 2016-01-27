#ifndef MODULE_H
#define MODULE_H

#include "../interface/JSFunction.h"
#include <dlfcn.h>
#include "../parseCodeLine.h"

#ifndef NULL
#define NULL 0
#endif /* NULL */

class Module {
public:
	Module();
	virtual ~Module() {
	}

	ExecuteContext * context;
	virtual JSFunction * importFunction() {
		return NULL;
	}
};


#endif /* MODULE_H */

