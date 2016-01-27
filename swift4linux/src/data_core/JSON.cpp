#include "JSON.h"
#include <iostream>
//#include "../swift/interface/JSFunction.h"
//#include "../swift/interface/JSClass.h"

JSKeyValue::JSKeyValue() {
	this->type = JSKEYVALUE;
}

void JSKeyValue::free() {

}

void JSKeyValue::operator=(JSObject* value) {
	this->value = value;
	setKeyValue2JSON(this);
}

void JSKeyValue::operator=(char* char_string) {
	JSObject* value = new JSString(char_string);

	this->value = value;
	setKeyValue2JSON(this);
}

void JSKeyValue::operator=(int number) {
	JSObject* value = new JSNumber(number);

	this->value = value;
	setKeyValue2JSON(this);
}

JSKeyValue & JSKeyValue::operator[](char* key) {
	JSKeyValue* jsKeyValue = NULL;
	if (this->type != -1 && this->value->type == JSJSON) {
		JSON * json = (JSON *) (this->value);
		jsKeyValue = &((*json)[key]);
	} else {
		JSON * json = new JSON();
		json->initialize();
		this->value = json;
		this->type = JSKEYVALUE;

		setKeyValue2JSON(this);

		jsKeyValue = &((*json)[key]);
	}

	return *jsKeyValue;
}

JSObject * JSKeyValue::operator~() {
	if (this->type != -1) {
		JSObject* object = this->value;
		return object;
//		return 88888;
	} else {
//		JSObject * emptyObject = new JSObject();
//		emptyObject->type = -1;
//		emptyObject->number = 0;
//		return emptyObject;
		return NULL;
	}
}

JSON::JSON() {
	is_initialized = false;
	this->type = JSJSON;
	this->initialize();
}

JSON::~JSON() {
	this->free();
}

bool JSON::initialize() {
	if (this->is_initialized == true) {
		return false;
	}
	this->list = new LIST(); //to optimize the memory use, get it from a managed pool instead of new it.//to do
	this->list->initialize();
	this->hashTable = new HashTable1();
	this->hashTable->initialize();

	this->json_string = NULL;

	this->length = 0;
	this->is_initialized = true;

	this->keyEnd = NULL;

	return true;
}

bool JSON::free() {
	freeJSObject(this);

	delete(this->list);
	delete(this->hashTable);

	JSONIndicator* json_indicator=(JSONIndicator*)this->json_indicator;
	json_indicator->json=NULL;
	delete(json_indicator);

	delete (this->json_string);

	return true;
}

void JSON::setKeyEnd(char * keyEnd) {
	this->keyEnd = keyEnd;

}
//list
JSObject* JSON::pop() {
	JSObject* object = (JSObject*) this->list->pop();
	if (object != NULL && object->type == JSKEYVALUE) {
		JSKeyValue* jsKeyValue = ((JSKeyValue*) object);
		this->hashTable->del(jsKeyValue->key, jsKeyValue->keyLength);
	}
	this->length--;
	return object;
}

bool JSON::push(JSObject* object) {
	if (object == NULL) {
//		return false;
	}
	this->list->push(object); //todo uniqueness of value
	if (object->type == JSKEYVALUE) {
		JSKeyValue* jsKeyValue = ((JSKeyValue*) object);
		this->hashTable->setWithAttachment(jsKeyValue->key, jsKeyValue->keyLength, jsKeyValue->value, jsKeyValue);
	}
	this->length++;
	return true;
}

bool JSON::insert(JSObject* object, int index) {
	this->list->insert(object, index);
	if (object->type == JSKEYVALUE) {
		JSKeyValue* jsKeyValue = ((JSKeyValue*) object);
		this->hashTable->setWithAttachment(jsKeyValue->key, jsKeyValue->keyLength, jsKeyValue->value, jsKeyValue);
	}
	this->length++;
	return true;
}

