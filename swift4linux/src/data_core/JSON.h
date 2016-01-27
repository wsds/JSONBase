#ifndef JSON_H
#define JSON_H

#include "base/JSObject.h"
#include "base/List.h"
#include "base/HashTable1.h"

//#include "JSParsingObject.h"

#include "JSNumber.h"
#include "JSString.h"

#include "temp_string.h"

#ifndef NULL
#define NULL 0
#endif /* NULL */



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
class KeyValueIndicator {
public:
	int json_indicators_stack_top;
	JSKeyValue* key_value;
};

void setKeyValue2JSON(JSKeyValue* jsKeyValue);

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

class JSONReader {
public:
	JSONReader(JSON *json);
	JSON *json;
	JSObject* operator[](char* key);
};

class JSONIndicator {
public:
	int head;
	int tail;
	int quotes_count; //[0,1,3]

	JSON* json;
};

void testJSONParse();
JSON* parseJSON(char* string);
JSON* parseJSON(const char* string);
JSON* parseJSON(char* char_string, int string_length);
JSON* parseJSON(JSString* js_string);
JSObject* parseObject(char* string, JSONIndicator* object_indicator, bool isJSKeyValue);

char* stringifyJSON(JSON* json);
JSString* stringifyJSON2JSString(JSON* json);

JSON* cloneJSON(JSON* json);
void cloneJSON(JSON * json, JSON * clone);

int freeJSObject(JSObject* object);

#endif /* JSON_H */

