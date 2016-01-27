#ifndef INTERPRETMAIN_H
#define INTERPRETMAIN_H
#include <iostream>

#include "../data_core/data_core.h"

#include "execute.h"
#include "stringifyExecutable.h"

#include "../lib/JSLog.h"
#include "Modules/importModules.h"


#ifndef NULL
#define NULL 0
#endif /* NULL */

#define KEYWORD 0
#define NAME 1
#define CODEOPERATOR  2
#define CODE_NUMBER 3
#define CODE_STRING 4
#define CODE_JSON 5
#define DELIMITER 6
#define BRACKET 7

#define SKIP 10;
#define CHILDNAME 11

//void interpret_main();
//void resolveCodeLine(char* line);

class Interpreter{
public:
	Interpreter();

	static Interpreter *instance;
	static Interpreter * getInstance();

	bool is_initialized;
	bool initialize();
	int contextIDStart;
	HashTable2 *contextMap;
	HashTable2 *executeContextMap;

	int newContext();
	void inputCode(int contextID, JSString * code);
	void inputCodeLine(int contextID, JSString * codeLineStr);
	void importCode(int contextID, JSString * code);
	void runCode(int contextID, JSString * code);

	void run(int contextID);
	void clear(int contextID);
	void logClosure(int contextID);
};


#endif /* INTERPRETMAIN_H */



