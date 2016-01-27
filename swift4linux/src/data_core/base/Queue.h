#ifndef QUEUE_H
#define QUEUE_H

#include "MemoryManagement.h"

#ifndef NULL
#define NULL 0
#endif /* NULL */

class Queue {
public:
	Queue();
	//store the orginal type of data directly should be supported. or supported by another class. 
	void** elements; //~~~~~~~~~~~~~~~Memory Management~~~~~~~~~~~~~~~~~
	//JSObject* elements[50];//~~~~~~~~~~~~~~~Memory Management~~~~~~~~~~~~~~~~~//for debug

	int length;
	int tail;
	int head;

	int max_size;
	int threshold;

	bool is_initialized;

	//API
	//pop O(1)
	void* take();

	//push O(1)
	bool offer(void* object);

	//resize O(n)
	bool resize();

	//initialize default size=8;
	bool initialize();

	bool free();

};

#endif /* QUEUE_H */

