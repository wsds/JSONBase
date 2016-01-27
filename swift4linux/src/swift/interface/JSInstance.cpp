#include "JSInstance.h"


JSInstance::JSInstance(){
	this->type = JSINSTANCE;
	(*(this->function))(NULL, NULL);
}

void test123(JSObject* parameter, JSObject* result){

	parameter->number = 123;

}

void JSInstance::test(){

	this->function = test123;

}

