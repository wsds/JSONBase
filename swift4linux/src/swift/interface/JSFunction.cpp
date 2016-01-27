#include "JSFunction.h"


JSFunction::JSFunction(){
	this->type = JSFUNCTION;
	this->char_string = NULL;
	this->nativeFunction = NULL;
	this->functionDefinition = NULL;
	this->closure = NULL;
}

void JSFunction::free(){

}
