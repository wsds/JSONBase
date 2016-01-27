#include "HashTable1.h"

HashTable1::HashTable1() {
	this->is_initialized = false;
}
void* HashTable1::getAttachment(void* key, int keyLength) {

	unsigned int hash = dictGenHashFunction(key, keyLength);
	int index = hash % max_size;
	if (elements[index] != NULL) {
		HashEntry* brother = elements[index];
		do {
			if (brother->keyLength == keyLength && stringCompare(brother->key, key, keyLength) == true) {
				return brother->attachment;
			}
			brother = brother->next;
		} while (brother != NULL);
	}

	return NULL;
}


void* HashTable1::setWithAttachment(void* key, int keyLength, void* value, void* attachment) {


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
				brother->attachment = attachment;
				return oldObject; //replace entry
			}
			brother = brother->next;
		} while (brother != NULL);
	}

	HashEntry* element = new HashEntry(); //get from HashEntry pool//to do
	element->key = key;
	element->value = value;
	element->attachment = attachment;
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
