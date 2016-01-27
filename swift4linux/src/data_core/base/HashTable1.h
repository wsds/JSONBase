#ifndef HASHTABLE1_H
#define HASHTABLE1_H

#include "HashTable.h"

#ifndef NULL
#define NULL 0
#endif /* NULL */


class HashTable1: public HashTable  {
public:
	HashTable1();
	//get O(1)
	void* getAttachment(void* key, int keyLength);
	void* setWithAttachment(void* key, int keyLength, void* value, void * attachment);
};

#endif /* HASHTABLE1_H */

