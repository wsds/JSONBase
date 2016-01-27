#include "importModules.h"
typedef void* create_type();

void resolveNativeFUnction(JSFunction * js_function) {
	js_function->function_name = new JSString(js_function->native_function_name);

	JSString * variableStr = new JSString(js_function->native_variables);
	CodeLine* codeLine = new CodeLine();
	codeLine->codeElementList = new LIST();
	codeLine->codeLineStr = variableStr;
	int resolveResult = resolveElement(variableStr->char_string, variableStr->length, codeLine);

	js_function->variables = new LIST();

	int skipToIndex = -1;
	for (int i = 0; i < codeLine->codeElementList->length; i++) {
		CodeElement * codeElement = (CodeElement *) codeLine->codeElementList->find(i);
		if (codeElement->type == NAME) {
			MetaExecutable * metaExecutable = new MetaExecutable();

			metaExecutable->codeElement = codeElement;

			js_function->variables->push(metaExecutable);
		}
	}
	js_function->functionDefinition = NULL;

}
void importModules(ExecuteContext * context) {
	void * handle = dlopen("/alidata/swift1/JSAddModule/Debug/libAdd.so", RTLD_LAZY);
	if (handle == NULL) {
		char * error_message = dlerror();
		int i;
		i = i + 1;
	}
	create_type * create_void = (create_type *) dlsym(handle, "create");
//	void * create();
//	create = (void*())create_void;
	Module * module = create_void();
	if (module != NULL) {
		JSFunction * js_function = module->importFunction();
		resolveNativeFUnction(js_function);
		context->importClosure->set(js_function->function_name, js_function);

		int i;
		i = i + 1;
	}
//	delete module;
//	dlclose(handle);

	Module * module1 = (Module*)new JSLogModule();
	if (module1 != NULL) {
		JSFunction * js_function = module1->importFunction();
		resolveNativeFUnction(js_function);
		context->importClosure->set(js_function->function_name, js_function);
	}
}