bool JSON::replace(JSObject* object, int index) {
	this->list->replace(object, index);
	if (object->type == JSKEYVALUE) {
		JSKeyValue* jsKeyValue = ((JSKeyValue*) object);
		this->hashTable->setWithAttachment(jsKeyValue->key, jsKeyValue->keyLength, jsKeyValue->value, jsKeyValue);
	}
	return true;
}

JSObject* JSON::delAT(int index) {
	JSObject* object = (JSObject*) this->list->del(index);

	if (object != NULL) //deleted
	{
		if (object->type == JSKEYVALUE) {
			JSKeyValue* jsKeyValue = ((JSKeyValue*) object);
			this->hashTable->del(jsKeyValue->key, jsKeyValue->keyLength);
		}
		this->length--;
	} else //not found
	{
	}
	return object;
}

JSObject* JSON::find(int index) {
	JSObject* object = (JSObject*) this->list->find(index);
	return object;
}
JSONReader::JSONReader(JSON *json) {
	this->json = json;
}
JSObject* JSONReader::operator[](char* key) {
	JSObject * object = this->json->get(key);

	if (object != NULL) {
		return object;
	} else {
		JSObject * emptyObject = new JSObject();
		emptyObject->type = -1;
		emptyObject->number = 0;
		return emptyObject;
	}
}

JSKeyValue & JSON::operator[](JSString & jsString) {
	JSObject * object = (JSObject*) this->hashTable->get(jsString.char_string, jsString.length);

	JSKeyValue* jsKeyValue = NULL;

	if (object != NULL) {
		jsKeyValue = new JSKeyValue();
		jsKeyValue->key = jsString.char_string;
		jsKeyValue->keyLength = jsString.length;
		jsKeyValue->value = object;
		jsKeyValue->json = this;
	} else {
		jsKeyValue = new JSKeyValue();
		jsKeyValue->key = jsString.char_string;
		jsKeyValue->keyLength = jsString.length;
		jsKeyValue->json = this;
		jsKeyValue->type = -1;
	}

	return *jsKeyValue;
}
JSKeyValue & JSON::operator[](const char* key) {
	return this->operator[]((char*) key);
}
JSKeyValue & JSON::operator[](char* key) {
	int keyLength = getLength(key, this->keyEnd);
	JSObject * object = (JSObject*) this->hashTable->get(key, keyLength);

	JSKeyValue* jsKeyValue = NULL;

	if (object != NULL) {
		jsKeyValue = new JSKeyValue();
		jsKeyValue->key = key;
		jsKeyValue->keyLength = keyLength;
		jsKeyValue->value = object;
		jsKeyValue->json = this;
	} else {
		jsKeyValue = new JSKeyValue();
		jsKeyValue->key = key;
		jsKeyValue->keyLength = keyLength;
		jsKeyValue->json = this;
		jsKeyValue->type = -1;
	}

	return *jsKeyValue;
}
//hash
JSObject* JSON::get(int key) {
	JSObject* object = (JSObject*) this->hashTable->get(&key, 4);
	return object;
}

JSObject* JSON::get(char* key, int keyLength) {
	JSObject* object = (JSObject*) this->hashTable->get(key, keyLength);
	return object;
}

JSKeyValue* JSON::getKeyValue(char* key, int keyLength) {
	JSKeyValue* keyValue = (JSKeyValue*) this->hashTable->getAttachment(key, keyLength);
	if (keyValue != NULL) {
		keyValue->json = this;
	}
	return keyValue;
}

JSObject* JSON::get(char* key) {
	int keyLength = getLength(key);
	JSObject* object = this->get(key, keyLength);
	return object;
}

void setKeyValue2JSON(JSKeyValue* jsKeyValue) {
	((JSON*) (jsKeyValue->json))->set(jsKeyValue->key, jsKeyValue->keyLength, jsKeyValue->value);
}

JSObject* JSON::get(JSString & jsString) {
	JSObject* object = this->get(jsString.char_string, jsString.length);
	return object;
}

