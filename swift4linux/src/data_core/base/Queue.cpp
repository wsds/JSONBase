#include "Queue.h"
Queue::Queue() {
	is_initialized = false;
}
void* Queue::take() {
	if (this->length > 0) {
		this->length--;

		void* object = this->elements[this->head % this->max_size];
		this->elements[this->head % this->max_size] = NULL;

		this->head++;
		return object;
	} else {
		return NULL;
	}
}

bool Queue::offer(void * object) {
	if (this->max_size <= this->length) {
		this->resize(); //synchronous
	}

	if (this->length + 1 > this->threshold) {
		this->resize(); //asynchronous
	}

	this->length++;
	this->elements[this->tail % this->max_size] = object;
	this->tail++;

	return true;

}

bool Queue::resize() {
	void** old_elements = this->elements;

	int new_max_size = this->max_size * 2;

	this->threshold = (int) (new_max_size * 0.8);

	int mem_size = new_max_size * sizeof(void*);

	void** new_elements = (void**) JSMalloc(mem_size);

	int i = 0; //~~~~~~~~~~~~~~~Need Memory Management~~~~~~~~~~~~~~~~~
	for (i = this->head; i < this->tail; i++) {
		new_elements[i % new_max_size] = old_elements[i % this->max_size];
	}

	this->elements = new_elements;
	//
	this->max_size = new_max_size;
	JSFree((void*) old_elements);

	return true;
}

bool Queue::initialize() {
	this->max_size = 8;
	this->length = 0;
	this->tail = 0;
	this->head = 0;
	this->threshold = (int) (this->max_size * 0.8);

	int mem_size = this->max_size * sizeof(void*);

	this->elements = (void**) JSMalloc(mem_size);

	this->is_initialized = true;

	return true;
}

bool Queue::free() {
	//JSFree(this->elements);
	return true;
}
