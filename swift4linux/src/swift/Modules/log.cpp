#include "log.h"

JSObject * myLog(ExecuteContext * context) {
	JSString * variable_name = new JSString("object");
	JSObject * object = getValueFromClosure(variable_name, context);
	JSLog(object);
	return NULL;
}

JSFunction * JSLogModule::importFunction() {

	JSFunction * js_function = new JSFunction();

	js_function->native_function_name = "JSLog";
	js_function->native_variables = "(object)";

	js_function->nativeFunction = &myLog;

	return js_function;

}