JSObject* JSON::get(JSString * jsString) {
	JSObject* object = this->get(jsString->char_string, jsString->length);
	return object;
}

bool JSON::set(int key, JSObject* value) {
	char * intNumberNew = (char*) JSMalloc(4);
	obverse_memcpy(intNumberNew, &key, 4);
	return this->set(intNumberNew, 4, value);
}

bool JSON::set(char* key, JSObject* value) {
	int keyLength = getLength(key);
	return this->set(key, keyLength, value);
}

bool JSON::set(JSString* jsString, JSObject* value) {
	return this->set(jsString->char_string, jsString->length, value);
}

bool JSON::set(char* key, char* valueString) {
	JSString* js_string = new JSString();
	(*js_string) = valueString;
	int keyLength = getLength(key);
	return this->set(key, keyLength, js_string);
}

bool JSON::set(char* key, int keyLength, JSObject* value) {
	JSKeyValue* jsKeyValue = NULL;
	JSKeyValue* old_jsKeyValue = (JSKeyValue*) this->hashTable->getAttachment(key, keyLength);
	if (old_jsKeyValue == NULL) {
		jsKeyValue = new JSKeyValue();
		jsKeyValue->key = key;
		jsKeyValue->value = value;
		jsKeyValue->keyLength = keyLength;

		this->list->push((JSObject*) jsKeyValue);
		this->length++;
	} else {
		jsKeyValue = old_jsKeyValue;
		jsKeyValue->value = value;
	}

	JSObject* oldObject = (JSObject*) this->hashTable->setWithAttachment(key, keyLength, value, jsKeyValue);
	//free the old object //to do

	return true;
}

JSObject* JSON::del(JSString & jsString) {
	JSObject* object = this->del(jsString.char_string, jsString.length);
	return object;
}

JSObject* JSON::del(int key) {
	JSObject* object = this->del((char *) &key, 4);
	return object;
}

JSObject* JSON::del(char* key) {
	int keyLength = getLength(key);
	JSObject* object = this->del(key, keyLength);
	return object;
}

JSObject* JSON::del(char* key, int keyLength) {
	JSKeyValue* old_jsKeyValue = (JSKeyValue*) this->hashTable->getAttachment(key, keyLength);
	JSObject* oldObject = (JSObject*) this->hashTable->del(key, keyLength);

	if (old_jsKeyValue != NULL) //deleted
	{
		int index = -1;
		for (int i = 0; i < this->length; i++) {
			JSObject * oldObject_list = (JSObject*) this->list->elements[i];
			if (oldObject_list->type == JSKEYVALUE) {
				JSKeyValue* oldKeyValue_list = (JSKeyValue*) oldObject_list;
				if (old_jsKeyValue == oldKeyValue_list) {
					index = i;
					break;
				}
			}
		}

		//free the old object //to do
		if (index == -1) {
//			return false;
			return oldObject;
		}
		this->list->del(index);
		this->length--;
	} else //not found
	{
	}
	return oldObject;
}



int preStringifyJSObject(JSObject* object) {
	int offset = 0;
	if (object == NULL) {
		offset = offset + 4;
	} else if (object->type == JSNUMBER) {
		JSNumber * js_number = (JSNumber*) object;
		offset = numberToString(((JSObject*) js_number)->number, NULL);
	} else if (object->type == JSSTRING) {
		JSString * js_string = (JSString*) object;
		if (js_string->length == 1) {
			offset = offset + 3;
		} else {
			bool hasQuote = false;
			for (int j = 0; j < js_string->length; j++) {
				if (js_string->char_string[j] == SINGLEQUOTE || js_string->char_string[j] == DOUBLEQUOTES) {
					hasQuote = true;
					break;
				}
			}
			offset = offset + js_string->length;
			if (hasQuote) {
				offset = offset + 6;
			} else {
				offset = offset + 2;
			}
		}
	} else if (object->type == JSKEYVALUE) {
		JSKeyValue * key_value = (JSKeyValue*) object;

		if (HasKeyQuote == true) {
			offset = 3 + key_value->keyLength;
		} else {
			offset = 1 + key_value->keyLength;
		}
		offset = offset + preStringifyJSObject(key_value->value);
	} else if (object->type == JSJSON) {
		int sub_offset = 0;
		JSObject *sub_object;
		for (int i = 0; i < ((JSON*) object)->list->length; i++) {
			if (i != 0) {
				sub_offset++;
			}
			sub_object = (JSObject*) ((JSON*) object)->list->find(i);
			sub_offset = sub_offset + preStringifyJSObject(sub_object);
		}
		offset = sub_offset + 2;
	}
	return offset;
}

