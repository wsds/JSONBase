#include "JSString.h"

JSString::JSString() {
	this->type = JSSTRING;
	this->initialize(16);
}

JSString::~JSString() {
	this->length = 0;
	if (this->max_size > 0) {
		JSFree(this->char_string);
		this->max_size = 0;
	}
}

JSString::JSString(int max_size) {
	this->type = JSSTRING;
	this->initialize(max_size);
}

JSString::JSString(char* char_string) {
	this->type = JSSTRING;
	this->keyEnd = NULL;
	int targetLength = getLength(char_string, this->keyEnd);
	this->initialize(targetLength);
	*this = char_string;
}

void JSString::initialize(int max_size) {
	this->length = 0;

	if (max_size == 0) {
		this->max_size = 0;
		this->char_string = NULL;
	} else {
		this->max_size = 16;
		while (this->max_size < max_size) {
			this->max_size = this->max_size * 2;
		}
		this->char_string = JSMalloc(this->max_size);
	}
	this->keyEnd = NULL;
}

void JSString::set(char * char_string, int length) {
	this->char_string = char_string;
	this->length = length;
	this->max_size = 0;
}

void JSString::copy(char * char_string, int length) {
	if (char_string == NULL) {
		return;
	}

	if (length == 0) {
		this->length = 0;
		return;
	}

	int targetLength = length;

	if (this->max_size < targetLength) {
		this->resize(targetLength); //synchronous
	}
	memcpy(this->char_string, char_string, targetLength);

	*(this->char_string + targetLength) = '\0';

	this->length = targetLength;
}

JSString * JSString::replaceAll(char * find, char * replaceWith) {
	int findLength = getLength(find, this->keyEnd);
	int replaceWithLength = getLength(replaceWith, this->keyEnd);
	return this->replaceAll(find, findLength, replaceWith, replaceWithLength);
}

JSString * JSString::replaceAll(char * find, int findLength, char * replaceWith, int replaceWithLength) {
	JSString * new_string = new JSString(this->length);
	int index = 0;
	bool isSame = false;

	for (int i = 0; i < this->length;) {
		isSame = true;
		for (int j = 0; j < findLength; j++) {
			if (i + j >= this->length) {
				isSame = false;
				break;
			} else if (this->char_string[i + j] != find[j]) {
				isSame = false;
				break;
			}
		}

		if (isSame == false) {
			if (new_string->max_size < index + 1) {
				this->resize(index + 1);
			}
			new_string->char_string[index] = this->char_string[i];
			index++;
			i++;
		} else {
			if (new_string->max_size < index + replaceWithLength) {
				this->resize(index + replaceWithLength);
			}
			memcpy(new_string->char_string + index, replaceWith, replaceWithLength);

			index = index + replaceWithLength;
			i = i + findLength;
		}

	}

	new_string->length = index;
	return new_string;
}

void JSString::resize(int min_size) {
	char* old_char_string = NULL;
	if (this->max_size == 0) {
		this->max_size = 16;
	} else {
		old_char_string = this->char_string;
	}
	while (this->max_size < min_size) {
		this->max_size = this->max_size * 2;
	}

	char * new_char_string = JSMalloc(this->max_size);

	memcpy(new_char_string, this->char_string, this->length);

	if (old_char_string != NULL) {
		JSFree(old_char_string);
	}

	this->char_string = new_char_string;
}

JSString& JSString::operator=(char * char_string) {
	if (char_string == NULL) {
		return *this;
	}

	int targetLength = getLength(char_string, this->keyEnd);

	if (this->max_size < targetLength) {
		this->resize(targetLength); //synchronous
	}
	memcpy(this->char_string, char_string, targetLength);

	*(this->char_string + targetLength) = '\0';

	this->length = targetLength;

	return *this;
}

JSString& JSString::operator=(const char * char_string) {
	*this = (char *) char_string;
	return *this;
}

JSString& JSString::operator=(int number) {
	memset(this->char_string, 0, this->length);

	this->length = 0;

	this->length = numberToString(number, this->char_string);

	return *this;
}

JSString& JSString::operator=(JSString & jsString) {
	this->char_string = jsString.char_string;
	this->length = jsString.length;
	this->max_size = jsString.max_size;

	return *this;
}

JSString& JSString::operator^=(JSString & jsString) {
	this->keyEnd = jsString.keyEnd;
	*this = jsString.char_string;
	return *this;
}

JSString& JSString::operator+(char * char_string) {

	if (char_string == NULL) {
		return *this;
	}

	int targetLength = getLength(char_string, this->keyEnd);

	if (this->max_size < this->length + targetLength) {
		this->resize(this->length + targetLength); //synchronous
	}

	memcpy(this->char_string + this->length, char_string, targetLength);
	this->length = this->length + targetLength;

	return *this;
}

JSString& JSString::operator+(const char * char_string) {

	*this + (char *) char_string;
	return *this;
}

