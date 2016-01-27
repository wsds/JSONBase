#include "HashTable2.h"

HashTable2::HashTable2() {
	this->is_initialized = false;
}
void* HashTable2::get(char* key) {
	return this->HashTable::get(key, getLength(key, this->keyEnd));
}
void* HashTable2::get(int number) {
	return this->HashTable::get((char *)&number, 4);
}

void* HashTable2::set(char* key, void* value) {
	return this->HashTable::set(key, getLength(key, this->keyEnd), value);
}
void* HashTable2::set(int key, void* value) {
	char * intNumberNew = (char*) JSMalloc(4);
	obverse_memcpy(intNumberNew, &key, 4);
	return this->HashTable::set(intNumberNew, 4, value);
}

void* HashTable2::del(char* key) {
	return this->HashTable::del(key, getLength(key, this->keyEnd));
}
void* HashTable2::del(int key) {
	memset(this->intNumber, 0, 5);
	for (int i = 0; i < 4; i++) {
		*(this->intNumber + i) = *((char *) (&key) + i);
	}
	return this->HashTable::del(this->intNumber, 4);
}

bool HashTable2::initialize() {
	if (this->is_initialized == true) {
		return true;
	}
	this->HashTable::initialize();

	int mem_size = this->max_size * sizeof(void*);

	this->intNumber = (char*) JSMalloc(5);

	this->keyEnd = NULL;

	return true;

}