int freeJSObject(JSObject* object) {
	if (object == NULL) {

	} else if (object->type == JSNUMBER) {
		JSNumber * js_number = (JSNumber*) object;
		delete (js_number);
	} else if (object->type == JSSTRING) {
		JSString * js_string = (JSString*) object;
		delete (js_string);
	} else if (object->type == JSKEYVALUE) {
		JSKeyValue * key_value = (JSKeyValue*) object;
		freeJSObject(key_value->value);
		delete (key_value);
	} else if (object->type == JSJSON) {
		JSON* json = (JSON*) object;
		JSObject *sub_object;
		for (int i = 0; i < json->list->length; i++) {
			sub_object = (JSObject *) json->list->find(i);
			freeJSObject(sub_object);
		}
	}
	return 0;
}

int stringifyJSObject(JSObject* object, JSString* json_string, JSON* parent) {
	if (object == NULL) {
		(*json_string) + nullStr;
	} else if (object->type == JSNUMBER) {
		JSNumber * js_number = (JSNumber*) object;
		(*json_string) + js_number->number;
	} else if (object->type == JSSTRING) {
		JSString * js_string = (JSString*) object;
		bool hasQuote = false;
		for (int j = 0; j < js_string->length; j++) {
			if (js_string->char_string[j] == SINGLEQUOTE || js_string->char_string[j] == DOUBLEQUOTES) {
				hasQuote = true;
				break;
			}
		}

		JSString* temp_string = new JSString(0);
		temp_string->char_string = js_string->char_string;
		temp_string->length = js_string->length;

		if (hasQuote) {
			js_string->char_string = json_string->char_string + json_string->length + 3;
			(*json_string) * ("\"\"\"@@\"\"\"") % (*temp_string);
		} else {
			js_string->char_string = json_string->char_string + json_string->length + 1;
			(*json_string) * ("\"@@\"") % (*temp_string);
		}

		delete (temp_string);
	} else if (object->type == JSKEYVALUE) {
		JSKeyValue * key_value = (JSKeyValue*) object;

		JSString* key_string = new JSString(0);
		key_string->char_string = key_value->key;
		key_string->length = key_value->keyLength;

		if (HasKeyQuote == true) {
			key_value->key = json_string->char_string + json_string->length + 1;
			(*json_string) * ("\"@@\":") % (*key_string);
		} else {
			key_value->key = json_string->char_string + json_string->length;
			(*json_string) * ("@@:") % (*key_string);
		}

		parent->hashTable->resetKey(key_value->key, key_value->keyLength);

		delete (key_string);

		//parse value
		stringifyJSObject(key_value->value, json_string, NULL);
	} else if (object->type == JSJSON) {
		JSON* json = (JSON*) object;

		if (useBIGBRACKET == true) {
			json_string->char_string[json_string->length] = LEFTBIGBRACKET;
		} else {
			json_string->char_string[json_string->length] = LEFTBRACKET;
		}
		json_string->length++;

		JSObject *sub_object;
		for (int i = 0; i < json->list->length; i++) {
			if (i != 0) {
				json_string->char_string[json_string->length] = COMMA;
				json_string->length++;
			}
			sub_object = (JSObject *) json->list->find(i);
			stringifyJSObject(sub_object, json_string, json);
		}

		if (useBIGBRACKET == true) {
			json_string->char_string[json_string->length] = RIGHTBIGBRACKET;
		} else {
			json_string->char_string[json_string->length] = RIGHTBRACKET;
		}
		json_string->length++;
	} else if (object->type == JSFUNCTION) {
		(*json_string) + funcStr;
	} else if (object->type == JSCLASS) {
		(*json_string) + classStr;
	}
	return 0;
}

