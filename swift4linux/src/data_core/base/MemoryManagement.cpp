#include "MemoryManagement.h"

char * JSMalloc(size_t size) {
	void *pointer = malloc(size + 1);

//	for (unsigned int i = 0; i < size; i++){
//		*((char*)pointer + i) = 0;
//	}
	memset(pointer, 0, size + 1);
	return (char *) pointer;
}

void* JSMalloc(size_t size, int FD, int offset) {
	void *pointer;
	ftruncate(FD, size);
	pointer = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, FD, offset); //offset must be n times of 4096 the size of Memory Page.

	if ((pointer) == (void *) -1) {
		return NULL;
	}
//	memset(pointer, 0, size);
	return pointer;
}

void JSFree(void* pointer, size_t size, int FD, size_t fileLength) {

	munmap(pointer, size);
	close(FD);
}

void JSFree(void* pointer) {
	if (pointer != NULL) {
		free(pointer);
	}
}
