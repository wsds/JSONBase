#ifndef EXECUTE_H
#define EXECUTE_H

#include "../data_core/data_core.h"
#include "FunctionsClosures/Closure.h"
#include "interface/JSClass.h"
#include "interface/JSFunction.h"
#include "analyzeCodeLine.h"

#ifndef NULL
#define NULL 0
#endif /* NULL */

JSObject* execute(Executable * executable, ExecuteContext * context);
JSObject* execute(ExecutableBlock * executableBlock, ExecuteContext * context);

JSObject* execute(MetaExecutable* meta, ExecuteContext * context);
JSObject* execute(Expression * expression, ExecuteContext * context);
JSObject* execute(FunctionCall * functionCall, ExecuteContext * context);

JSObject* execute(StructuredExecutable * structuredExecutable, ExecuteContext * context);
JSObject* execute(StructuredExecutable * structuredExecutable, ExecuteContext * context, LIST* variables);

JSObject* execute(FunctionDefinition * functionDefinition, ExecuteContext * context);
JSObject* execute(ClassDefinition * classDefinition, ExecuteContext * context);

JSObject* execute(IfBlock * ifBlock, ExecuteContext * context);
JSObject* execute(ForBlock * forBlock, ExecuteContext * context);
JSObject* execute(ForInBlock * forInBlock, ExecuteContext * context);

JSObject* execute(FunctionReturn* functionReturn, ExecuteContext * context);

JSObject* resolveExecutableList(LIST* executableList, ExecuteContext * context);

JSObject* excuteAssignment(ResultedExecutable * source, ResultedExecutable *target, ExecuteContext * context);
void newVariable(MetaExecutable * target, ExecuteContext * context);
JSObject* executeNativeFunction(JSFunction* jsFunction, ExecuteContext * context);

JSObject * getValueFromClosure(CodeElement* codeElement, ExecuteContext * context);

JSKeyValue * getKeyValueFromClosure(CodeElement* codeElement, ExecuteContext * context);

JSObject * getValueFromClosure(JSString * key, ExecuteContext * context);

#endif /* EXECUTE_H */

