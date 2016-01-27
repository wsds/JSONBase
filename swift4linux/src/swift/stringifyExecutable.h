#ifndef STRINGIFYEXECUTABLE_H
#define STRINGIFYEXECUTABLE_H

#include "../data_core/data_core.h"
#include "analyzeCodeLine.h"

#ifndef NULL
#define NULL 0
#endif /* NULL */

JSON * stringify(Executable * executable);
JSON * stringify(ExecutableBlock * executableBlock);

JSON * stringify(Expression * expression);

JSON * stringify(Operator * js_operator);
JSON * stringify(MetaExecutable * meta);

JSON * stringify(FunctionCall * functionCall);
JSON * stringify(FunctionDefinition * functionDefinition);
JSON * stringify(ClassDefinition * classDefinition);

JSON * stringify(IfBlock * ifBlock);
JSON * stringify(ForBlock * forBlock);
JSON * stringify(ForInBlock * forInBlock);

JSON * stringify(FunctionReturn* functionReturn);

JSON * stringifyAssignment(Executable * target, MetaExecutable * source, bool isNew);
#endif /* STRINGIFYEXECUTABLE_H */

