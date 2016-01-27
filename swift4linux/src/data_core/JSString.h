#ifndef JSSTRING1_H
#define JSSTRING1_H

#include "base/JSObject.h"
#include "base/MemoryManagement.h"
#include "temp_string.h"

#ifndef NULL
#define NULL 0
#endif /* NULL */

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
	JSString * replaceAll(char * find, int findLength, char * replaceWith, int replaceWithLength);

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

char * numberToString(int number);

#endif /* JSSTRING1_H */

