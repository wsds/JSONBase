#ifndef MODULE_H
#define MODULE_H

//#include "../interface/JSFunction.h"

#ifndef NULL
#define NULL 0
#endif /* NULL */

#define EMPTY -1

#define JSBOOL 0
#define JSNUMBER 1
#define JSJSON 2
#define JSKEYVALUE 3
#define JSSTRING 4

#define JSINSTANCE 5
#define JSCLASS 6
#define JSFUNCTION 7

class JSObject {
public:
	char type;
	int number; //for debug
	char* char_string; //for debug
};
class JSNumber: public JSObject {

public:
	JSNumber();
	JSNumber(int number);

	//int number;
	void test() {

//		int i = 1 + 1;
	}

};

class JSString: public JSObject {
public:
	//char* char_string;
	int length;
	char* char_string; //for debug

	int max_size;
	char * keyEnd;

	JSString();
	JSString(char* char_string);
	JSString(int max_size);
	~JSString();

	void initialize(int max_size);
	void resize(int min_size);
	void set(char * char_string, int length);
	void copy(char * char_string, int length);
	JSString * replaceAll(char * find, char * replaceWith);
	JSString * replaceAll(char * find, int findLength, char * replaceWith,
			int replaceWithLength);

	JSString& operator=(JSString & jsString);
	JSString& operator=(char * char_string);

	JSString& operator=(const char * char_string);
	JSString& operator=(int number);

	JSString& operator^=(JSString & jsString);

	JSString& operator+(JSString & jsString);
	JSString& operator+(char * char_string);
	//[Important!]must add the & after returning JSString1, otherwise function call will return a mirror object with the same content but different point to the next function call.
	JSString& operator+(const char * char_string);
	JSString& operator+(int number);

	JSString& operator*(JSString & jsString);
	JSString& operator*(char * char_string);
	JSString& operator*(const char * char_string);
	JSString& operator*(int number);

	bool operator==(JSString & jsString);
	bool operator==(char * char_string);
	bool operator==(const char * char_string);
	bool operator==(int number);

	bool operator>=(char * char_string);

	JSString& operator%(JSString & jsString);
	JSString& operator%(char * char_string);
	JSString& operator%(const char * char_string);
	JSString& operator%(int number);
	void format(char * char_string, int targetLength);

	JSString& operator%=(char * char_string);

	char* operator~();

};

class LIST {
public:
	LIST();
	//store the orginal type of data directly should be supported. or supported by another class.
//	void ** elements; //~~~~~~~~~~~~~~~Memory Management~~~~~~~~~~~~~~~~~
	void* elements[50]; //~~~~~~~~~~~~~~~Memory Management~~~~~~~~~~~~~~~~~//for debug

	int length;

	int max_size;
	int threshold;

	bool is_initialized;

	//API
	//find O(1)
	void * find(int index);
	//find O(n)
	int findValue(void * object);

	//insert O(n)
	bool insert(void * object, int index);

	//replace O(1)
	bool replace(void * object, int index);

	//del O(n)
	void * del(int index);

	//pop O(1)
	void * pop();

	//push O(1)
	bool push(void * object);

	//resize O(n)
	bool resize();

	//initialize default size=8;
	bool initialize();

	bool free();

};
class HashEntry {
public:
	void * key;
	void * value;

	void * attachment;

	unsigned int hash;
	int keyLength;

	int level;

	HashEntry* next;
};

class HashTable {
public:
	HashTable();
	HashEntry ** elements; //~~~~~~~~~~~~~~~Memory Management~~~~~~~~~~~~~~~~~
	//HashEntry* elements[50];//~~~~~~~~~~~~~~~Memory Management~~~~~~~~~~~~~~~~~//for debug

	int length;

	int max_size;
	int threshold;

	bool is_initialized;

	//get O(1)
	void* get(void* key, int keyLength);

	//set O(1)
	void* set(void* key, int keyLength, void* value);
	void* resetKey(void* newKey, int keyLength);

	//del O(1)
	void* del(void* key, int keyLength);

	//resize O(n)
	bool resize();

	//initialize default size=8;
	bool initialize();

	bool free();

};

class HashTable1: public HashTable {
public:
	HashTable1();
	//get O(1)
	void* getAttachment(void* key, int keyLength);
	void* setWithAttachment(void* key, int keyLength, void* value,
			void * attachment);
};

class JSKeyValue: public JSObject {
public:
	JSKeyValue();
	char* key;
	int keyLength;

	JSObject* value;

	JSObject* json;
	void operator=(JSObject* value);
	void operator=(char* char_string);
	void operator=(int number);

	JSKeyValue & operator[](char* key);

	JSObject* operator~();

	void free();
};
class JSON: public JSObject {
public:
	JSON();
	~JSON();

	LIST *list;
	HashTable1 *hashTable;
	JSString *json_string;

	char * keyEnd;

	void * json_indicator;

	int length;
	bool is_initialized;

	bool initialize();
	bool free();

	//API
	void setKeyEnd(char * keyEnd);
	//list
	JSObject* pop();
	bool push(JSObject* object);
	bool insert(JSObject* object, int index);
	bool replace(JSObject* object, int index);
	JSObject* delAT(int index);
	JSObject* find(int index);

	//hash
	bool set(char* key, int keyLength, JSObject* value);
	JSObject* get(char* key, int keyLength);
	JSObject* del(char* key, int keyLength);

	//for convenience
	JSObject* get(JSString & jsString);
	JSObject* get(JSString * jsString);
	JSObject* get(int key);
	JSObject* get(char* key);

	JSKeyValue* getKeyValue(char* key, int keyLength);

	bool set(int key, JSObject* value);
	bool set(char* key, JSObject* value);
	bool set(char* key, char* valueString);
	bool set(JSString* jsString, JSObject* value);

	JSObject* del(JSString & jsString);
	JSObject* del(int key);
	JSObject* del(char* key);
	//for convenience

	JSKeyValue & operator[](char* key);
	JSKeyValue & operator[](const char* key);
	JSKeyValue & operator[](JSString & jsString);

	//serialize
};

class Closure {
public:
	Closure* previous;
	JSON* variables;
	Closure* next;

	bool initialize();
	bool free();

	Closure* forward();
	Closure* backward();

	JSObject* get(char* key);
	bool set(JSString* key, JSObject* jsObject);
	bool set(char* key, int keyLength, JSObject* jsObject);

	JSObject* lookup(JSString* key);
	JSKeyValue* lookupKeyValue(JSString* key);
};

class ExecuteContext {
public:
	ExecuteContext();

	JSObject * none_object;

	Closure * rootClosure;
	Closure * currentClosure;
	bool skipElse;
};

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

class FunctionDefinition: public StructuredExecutable {
public:
	FunctionDefinition();
	JSString * functionName;
	LIST* variables;
};

class JSFunction: public JSObject {
public:
	JSFunction();

	void free();

	JSString* function_name;
	LIST* variables;
	FunctionDefinition * functionDefinition;

	char * native_function_name;
	char * native_variables;
	JSObject* (*nativeFunction)(ExecuteContext * context);

	Closure* closure;
};

class Module {
public:
	Module();
	virtual ~Module() {
	}

//	ExecuteContext * context;
	virtual JSFunction * importFunction() {
		return NULL;
	}
};

#endif /* MODULE_H */