char* stringifyJSON(JSON* json) {

	int json_string_length = preStringifyJSObject((JSObject*) json);
	JSString * old_json_string = NULL;
	if (json->json_string != NULL) {
		old_json_string = json->json_string;
	}
	json->json_string = new JSString(json_string_length);

	int offset = stringifyJSObject((JSObject*) json, json->json_string, NULL);

	delete (old_json_string);

	return json->json_string->char_string;

}

JSString* stringifyJSON2JSString(JSON* json) {

	int json_string_length = preStringifyJSObject((JSObject*) json);
	JSString * old_json_string = NULL;
	if (json->json_string != NULL) {
		old_json_string = json->json_string;
	}
	json->json_string = new JSString(json_string_length);

	int offset = stringifyJSObject((JSObject*) json, json->json_string, NULL);

	delete (old_json_string);

	return json->json_string;

}

JSObject * cloneJSObject(JSObject* object, JSON * parent) {
	JSObject * clone = NULL;
	if (object == NULL) {
		return NULL;
	} else if (object->type == JSNUMBER) {
		JSNumber * js_number = new JSNumber();
		js_number->number = ((JSNumber *) object)->number;
		clone = js_number;
	} else if (object->type == JSSTRING) {
//		JSString * js_string = new JSString();
//		js_string->char_string = (char *) JSMalloc(strlen(((JSString *) object)->char_string) + 1);
//		strcopy(((JSString *) object)->char_string, js_string->char_string);
//		js_string->length = ((JSString *) object)->length;
//		clone = js_string;
	} else if (object->type == JSFUNCTION) {
//		JSFunction * jsFunction = new JSFunction();
//
//		jsFunction->function_name = ((JSFunction *)object)->function_name;
//		jsFunction->function = ((JSFunction *)object)->function;
//		jsFunction->functionDefinition = ((JSFunction *)object)->functionDefinition;
//		jsFunction->closure = new Closure();
//		jsFunction->closure->variables = parent;
//
//		clone = jsFunction;
	} else if (object->type == JSCLASS) {
//		JSClass * jsClass = new JSClass();
//
//		jsClass->className = (char *)JSMalloc(strlen(((JSClass *)object)->className) + 1);
//		strcopy(((JSClass *)object)->className, jsClass->className);
//
//		jsClass->children = new JSON();
//		jsClass->children->initialize();
//		cloneJSON((JSON *)object, jsClass->children);
//
//		clone = jsClass;
	} else if (object->type == JSJSON) {
		JSON * json = new JSON();
		json->initialize();
		cloneJSON((JSON *) object, json);
		clone = json;
	}
	return clone;
}

void cloneJSON(JSON * json, JSON * clone) {

	JSObject * sub_object = NULL;
	for (int i = 0; i < json->list->length; i++) {
		sub_object = (JSObject *) json->list->find(i);
		if (sub_object->type == JSKEYVALUE) {
			JSKeyValue * key_value = (JSKeyValue*) sub_object;

			char * key = NULL;
			key = (char *) JSMalloc(strlen(key_value->key) + 1);
			strcopy(key_value->key, key);
			JSObject * value = cloneJSObject(key_value->value, clone);
//			clone->set(key, value);
		} else {
			JSObject * cloneObject = cloneJSObject(sub_object, clone);
			clone->push(cloneObject);
		}

	}
}

JSON* cloneJSON(JSON * json) {
	JSON * clone = new JSON();
	clone->initialize();
	cloneJSON(json, clone);
	return clone;
}

