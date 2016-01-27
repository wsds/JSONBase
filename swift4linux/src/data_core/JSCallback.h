#ifndef JSCALLBACK_H
#define JSCALLBACK_H


#include "base/JSObject.h"
#include "base/List.h"
#include "base/HashTable.h"

#ifndef NULL
#define NULL 0
#endif /* NULL */

class JSCallback : public JSObject
{
public:
	JSCallback();
	~JSCallback();
};


#endif /* JSCALLBACK_H */

