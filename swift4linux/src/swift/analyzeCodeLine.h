#ifndef ANALYZECODELINE_H
#define ANALYZECODELINE_H

#include "../data_core/data_core.h"

#include "parseCodeLine.h"

#include "../lib/JSLog.h"

#ifndef NULL
#define NULL 0
#endif /* NULL */

#define META 0
#define ASSIGNMENT 1
#define FUNCTIONCALL 2
#define FUNCTIONDEFINITION 3

#define CONDITION 4
#define IFBLOCK 5
#define FORBLOCK 6
#define FORINBLOCK 7

#define EXPRESSION 9
#define EXCUTEABLEBLOCK 10

#define OPERATOR 11
#define FUNCTIONRETURN 12
#define CLASSDEFINITION 13
#define INSTANCEDEFINITION 14

#define EXCUTED 20
#define RESULTED 21
class Executable {
public:
	int type;
};

class ResultedExecutable: public Executable {
public:
	ResultedExecutable();
	JSObject* value;
	JSKeyValue* keyValue;
};

class ExecutableBlock: public ResultedExecutable {
public:
	ExecutableBlock();
	LIST* executables;
};

class StructuredExecutable: public Executable {
public:
	StructuredExecutable();
	ExecutableBlock* executableBlock;
};


class MetaExecutable: public ResultedExecutable {
public:
	MetaExecutable();
	CodeElement* codeElement;
};

class Operator: public Executable {
public:
	Operator();
	char code_operator;
	char code_operator2;

	ResultedExecutable* left;
	ResultedExecutable* right;
	int operator_type;
};
class Expression: public ResultedExecutable {
public:
	Expression();
	bool isNew;

	LIST* executables;
};

class FunctionCall: public ResultedExecutable {
public:
	FunctionCall();
	JSString * functionName;

	CodeElement* functionNameChain;
	bool isNew;
	LIST* variables;
};

class FunctionReturn: public Executable {
public:
	FunctionReturn();
	ResultedExecutable*  variable;
};
class FunctionDefinition: public StructuredExecutable {
public:
	FunctionDefinition();
	JSString * functionName;
	LIST* variables;
};

class ClassDefinition: public StructuredExecutable {
public:
	ClassDefinition();
	JSString * className;
	LIST* variables;
};

class InstanceDefinition: public StructuredExecutable {
public:
	InstanceDefinition();
	JSString * instanceName;
	LIST* variables;
};

class IfBlock: public StructuredExecutable {
public:
	IfBlock();

	bool isElse;

	Executable * condition;

	IfBlock* next;
};

class ForBlock: public StructuredExecutable {
public:
	ForBlock();
	Executable * pre_executable;

	LIST * conditions;

	Executable * last_executable;

	bool isDo;
};

class ForInBlock: public StructuredExecutable {
public:
	ForInBlock();

	JSON* iterator;
	JSString* iteratorName;
	JSString* keyName;
	JSString* valueName;

};

class Context {
public:
	Context();

	LIST * structuredExecutableStack;
	ExecutableBlock * currentExecutableBlock;
	ExecutableBlock * rootExecutableBlock;

	StructuredExecutable * pre_structuredExecutable;
	StructuredExecutable * structuredExecutable;
	int structuredLevel; //-1:pop 0:none 1:push
};

Executable* analyzeCodeLine(CodeLine * codeLine, int from, int end, Context * context);
Executable* analyzeCodeLine(CodeLine * codeLine, Context * context);

#endif /* ANALYZECODELINE_H */
