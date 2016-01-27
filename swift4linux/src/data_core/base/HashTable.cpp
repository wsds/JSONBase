#include "HashTable.h"

HashTable::HashTable() {
	is_initialized = false;
}

HashTable::~HashTable() {
	this->free();
}
void HashTable::free() {

	for (int i = 0; i < this->max_size; i++) {
		HashEntry* element = this->elements[i];
		if (element != NULL) {
			delete (element);
		}
	}
	JSFree(this->elements);
}

void* HashTable::get(void* key, int keyLength) {

	unsigned int hash = dictGenHashFunction(key, keyLength);
	int index = hash % max_size;
	if (elements[index] != NULL) {
		HashEntry* brother = elements[index];
		do {
			if (brother->keyLength == keyLength && stringCompare(brother->key, key, keyLength) == true) {
				return brother->value;
			}
			brother = brother->next;
		} while (brother != NULL);
	}

	return NULL;
}

void* HashTable::set(void* key, int keyLength, void* value) {
	HashEntry* element = new HashEntry(); //get from HashEntry pool//to do

	unsigned int hash = dictGenHashFunction(key, keyLength);
	int index = hash % max_size;

	if (elements[index] != NULL) {
		HashEntry* brother = elements[index];
		do {
			if (brother->keyLength == keyLength && stringCompare(brother->key, key, keyLength) == true) {
				//free the old value "brother->value"
				//todo
				void * oldObject = brother->value;
				brother->value = value;
				return oldObject; //replace entry
			}
			brother = brother->next;
		} while (brother != NULL);
	}

	element->key = key;
	element->value = value;
	element->attachment = NULL;
	element->hash = hash;
	element->keyLength = keyLength;

	if (elements[index] == NULL) {
		elements[index] = element;
	}

	else {
		int level = 1;
		HashEntry* brother = elements[index];
		while (brother->next != NULL) {
			level++;
			brother = brother->next;
		}
		brother->next = element;
		element->level = level;
	}

	this->length++;
	if (this->length > this->threshold) {
		this->resize(); //asynchronous//todo
	}

	return NULL; //new entry
}

void* HashTable::resetKey(void* newKey, int keyLength) {

	unsigned int hash = dictGenHashFunction(newKey, keyLength);
	int index = hash % max_size;

	if (elements[index] != NULL) {
		HashEntry* brother = elements[index];
		do {
			if (brother->keyLength == keyLength && stringCompare(brother->key, newKey, keyLength) == true) {
				brother->key = newKey;
			}
			brother = brother->next;
		} while (brother != NULL);
	}

	return NULL; //new entry
}

void* HashTable::del(void* key, int keyLength) {
	unsigned int hash = dictGenHashFunction(key, keyLength);
	int index = hash % max_size;
	if (elements[index] != NULL) {
		HashEntry** brother = elements + index;
		do {
			if ((*brother)->keyLength == keyLength && stringCompare((*brother)->key, key, keyLength) == true) {

				HashEntry* old_element = *brother;
				//free the old element, put them into a HashEntry pool to reuse them.
				//todo

				(*brother) = (*brother)->next;
				this->length--;
				return old_element->value;
			}
			brother = &((*brother)->next);
		} while ((*brother) != NULL);
	}

	return NULL;
}

bool HashTable::resize() {
	int old_index = 0; //~~~~~~~~~~~~~~~Need Memory Management~~~~~~~~~~~~~~~~~
	int index = 0;
	HashEntry* element = NULL;
	HashEntry* old_element = NULL;
	HashEntry* brother = NULL;
	int level = 1;
	int old_max_size = this->max_size;

	HashEntry** old_elements = this->elements;

	this->max_size = this->max_size * 2 + 7;
	this->threshold = (int) (this->max_size * 0.8);

	int mem_size = this->max_size * sizeof(void*);

	this->elements = (HashEntry**) JSMalloc(mem_size);

	for (old_index = 0; old_index < old_max_size; old_index++) {
		element = old_elements[old_index];
		if (element != NULL) {
			do {

				//put old element into the new space
				index = element->hash % this->max_size;
				if (elements[index] == NULL) {
					elements[index] = element;
				}

				else {
					level = 1;
					brother = elements[index];
					while (brother->next != NULL) {
						level++;
						brother = brother->next;
					}
					brother->next = element;
					element->level = level;
				}
				//put old element into the new space
				old_element = element;
				element = element->next;
				old_element->next = NULL;
			} while (element != NULL);
		}

	}
	JSFree((void*) old_elements);

	return true;
}

bool HashTable::initialize() {
	if (is_initialized == true) {
		return true;
	}
	this->max_size = 1 + 7;
	this->length = 0;
	this->threshold = (int) (this->max_size * 0.8);

	int mem_size = this->max_size * sizeof(void*);

	this->elements = (HashEntry**) JSMalloc(mem_size);
	//for (int i = 0; i < 50; i++){
	//	this->elements[i] = NULL;
	//}

	this->is_initialized = true;

	return true;

}

static unsigned int dict_hash_function_seed = 5381;
/* MurmurHash2, by Austin Appleby
 * Note - This code makes a few assumptions about how your machine behaves -
 * 1. We can read a 4-byte value from any address without crashing
 * 2. sizeof(int) == 4
 *
 * And it has a few limitations -
 *
 * 1. It will not work incrementally.
 * 2. It will not produce the same results on little-endian and big-endian
 *    machines.
 */
unsigned int dictGenHashFunction(void *key, int keyLength) {

	/* 'm' and 'r' are mixing constants generated offline.
	 They're not really 'magic', they just happen to work well.  */
	unsigned int seed = dict_hash_function_seed;
	const unsigned int m = 0x5bd1e995;
	const int r = 24;

	/* Initialize the hash to a 'random' value */
	unsigned int h = seed ^ keyLength;

	/* Mix 4 bytes at a time into the hash */
	const unsigned char *data = (const unsigned char *) key;

	while (keyLength >= 4) {
		unsigned int k = *(unsigned int*) data;

		k *= m;
		k ^= k >> r;
		k *= m;

		h *= m;
		h ^= k;

		data += 4;
		keyLength -= 4;
	}

	/* Handle the last few bytes of the input array  */
	switch (keyLength) {
	case 3:
		h ^= data[2] << 16;
		break;
	case 2:
		h ^= data[1] << 8;
		break;
	case 1:
		h ^= data[0];
		h *= m;
		break;
	};

	/* Do a few final mixes of the hash to ensure the last few
	 * bytes are well-incorporated. */
	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return (unsigned int) h;
}

