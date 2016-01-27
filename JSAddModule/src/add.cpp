#include "add.h"
extern "C" {
void * create() {
	void * c();

	return new JSAddModule();
}
}

JSObject * add(ExecuteContext * context) {
	JSObject * result = new JSNumber();
	result->number = 123456;
	return result;
}

JSFunction * JSAddModule::importFunction() {

	JSFunction * js_function = new JSFunction();

	js_function->native_function_name = (char *) "add";
	js_function->native_variables = (char *) "(object, a, b)";
	js_function->nativeFunction = &add;

	return js_function;
}
