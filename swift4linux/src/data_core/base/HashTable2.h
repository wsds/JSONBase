#ifndef HASHTABLE2_H
#define HASHTABLE2_H

#include "HashTable.h"

#ifndef NULL
#define NULL 0
#endif /* NULL */


class HashTable2: public HashTable  {
public:
	HashTable2();

	char * keyEnd;
	char * intNumber;
	int nubmer;

	void* get(char* key);
	void* get(int number);

	void* set(char* key, void* value);
	void* set(int number, void* value);

	void* del(char* key);
	void* del(int number);

	bool initialize();
};

#endif /* HASHTABLE2_H */

