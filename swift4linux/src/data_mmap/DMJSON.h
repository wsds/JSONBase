#ifndef DMJSON_H
#define DMJSON_H

#include "MemoryMap.h"
#include "DMString.h"
#include "DMNumber.h"
#include "DMHashTable.h"
#include "DMList.h"

class DMJSON {
public:
	MemorySpace * self;

	DMLIST * dm_list;
	MemorySpace * dm_list_space;//todo recover it whenever is used
	DMHashTable * dm_hashTable;
	MemorySpace * dm_hashTable_space;//todo recover it whenever is used

	int length;
	int is_initialized;

//	DMHashTable *base_map;
//	DMString *json_string;

	DMJSON * parent_json;
	MemorySpace * parent_json_space;

	DMString * temp_key;

	bool initialize();
	void free();

	//API
	//list
	void set(int index, MemorySpace* value, bool isreplace);
	MemorySpace* get(int index, bool isdel);
	//TODO reorder

	//hash
	void set(MemorySpace* key, MemorySpace* value);
	MemorySpace* get(MemorySpace* key);
	MemorySpace* del(MemorySpace* key);

	void push(MemorySpace * dm_key_value);

	void traverse(TraverseCallback * traverseCallback);

	//JSONBase
	bool put(DMString * key_chain, MemorySpace * value);
	bool set(DMString * key_chain, MemorySpace * value);
	MemorySpace * get(DMString * key_chain);
	MemorySpace * del(DMString * key_chain);
};

DMJSON * newDMJSON();
bool exchangeSpace(MemorySpace * left, MemorySpace * right);

#endif /* DMJSON_H */

