#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "MemoryManagement.h"
#include "../temp_string.h"

#ifndef NULL
#define NULL 0
#endif /* NULL */

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
	~HashTable();
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

	void free();


};

unsigned int dictGenHashFunction(void *key, int keyLength);

#endif /* HASHTABLE_H */

