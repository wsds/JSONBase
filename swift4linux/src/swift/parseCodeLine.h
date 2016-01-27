#ifndef PARSECODELINE_H
#define PARSECODELINE_H

#include "../data_core/data_core.h"

#ifndef NULL
#define NULL 0
#endif /* NULL */

class KeyWords {
public:

	static KeyWords *instance;

	static KeyWords * getInstance() {
		if (instance == NULL) {
			instance = new KeyWords();
		}
		return instance;
	}

	KeyWords();
	char* string_var;
	char* string_new;
	char* string_func;
	char* string_if;
	char* string_else;

	char* string_for;
	char* string_in;
	char* string_while;
	char* string_do;

	char* string_return;

	char* string_class;
	char* string_instance;
	char* string_this;
	char* string_self;

	JSON * keyWordMap;

};

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
class CodeElement {
public:
	int type;
	JSString * keyword;
	JSString * variable_name;
	char code_operator;
	char code_operator2;

	int number;
	JSString * char_string;
	JSString * jsonstr;

	int index;

	char bracket;
	int nextBracketIndex;
	int preBracketIndex;

	//char * delimiter;
	bool isResolvedDelimiter;

	CodeElement * child;
};

class CodeLine {
public:
	int funtionLevel;
	JSString * codeLineStr;
	CodeElement** codeElements;
	LIST * codeElementList;
};

CodeLine* resolveCodeLine(JSString * line);
int resolveElement(char* from, int length, CodeLine* codeLine);
#endif /* PARSECODELINE_H */

