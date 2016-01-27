#include "Closure.h"

bool Closure::initialize() {
	this->variables = new JSON();
	this->variables->initialize();
	return true;
}

bool Closure::set(JSString* key, JSObject* jsObject) {
	return this->variables->set(key, jsObject);
}

bool Closure::set(char* key, int keyLength, JSObject* jsObject) {
	return this->variables->set(key, keyLength, jsObject);
}

JSObject* Closure::get(char* key) {
	if (key == NULL) {
		return NULL;
	}
	return this->variables->get(key);
}

JSObject* Closure::lookup(JSString* key) {
	if (key == NULL) {
		return NULL;
	}
	JSObject* jsObject = this->variables->get(key);
	if (jsObject == NULL) {
		if (this->previous != NULL) {
			jsObject = this->previous->lookup(key);
		} else {
			jsObject = NULL;
		}
	}
	return jsObject;
}

JSKeyValue* Closure::lookupKeyValue(JSString* key) {
	if (key == NULL) {
		return NULL;
	}
	JSKeyValue* js_keyValue = this->variables->getKeyValue(key->char_string, key->length);
	if (js_keyValue == NULL) {
		if (this->previous != NULL) {
			js_keyValue = this->previous->lookupKeyValue(key);
		} else {
			js_keyValue = NULL;
		}
	}
	return js_keyValue;
}


ExecuteContext::ExecuteContext() {

	this->none_object = new JSString("NONE");


	this->rootClosure = new Closure();
	this->rootClosure->initialize();

	this->importClosure = new Closure();
	this->importClosure->initialize();

	this->importClosure->next = this->rootClosure;
	this->rootClosure->previous = this->importClosure;

	this->currentClosure = this->rootClosure;

	this->skipElse = true;
}