int parsingStatus = 1; //[1:normal,2:quote_start,3:string,4:second_quote_start,5:quote_stop,6:second_quote_stop],[1-2,2-3,3-1,2-4,4-5,5-3,4-1]
int QuoteStatus = 11; //[ 11 = ' , 13 = ''' , 21 = " , 23 = """]
int last_COMMA_index = 0;

int json_indicators_stack_size = 10;

int key_value_stack_size = 10;
JSON* parseJSON(const char* string) {
	return parseJSON((char*) string);
}
JSON* parseJSON(char* char_string) {
	int string_length = strlen(char_string);
	return parseJSON(char_string, string_length);
}

JSON* parseJSON(JSString* js_string) {
	return parseJSON(js_string->char_string, js_string->length);
}
JSON* parseJSON(char* char_string, int string_length) {

	char localChar;

	JSString * json_string = new JSString(string_length);
	json_string->copy(char_string, string_length);
	char* json_char_string = json_string->char_string;

	last_COMMA_index = 0;

	JSONIndicator* json_indicator = NULL;
	JSONIndicator* object_indicator = NULL;
	JSON* result_json = NULL;

	int json_indicators_stack_top = 0;
	int key_value_stack_top = 0;

	int mem_size = json_indicators_stack_size * sizeof(JSONIndicator*);
	JSONIndicator** json_indicators = (JSONIndicator**) JSMalloc(mem_size);

	int key_value_mem_size = key_value_stack_size * sizeof(KeyValueIndicator*);
	KeyValueIndicator** key_value_indicators = (KeyValueIndicator**) JSMalloc(key_value_mem_size);

	for (int i = 0; i < string_length; i++) {
		localChar = json_char_string[i];

		if (parsingStatus == 3) {

			if ((localChar == SINGLEQUOTE && QuoteStatus == 11) || (localChar == DOUBLEQUOTES && QuoteStatus == 21)) {
				json_indicator->quotes_count = 1;
				parsingStatus = 1;
				continue;
			} else if ((localChar == SINGLEQUOTE && QuoteStatus == 13) || (localChar == DOUBLEQUOTES && QuoteStatus == 23)) {
				parsingStatus = 5;
			}
		} else if (parsingStatus == 2) {
			if ((localChar == SINGLEQUOTE && QuoteStatus == 11) || (localChar == DOUBLEQUOTES && QuoteStatus == 21)) {
				parsingStatus = 4;
			} else {
				parsingStatus = 3;
			}
		} else if (parsingStatus == 4) {
			if (localChar == SINGLEQUOTE && QuoteStatus == 11) {
				parsingStatus = 3;
				QuoteStatus = 13;
			} else if (localChar == DOUBLEQUOTES && QuoteStatus == 21) {
				parsingStatus = 3;
				QuoteStatus = 23;
			} else {
				json_indicator->quotes_count = 1;
				parsingStatus = 1;
			}
		} else if (parsingStatus == 5) {
			if ((localChar == SINGLEQUOTE && QuoteStatus == 13) || (localChar == DOUBLEQUOTES && QuoteStatus == 23)) {
				parsingStatus = 6;
			} else {
				parsingStatus = 3;
			}
		} else if (parsingStatus == 6) {
			if ((localChar == SINGLEQUOTE && QuoteStatus == 13) || (localChar == DOUBLEQUOTES && QuoteStatus == 23)) {
				json_indicator->quotes_count = 3;
				parsingStatus = 1;
				continue;
			} else {
				parsingStatus = 3;
			}
		}

		//*************************************parsingStatus == 1*****************************************************************************************************
		if (parsingStatus == 1) {
			if (localChar == COMMA) {
				if (i > last_COMMA_index) {
					object_indicator = new JSONIndicator(); //get from pool//to do
					object_indicator->head = last_COMMA_index;
					object_indicator->tail = i;
					object_indicator->quotes_count = json_indicator->quotes_count;
					json_indicator->quotes_count = 0;

					last_COMMA_index = i + 1;

					JSObject* object = parseObject(json_char_string, object_indicator, false);
					if (key_value_stack_top > 0 && key_value_indicators[key_value_stack_top - 1]->key_value->value == NULL) {
						if (object != NULL) {
							JSKeyValue * jsKeyValue = key_value_indicators[key_value_stack_top - 1]->key_value;
							jsKeyValue->value = object;
							json_indicator->json->push(jsKeyValue);
							key_value_stack_top--;
						} else {
							//report error,value cannot be NULL
						}
					} else {
						if (object != NULL) {
							json_indicator->json->push(object);
						}
					}

				}
			} else if (localChar == COLON) {
				if (i > last_COMMA_index) {
					object_indicator = new JSONIndicator(); //get from pool//to do
					object_indicator->head = last_COMMA_index;
					object_indicator->tail = i;
					object_indicator->quotes_count = json_indicator->quotes_count;
					json_indicator->quotes_count = 0;

					last_COMMA_index = i + 1;

					JSKeyValue * jsKeyValue = (JSKeyValue*) parseObject(json_char_string, object_indicator, true);
					if (key_value_stack_top < key_value_stack_size) {
						if (jsKeyValue != NULL) {
							key_value_indicators[key_value_stack_top] = new KeyValueIndicator();
							key_value_indicators[key_value_stack_top]->key_value = jsKeyValue;
							key_value_indicators[key_value_stack_top]->json_indicators_stack_top = json_indicators_stack_top;
							key_value_indicators[key_value_stack_top]->key_value->value = NULL;
//							json_indicator->json->push(object);
							key_value_stack_top++;
						} else {
							//report error,key cannot be NULL
						}
					} else {
						//report error;
					}
				}
			} else if (localChar == SINGLEQUOTE) {
				QuoteStatus = 11;
				parsingStatus = 2;
			} else if (localChar == DOUBLEQUOTES) {
				QuoteStatus = 21;
				parsingStatus = 2;
			} else if (localChar == LEFTBRACKET || localChar == LEFTBIGBRACKET) {
				if (json_indicators_stack_top < json_indicators_stack_size) {

					JSON* json = new JSON();
					json->initialize();

					JSONIndicator* new_json_indicator = new JSONIndicator(); //get from pool//to do
					new_json_indicator->head = i;
					new_json_indicator->json = json;
					new_json_indicator->quotes_count = 0;
					json->json_indicator = (void *) new_json_indicator;

					json_indicators[json_indicators_stack_top] = new_json_indicator;
					json_indicators_stack_top++;

					if (i == 0 && json_indicator == NULL) {
						result_json = json;
					} else {
						if (key_value_stack_top > 0 && key_value_indicators[key_value_stack_top - 1]->key_value->value == NULL) {
							JSKeyValue * jsKeyValue = key_value_indicators[key_value_stack_top - 1]->key_value;
							jsKeyValue->value = (JSObject*) json;
							json_indicator->json->push(jsKeyValue);

							key_value_indicators[key_value_stack_top - 1]->json_indicators_stack_top = json_indicators_stack_top;
						} else {
							json_indicator->json->push((JSObject*) json);
						}
					}

					json_indicator = (JSONIndicator*) json->json_indicator;

					last_COMMA_index = i + 1;
				} else {
					//report error
				}
			} else if (localChar == RIGHTBRACKET || localChar == RIGHTBIGBRACKET) {

				object_indicator = new JSONIndicator(); //get from pool//to do
				object_indicator->head = last_COMMA_index;
				object_indicator->tail = i;
				object_indicator->quotes_count = json_indicator->quotes_count;
				json_indicator->quotes_count = 0;

				JSObject* object = parseObject(json_char_string, object_indicator, false);
				delete(object_indicator);
				if (key_value_stack_top > 0 && key_value_indicators[key_value_stack_top - 1]->json_indicators_stack_top == json_indicators_stack_top) {
					if (key_value_indicators[key_value_stack_top - 1]->key_value->value == NULL) {
						if (object != NULL) {
							JSKeyValue * jsKeyValue = key_value_indicators[key_value_stack_top - 1]->key_value;
							jsKeyValue->value = object;
							json_indicator->json->push(jsKeyValue);
						} else {
							//report error,value cannot be NULL
						}
					} else if (key_value_stack_top > 0) {
						if (object != NULL) {
							json_indicator->json->push(object);
						}
					}
					key_value_stack_top--;
				} else {
					if (object != NULL) {
						json_indicator->json->push(object);
					}
				}

				last_COMMA_index = i + 1;

				if (json_indicators_stack_top > 1) {
					//resolve the last element spited by COMMA
					json_indicator = json_indicators[json_indicators_stack_top - 1];
					json_indicator->tail = i;

					json_indicator = json_indicators[json_indicators_stack_top - 2];
					//json_indicator->json = parseJSON(string, json_indicator);
					json_indicators_stack_top--;
				} else if (json_indicators_stack_top == 1) {
					if (i + 1 == string_length) {
						//resolve the last element spited by COMMA

						result_json->json_string = json_string;

						JSFree(json_indicators);
						JSFree(key_value_indicators);
						return result_json;
					} else {
						//report error
					}
				} else {
					//report error
				}
			}
		}
		//*************************************parsingStatus == 1*****************************************************************************************************

	}

	JSFree(json_indicators);
	JSFree(key_value_indicators);
	return NULL;
}