JSString& JSString::operator+(JSString & jsString) {

	int targetLength = jsString.length;

	if (this->max_size < this->length + targetLength) {
		this->resize(this->length + targetLength); //synchronous
	}

	memcpy(this->char_string + this->length, jsString.char_string, targetLength);
	this->length = this->length + targetLength;

	return *this;
}

JSString& JSString::operator+(int number) {

	int targetLength = numberToString(number, number_string_buffer);

	if (this->max_size < this->length + targetLength) {
		this->resize(this->length + targetLength); //synchronous
	}

	reverse_memcpy(this->char_string + this->length, number_string_buffer, targetLength);
	this->length = this->length + targetLength;

	return *this;
}

JSString& JSString::operator*(char * char_string) {
	*this + char_string;
	return *this;
}

JSString& JSString::operator*(const char * char_string) {
	*this + (char *) char_string;
	return *this;
}

JSString& JSString::operator*(JSString & jsString) {
	*this + jsString;
	return *this;
}

JSString& JSString::operator*(int number) {
	*this + number;
	return *this;
}


JSString& JSString::operator%(char * char_string) {
	if (char_string == NULL) {
		return *this;
	}

	int targetLength = getLength(char_string, this->keyEnd);
	this->format(char_string, targetLength);

	return *this;
}

void JSString::format(char * char_string, int targetLength) {

	if (char_string == NULL) {
		return;
	}

	int first_CHAR_AT = -1;
	int second_CHAR_AT = -1;
	for (int i = 0; i < this->length; i++) {
		if (*(this->char_string + i) == CHAR_AT) {
			if (first_CHAR_AT == -1) {
				first_CHAR_AT = i;
			} else {
				second_CHAR_AT = i;
				break;
			}
		}
	}

	if (first_CHAR_AT == -1 || second_CHAR_AT == -1) {
		*this + char_string;
		return;
	}

	int formatLength = second_CHAR_AT - first_CHAR_AT + 1;

	if (this->max_size < this->length + targetLength - formatLength) {
		this->resize(this->length + targetLength - formatLength); //synchronous
	}

	if (formatLength > targetLength) {
		obverse_memcpy_slow(this->char_string + first_CHAR_AT + targetLength, this->char_string + second_CHAR_AT + 1, this->length - second_CHAR_AT);

	} else if (formatLength < targetLength) {
		reverse_memcpy(this->char_string + first_CHAR_AT + targetLength, this->char_string + second_CHAR_AT + 1, this->length - second_CHAR_AT);
	}

	memcpy(this->char_string + first_CHAR_AT, char_string, targetLength);
	this->length = this->length + targetLength - formatLength;

	return;
}

JSString& JSString::operator%=(char * char_string) {
	JSString * jsString = new JSString();

	*jsString ^= *this;
	*jsString % char_string;

	return *jsString;

}

char* JSString::operator~() {
	char * new_char_string = JSMalloc(this->length);
	obverse_memcpy(new_char_string, this->char_string, this->length);
	return new_char_string;
}

JSString& JSString::operator%(const char * char_string) {
	*this % (char *) char_string;
	return *this;
}

JSString& JSString::operator%(JSString & jsString) {
	this->format(jsString.char_string, jsString.length);
	return *this;
}

JSString& JSString::operator%(int number) {
	int targetLength = numberToString(number, number_string_buffer);
	this->format(number_string_buffer, targetLength);
	return *this;
}


bool JSString::operator==(JSString & jsString) {

	if (this->length != jsString.length) {
		return false;
	}

	bool isSame = stringCompare(this->char_string, jsString.char_string, this->length);
	return isSame;
}

bool JSString::operator==(char * char_string) {

	int targetLength = getLength(char_string, this->keyEnd);
	if (this->length != targetLength) {
		return false;
	}

	bool isSame = stringCompare(this->char_string, char_string, this->length);
	return isSame;
}

bool JSString::operator>=(char * char_string) {
	bool isSame = stringCompare(this->char_string, char_string, this->length);
	return isSame;
}

bool JSString::operator==(const char * char_string) {
	bool isSame = *this == (char *) char_string;
	return isSame;
}

bool JSString::operator==(int number) {

	int targetLength = numberToString(number, number_string_buffer);

	if (this->length != targetLength) {
		return false;
	}

	bool isSame = stringCompare(this->char_string, number_string_buffer, this->length);
	return isSame;
}

char * numberToString(int number) {

	int len = 0;
	bool isNegative = false;
	if (number >= 0) {
		isNegative = false;
	} else {
		isNegative = true;
		number = -number;
	}

	do {
		buf[len++] = number % radix + NUMBER_CHAR_START;
		number /= 10;
	} while (number != 0);

	if (isNegative == true) {
		buf[len++] = '-';
	}

	char * target = JSMalloc(len);
	for (int j = len - 1; j >= 0; j--) {
		target[len - j - 1] = buf[j];
	}
	target[len] = '\0';
	return target;
}
