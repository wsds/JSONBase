#ifndef JSADD_H
#define JSADD_H
#include "module.h"

class JSAddModule: public Module {
public:
	virtual JSFunction * importFunction();
};


int add(int x, int y);

#endif /* JSADD_H */

