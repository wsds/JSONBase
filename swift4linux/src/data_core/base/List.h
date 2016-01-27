#ifndef LIST_H
#define LIST_H

#include "MemoryManagement.h"

#ifndef NULL
#define NULL 0
#endif /* NULL */

class LIST {
public:
	LIST();
	~LIST();
	//store the orginal type of data directly should be supported. or supported by another class. 
	void ** elements; //~~~~~~~~~~~~~~~Memory Management~~~~~~~~~~~~~~~~~
//	void* elements[50];//~~~~~~~~~~~~~~~Memory Management~~~~~~~~~~~~~~~~~//for debug

	int length;

	int max_size;
	int threshold;

	bool is_initialized;

	//API
	//find O(1)
	void * find(int index);
	//find O(n)
	int findValue(void * object);

	//insert O(n)
	bool insert(void * object, int index);

	//replace O(1)
	bool replace(void * object, int index);

	//del O(n)
	void * del(int index);

	//pop O(1)
	void * pop();

	//push O(1)
	bool push(void * object);

	//resize O(n)
	bool resize();

	//initialize default size=8;
	bool initialize();

	bool free();

};

#endif /* LIST_H */