JSObject* parseObject(char* string, JSONIndicator* object_indicator, bool isJSKeyValue) {
	JSObject* object;
	int start = object_indicator->head;
	int end = object_indicator->tail;
	char localChar;
	while (true) {
		localChar = string[start];
		if (localChar == TAB || localChar == COMMA || localChar == ENTER || localChar == BR || localChar == BLANK) {
			start++;
		} else {
			break;
		}
	}

	while (true) {
		localChar = string[end - 1];
		if (localChar == TAB || localChar == COMMA || localChar == ENTER || localChar == BR || localChar == BLANK) {
			end--;
		} else {
			break;
		}
	}
	if (start >= end) {
		return NULL;
	}

	int length = end - start - object_indicator->quotes_count * 2;
	char *tartget_string = string + start + object_indicator->quotes_count;

	if (isJSKeyValue == false) {

		if (object_indicator->quotes_count != 0) {
			JSString * js_string = new JSString(0);
			js_string->char_string = tartget_string;
			js_string->length = length;
			object = (JSObject *) js_string;
		} else {
			JSNumber * js_number = new JSNumber();
			((JSObject*) js_number)->number = parseStringToNubmer(tartget_string, length);
			object = (JSObject *) js_number;

		}

	} else {
		JSKeyValue * jsKeyValue = new JSKeyValue();
		object = (JSObject*) jsKeyValue;

		jsKeyValue->key = tartget_string;
		jsKeyValue->keyLength = length;
	}

	return object;
}

void testJSONParse() {

	const char * json_str = "[a:1,'''abc''',123,[1,2,[a:1,b:[123,[1,12],456],123],'a':2,'b:3':\"123456\"],'''a''', '''b''']";
	const char * json_str1 = "[123,[1,12,13,[123,[1,12],456],14,15,16,17],456,789,101112]";
	const char * json_str2 = "[123,[1,'''abc'''],456,\"def\",'''b''','''abc''']";
	const char * json_str3 = "[123,[1,'''abc'''],\"a\":\"\"\"456\"\"\",\"def\"]";
	const char * json_str4 = "[123,567:[012,456,'123123123'],567]";
	const char * json_str5 = "[123,567:[012,[\"hello\"],456,'''''',123:[123,'123123'],123],567]";

	JSON* json = parseJSON((char *) json_str5);

	std::cout << stringifyJSON(json) << std::endl;

}
