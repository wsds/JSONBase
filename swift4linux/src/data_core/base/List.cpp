#include "List.h"

LIST::LIST() {
	is_initialized = false;
	this->initialize();
}
LIST::~LIST() {
	JSFree(this->elements);

}
void * LIST::find(int index) {
	void * object = this->elements[index];
	return object;
}

int LIST::findValue(void * object) {
	int i = 0;
	for (i = 0; i <= this->length; i++) {
		if (this->elements[i] == object) {
			return i;
		}
	}
	return -1;

}

bool LIST::insert(void * object, int index) {
	if (this->length >= index && index >= 0) {
		if (this->max_size <= this->length) {
			this->resize(); //synchronous
		}
		int i = 0;
		for (i = this->length; i > index; i--) {
			this->elements[i] = this->elements[i - 1];
		}
		this->elements[index] = object;

		this->length++;

		if (this->length > this->threshold) {
			this->resize(); //asynchronous
		}

		return true;
	} else {
		return false;
	}
}

bool LIST::replace(void * object, int index) {
	if (this->length > index && index >= 0) {
		this->elements[index] = object;
		return true;
	} else {
		return false;
	}
}

void * LIST::del(int index) {
	if (this->length > index && index >= 0) {
		void * object = this->elements[index];
		int i = 0;
		for (i = index; i <= this->length - 2; i++) {
			this->elements[i] = this->elements[i + 1];
		}
		this->elements[this->length - 1] = NULL;
		this->length--;

		return object;
	} else {
		return NULL;
	}

}

void * LIST::pop() {
	if (this->length > 0) {
		void * object = this->elements[this->length - 1];
		this->elements[this->length - 1] = NULL;
		this->length--;
		return object;
	} else {
		return NULL;
	}

}

bool LIST::push(void * object) {
	if (object == NULL) {
		return false;
	}
	if (this->max_size <= this->length) {
		this->resize(); //synchronous
	}

	this->elements[this->length] = object;
	this->length++;
	if (this->length > this->threshold) {
		this->resize(); //asynchronous
	}

	return true;

}

bool LIST::resize() {
	void ** old_elements = this->elements;
	this->max_size = this->max_size * 2;
	this->threshold = (int) (this->max_size * 0.8);

	int mem_size = this->max_size * sizeof(void*);

	this->elements = (void **) JSMalloc(mem_size);

	int i = 0; //~~~~~~~~~~~~~~~Need Memory Management~~~~~~~~~~~~~~~~~
	for (i = 0; i <= this->length; i++) {
		this->elements[i] = old_elements[i];
	}
	JSFree((void*) old_elements);

	return true;
}

bool LIST::initialize() {
	if (this->is_initialized == true) {
		return false;
	}
	this->max_size = 8;
	this->length = 0;
	this->threshold = (int) (this->max_size * 0.8);

	int mem_size = this->max_size * sizeof(void*);

	this->elements = (void **) JSMalloc(mem_size);

	this->is_initialized = true;

	return true;
}

bool LIST::free() {
	//JSFree(this->elements);
	return true;
}
