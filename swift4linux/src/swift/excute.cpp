#include "execute.h"

JSObject* execute(Executable * executable, ExecuteContext * context) { //runtime polymorphism
	JSObject* result = NULL;
	if (executable->type == EXPRESSION) {
		result = execute((Expression*) executable, context);
	} else if (executable->type == FUNCTIONCALL) {
		result = execute((FunctionCall*) executable, context);
	} else if (executable->type == FUNCTIONDEFINITION) {
		execute((FunctionDefinition*) executable, context);
	} else if (executable->type == EXCUTEABLEBLOCK) {
		result = execute((ExecutableBlock*) executable, context);
	} else if (executable->type == FUNCTIONRETURN) {
		result = execute((FunctionReturn*) executable, context);
	} else if (executable->type == CLASSDEFINITION) {
		result = execute((ClassDefinition*) executable, context);
	} else if (executable->type == IFBLOCK) {
		result = execute((IfBlock*) executable, context);
	} else if (executable->type == FORBLOCK) {
		result = execute((ForBlock*) executable, context);
	} else if (executable->type == FORINBLOCK) {
		result = execute((ForInBlock*) executable, context);
	} else if (executable->type == META) {
		result = execute((MetaExecutable*) executable, context);
	}
	return result;
}

JSObject* execute(FunctionReturn * functionReturn, ExecuteContext * context) {
	JSObject* result = NULL;
	result = execute(functionReturn->variable, context);

	if (functionReturn->variable->type != EXCUTEABLEBLOCK) {
		result = functionReturn->variable->value;
	} else {
		ExecutableBlock * executableBlock = (ExecutableBlock*) functionReturn->variable;
		JSON* json = new JSON();
		for (int i = 0; i < executableBlock->executables->length; i++) {
			ResultedExecutable * resultedExecutable = (ResultedExecutable*) executableBlock->executables->find(i);
			json->push(resultedExecutable->value);
		}
		result = json;
	}

	return result;
}

JSObject* execute(ExecutableBlock * executableBlock, ExecuteContext * context) {
	JSObject* result = NULL;
	Executable* executable = NULL;
	for (int i = 0; i < executableBlock->executables->length; i++) {
		executable = (Executable*) executableBlock->executables->find(i);
		result = execute(executable, context);
		if (executable->type == FUNCTIONRETURN) {
			break;
		}
	}
	executableBlock->value = result;
	return result;
}

JSObject * getValueFromJSON(CodeElement * codeElement, JSON * json) {
	JSObject * js_value = NULL;
	if (codeElement->type == NAME || codeElement->type == CHILDNAME) {
		js_value = json->get(codeElement->variable_name);
//		char* test = stringifyJSON(json);
	}

	if (codeElement->child == NULL) {
		return js_value;
	} else if (js_value != NULL) {
		if (js_value->type == JSCLASS) {
			return getValueFromJSON(codeElement->child, ((JSClass*) (js_value))->children);
		} else if (js_value->type == JSJSON) {
			return getValueFromJSON(codeElement->child, (JSON*) (js_value));
		}
	}
	return NULL;
}

JSKeyValue * getKeyValueFromJSON(CodeElement * codeElement, JSON * json) {
	JSKeyValue * jsKeyValue = NULL;
	if (codeElement->type == NAME || codeElement->type == CHILDNAME) {
		jsKeyValue = (JSKeyValue*) json->get(codeElement->variable_name);
//		char* test = stringifyJSON(json);
	}

	if (codeElement->child == NULL) {
		return jsKeyValue;
	} else if (jsKeyValue != NULL) {
		if (jsKeyValue->value->type == JSCLASS) {
			return getKeyValueFromJSON(codeElement->child, ((JSClass*) (jsKeyValue->value))->children);
		} else if (jsKeyValue->value->type == JSJSON) {
			return getKeyValueFromJSON(codeElement->child, (JSON*) (jsKeyValue->value));
		}
	}
	return NULL;
}

JSKeyValue * getKeyValueFromClosure(CodeElement* codeElement, ExecuteContext * context) {
	JSKeyValue * js_keyValue = NULL;
	if (codeElement->type == NAME) {
		js_keyValue = context->currentClosure->lookupKeyValue(codeElement->variable_name);
	}

	if (codeElement->child == NULL) {
		return js_keyValue;
	} else if (js_keyValue != NULL) {
		if (js_keyValue->type == JSCLASS) {
			return getKeyValueFromJSON(codeElement->child, ((JSClass*) js_keyValue)->children);
		} else if (js_keyValue->type == JSJSON) {
			return getKeyValueFromJSON(codeElement->child, (JSON*) (js_keyValue));
		}
	}
	return NULL;
}

JSObject * getValueFromClosure(CodeElement * codeElement, ExecuteContext * context) {
	JSObject * js_value = NULL;
	if (codeElement->type == NAME) {
		js_value = context->currentClosure->lookup(codeElement->variable_name);
	}

	if (codeElement->child == NULL) {
		return js_value;
	} else if (js_value != NULL) {
		if (js_value->type == JSCLASS) {
			return getValueFromJSON(codeElement->child, ((JSClass*) js_value)->children);
		} else if (js_value->type == JSJSON) {
			return getValueFromJSON(codeElement->child, (JSON*) (js_value));
		}
	}
	return NULL;
}

JSObject * getValueFromClosure(JSString * key, ExecuteContext * context) {
	JSObject * js_value = NULL;
	js_value = (JSKeyValue*) context->currentClosure->lookup(key);
	return js_value;
}

JSObject* excuteOperator(Operator* code_operator, ExecuteContext * context) {
	JSObject* result = NULL;

	JSObject* leftValue = code_operator->left->value;
	JSObject* rightValue = code_operator->right->value;
	if (code_operator->code_operator == '=' && code_operator->code_operator2 == NULL) {
		result = excuteAssignment(code_operator->right, code_operator->left, context);
	} else if (code_operator->code_operator == '*' && code_operator->code_operator2 == NULL) {
		if (leftValue->type == JSNUMBER && rightValue->type == JSNUMBER) {
			result = new JSNumber();
			result->number = leftValue->number * rightValue->number;
		}
	} else if (code_operator->code_operator == '/' && code_operator->code_operator2 == NULL) {
		if (leftValue->type == JSNUMBER && rightValue->type == JSNUMBER) {
			result = new JSNumber();
			result->number = leftValue->number / rightValue->number;
		}
	} else if (code_operator->code_operator == '%' && code_operator->code_operator2 == NULL) {
		if (leftValue->type == JSNUMBER && rightValue->type == JSNUMBER) {
			result = new JSNumber();
			result->number = leftValue->number % rightValue->number;
		}
	}

	else if (code_operator->code_operator == '+' && code_operator->code_operator2 == NULL) {
		if (leftValue->type == JSNUMBER && rightValue->type == JSNUMBER) {
			result = new JSNumber();
			result->number = leftValue->number + rightValue->number;
		}
	} else if (code_operator->code_operator == '-' && code_operator->code_operator2 == NULL) {
		if (leftValue->type == JSNUMBER && rightValue->type == JSNUMBER) {
			result = new JSNumber();
			result->number = leftValue->number - rightValue->number;
		}
	} else if (code_operator->code_operator == '>' && code_operator->code_operator2 == NULL) {
		if (leftValue->type == JSNUMBER && rightValue->type == JSNUMBER) {
			result = new JSNumber();
			result->number = leftValue->number > rightValue->number;
		}
	} else if (code_operator->code_operator == '<' && code_operator->code_operator2 == NULL) {
		if (leftValue->type == JSNUMBER && rightValue->type == JSNUMBER) {
			result = new JSNumber();
			result->number = leftValue->number < rightValue->number;
		}
	} else if (code_operator->code_operator == '>' && code_operator->code_operator2 == '=') {
		if (leftValue->type == JSNUMBER && rightValue->type == JSNUMBER) {
			result = new JSNumber();
			result->number = leftValue->number >= rightValue->number;
		}
	} else if (code_operator->code_operator == '<' && code_operator->code_operator2 == '=') {
		if (leftValue->type == JSNUMBER && rightValue->type == JSNUMBER) {
			result = new JSNumber();
			result->number = leftValue->number <= rightValue->number;
		}
	} else if (code_operator->code_operator == '=' && code_operator->code_operator2 == '=') {
		if (leftValue->type == JSNUMBER && rightValue->type == JSNUMBER) {
			result = new JSNumber();
			result->number = leftValue->number == rightValue->number;
		}
	} else if (code_operator->code_operator == '!' && code_operator->code_operator2 == '=') {
		if (leftValue->type == JSNUMBER && rightValue->type == JSNUMBER) {
			result = new JSNumber();
			result->number = leftValue->number != rightValue->number;
		}
	} else if (code_operator->code_operator == '&' && code_operator->code_operator2 == '&') {
		if (leftValue->type == JSNUMBER && rightValue->type == JSNUMBER) {
			result = new JSNumber();
			result->number = leftValue->number && rightValue->number;
		}
	} else if (code_operator->code_operator == '|' && code_operator->code_operator2 == '|') {
		if (leftValue->type == JSNUMBER && rightValue->type == JSNUMBER) {
			result = new JSNumber();
			result->number = leftValue->number || rightValue->number;
		}
	} else if (code_operator->code_operator == '!' && code_operator->code_operator2 == NULL && rightValue != NULL) {
		if (rightValue->type == JSNUMBER) {
			result = new JSNumber();
			result->number = !rightValue->number;
		}
	} else if (code_operator->code_operator == '+' && code_operator->code_operator2 == '+' && leftValue != NULL) {
		if (leftValue->type == JSNUMBER) {
			result = new JSNumber();
			result->number = leftValue->number + 1;
		}
	} else if (code_operator->code_operator == '+' && code_operator->code_operator2 == '+' && rightValue != NULL) {
		if (rightValue->type == JSNUMBER) {
			result = new JSNumber();
			result->number = rightValue->number + 1;
		}
	} else if (code_operator->code_operator == '-' && code_operator->code_operator2 == '-' && leftValue != NULL) {
		if (leftValue->type == JSNUMBER) {
			result = new JSNumber();
			result->number = leftValue->number - 1;
		}
	} else if (code_operator->code_operator == '-' && code_operator->code_operator2 == '-' && rightValue != NULL) {
		if (rightValue->type == JSNUMBER) {
			result = new JSNumber();
			result->number = rightValue->number - 1;
		}
	}
	return result;
}

JSObject* execute(MetaExecutable* meta, ExecuteContext * context) {
	JSObject* result = NULL;

	JSObject * value = NULL;
	if (meta->value == NULL) {
		if (meta->codeElement->type == CODE_NUMBER) {
			value = (JSObject *) (new JSNumber(meta->codeElement->number));
		} else if (meta->codeElement->type == CODE_STRING) {
			JSString * js_string = new JSString(meta->codeElement->char_string->length);
			js_string->copy(meta->codeElement->char_string->char_string, meta->codeElement->char_string->length);
			value = (JSObject *) js_string;
		} else if (meta->codeElement->type == CODE_JSON) {
			JSON* json = parseJSON(meta->codeElement->jsonstr);
			value = (JSObject *) json;
		} else if (meta->codeElement->type == NAME) {
			value = getValueFromClosure(meta->codeElement, context);
			meta->keyValue = getKeyValueFromClosure(meta->codeElement, context);
		}
		meta->value = value;
	} else {
		if (meta->codeElement->type == NAME) {
			meta->value = getValueFromClosure(meta->codeElement, context);
			meta->keyValue = getKeyValueFromClosure(meta->codeElement, context);
		}
	}

	return meta->value;
}

#define DOUBLEOPERATOR 0
#define PREFIX 1
#define POSTFIX 2

void mergeOperator(LIST* executableList, Operator* codeOperator, ExecuteContext * context) {
	int index = executableList->findValue(codeOperator);

	if (index - 1 >= 0 && codeOperator->operator_type != PREFIX) {
		Executable* left = (Executable*) executableList->find(index - 1);
		if (left->type == META || left->type == EXPRESSION || left->type == FUNCTIONCALL || left->type == RESULTED || left->type == EXCUTEABLEBLOCK) {
			codeOperator->left = (ResultedExecutable*) left;
		}
	}
	if (index + 1 < executableList->length && codeOperator->operator_type != POSTFIX) {
		Executable* right = (Executable*) executableList->find(index + 1);
		if (right->type == META || right->type == EXPRESSION || right->type == FUNCTIONCALL || right->type == RESULTED || right->type == EXCUTEABLEBLOCK) {
			codeOperator->right = (ResultedExecutable*) right;
		}
	}

	ResultedExecutable* resultedExecutable = new ResultedExecutable();

	JSObject* result = excuteOperator(codeOperator, context);
	resultedExecutable->value = result;

	executableList->replace(resultedExecutable, index);
	if (codeOperator->operator_type == DOUBLEOPERATOR) {
		executableList->del(index + 1);
		executableList->del(index - 1);
	} else if (codeOperator->operator_type == PREFIX) {
		executableList->del(index + 1);
	}
	if (codeOperator->operator_type == POSTFIX) {
		executableList->del(index - 1);
	}
}

JSObject* resolveExecutableList(LIST* executableList, ExecuteContext * context) {
	JSObject* result = NULL;
	for (int i = 0; i < executableList->length; i++) {
		Executable * executable = executableList->find(i);
		if (executable->type == EXPRESSION) {
			result = execute((Expression*) executable, context);
		} else if (executable->type == META) {
			result = execute((MetaExecutable*) executable, context);
		} else if (executable->type == FUNCTIONCALL) {
			result = execute((FunctionCall*) executable, context);
		} else if (executable->type == EXCUTEABLEBLOCK) {
			result = execute((ExecutableBlock*) executable, context);
		}
	}
	return result;
}

void resolveOperatorList(LIST* executableList, LIST* operatorList, char* operatorStr, int operator_type) {
	int operatorStr_length = getLength(operatorStr);

	for (int i = 0; i < executableList->length; i++) {
		Executable* executable = (Executable*) executableList->find(i);

		if (executable->type == OPERATOR) {
			Operator* codeOperator = (Operator*) executable;

			int index = 0;
			char localChar = 0;
			char lastChar = 0;
			bool isSpecifiedOperator = false;
			for (int j = 0; j < operatorStr_length; j++) {
				localChar = operatorStr[j];

				if (codeOperator->code_operator2 == NULL) {
					if (index == 0) {
						char nextChar = operatorStr[j + 1];
						if (codeOperator->code_operator == localChar && nextChar == ' ') {
							isSpecifiedOperator = true;
							break;
						}
					}
				} else {
					if (index == 1) {
						if (codeOperator->code_operator == localChar && codeOperator->code_operator2 == lastChar) {
							isSpecifiedOperator = true;
							break;
						}
					}
				}

				if (localChar != ' ') {
					index = (index + 1) % 2;
				} else {
					index = 0;
				}
				lastChar = localChar;
			}

			if (isSpecifiedOperator == true) {
				codeOperator->operator_type = operator_type;
				operatorList->push(codeOperator);
			}
		}
	}

}
JSObject* execute(Expression * expression, ExecuteContext * context) {
	JSObject* result = NULL;

	bool isNew = expression->isNew;

	Executable** executables;

	LIST* executableList = new LIST();
	int executable_length = expression->executables->length;
	for (int i = 0; i < executable_length; i++) {
		Executable* executable = (Executable*) expression->executables->find(i);
		executableList->push(executable);
	}
	executables = (Executable**) executableList->elements;

	if (isNew == true) {
		MetaExecutable * target = (MetaExecutable *) executables[0];
		if (target->type != META) {
			//report error
			return NULL;
		}

		newVariable(target, context);
	}

	result = resolveExecutableList(executableList, context);
	LIST * operatorList = new LIST();

	resolveOperatorList(executableList, operatorList, "!", PREFIX);
	resolveOperatorList(executableList, operatorList, "++ --", PREFIX);
	resolveOperatorList(executableList, operatorList, "* / %", DOUBLEOPERATOR);
	resolveOperatorList(executableList, operatorList, "+ -", DOUBLEOPERATOR);
	resolveOperatorList(executableList, operatorList, "> < >= <= == !=", DOUBLEOPERATOR);
	resolveOperatorList(executableList, operatorList, "&& ||", DOUBLEOPERATOR);
	resolveOperatorList(executableList, operatorList, "= += -= *= /= %=", DOUBLEOPERATOR);
	resolveOperatorList(executableList, operatorList, "++ --", POSTFIX);

	for (int i = 0; i < operatorList->length; i++) {
		Operator* codeOperator = (Operator*) operatorList->find(i);
		mergeOperator(executableList, codeOperator, context);

	}

	if (executableList->length == 1) {
		ResultedExecutable * resultedExecutable = (ResultedExecutable *) executableList->find(0);
		expression->value = resultedExecutable->value;
	}
	return expression->value;
}

JSObject* execute(FunctionDefinition * functionDefinition, ExecuteContext * context) {
	JSFunction * jsFunction = new JSFunction();
	jsFunction->function_name = functionDefinition->functionName;
	jsFunction->variables = functionDefinition->variables;
	jsFunction->functionDefinition = functionDefinition;
	jsFunction->closure = context->currentClosure;

	context->currentClosure->set(functionDefinition->functionName, jsFunction);

	JSObject * js_value = (JSObject *) context->currentClosure->lookup(functionDefinition->functionName);
	if (js_value->type != JSFUNCTION) {
		//report error
		return NULL;
	}

	return jsFunction;
}

JSObject* execute(ClassDefinition * classDefinition, ExecuteContext * context) {
	JSClass * jsClass = new JSClass();
	jsClass->className = classDefinition->className;
	jsClass->closure = context->currentClosure;

	Closure * closure = new Closure();
	closure->initialize();
	context->currentClosure->next = closure;
	closure->previous = context->currentClosure;

	context->currentClosure = context->currentClosure->next;

	ExecutableBlock* executableBlock = classDefinition->executableBlock;
	JSObject* result = execute(executableBlock, context);

	jsClass->children = context->currentClosure->variables;
	char * test = stringifyJSON(jsClass->children);

	context->currentClosure = context->currentClosure->previous;

	context->currentClosure->set(classDefinition->className, jsClass);
	return jsClass;
}

JSObject* execute(FunctionCall * functionCall, ExecuteContext * context) {
	JSObject* result = NULL;

	if (functionCall->functionName == NULL && functionCall->functionNameChain == NULL) {
		//report error
		return NULL;
	}
	JSObject * jsFunctionOrClass;
	JSKeyValue * jsFunctionKeyValue;
	if (functionCall->functionName != NULL) {
		jsFunctionOrClass = (JSObject *) context->currentClosure->lookup(functionCall->functionName);
	} else {
//		jsFunctionKeyValue = getFromClosure(functionCall->functionNameChain, context);
	}

	if (jsFunctionOrClass == NULL) {
		//report error
		return NULL;
	} else if (jsFunctionOrClass->type == JSFUNCTION) {
		JSFunction* jsFunction = (JSFunction*) jsFunctionOrClass;

		if (jsFunction->closure != NULL && context->currentClosure != jsFunction->closure) {
			context->currentClosure->next = jsFunction->closure;
			jsFunction->closure->previous = context->currentClosure;

			context->currentClosure = context->currentClosure->next;
		}
		result = resolveExecutableList(functionCall->variables, context);

		for (int i = 0; i < jsFunction->variables->length; i++) {
			MetaExecutable * variable = (MetaExecutable*) jsFunction->variables->find(i);
			ResultedExecutable * parameter = (ResultedExecutable*) functionCall->variables->find(i);
			variable->keyValue = parameter->keyValue;
			variable->value = parameter->value;
		}
		if (jsFunction->functionDefinition != NULL) {

			FunctionDefinition* functionDefinition = jsFunction->functionDefinition;
			result = execute((StructuredExecutable *) functionDefinition, context, jsFunction->variables);
		} else if (jsFunction->nativeFunction != NULL) {
			result = executeNativeFunction(jsFunction, context);
		}

	} else if (jsFunctionOrClass->type == JSCLASS) {
		JSClass* jsClass = (JSClass*) jsFunctionOrClass;
		if (functionCall->isNew == true) {
			JSClass* newJSClass = new JSClass();

			newJSClass->children = cloneJSON(jsClass->children);

			char* test = stringifyJSON(newJSClass->children);

			newJSClass->className = jsClass->className;
			newJSClass->closure = jsClass->closure;

			if (newJSClass->closure != NULL && context->currentClosure != newJSClass->closure) {
				context->currentClosure->next = newJSClass->closure;
				newJSClass->closure->previous = context->currentClosure;

				context->currentClosure = context->currentClosure->next;
			}

			LIST * jsVariables = new LIST();
//			resolveVariables(functionCall, jsVariables, context);

			result = newJSClass;
			if (newJSClass->closure != NULL && context->currentClosure != newJSClass->closure) {
				context->currentClosure = context->currentClosure->previous;
			}
		} else {
			result = jsClass;
		}
	}

	functionCall->value = result;

	return result;

}

JSObject* execute(StructuredExecutable * structuredExecutable, ExecuteContext * context) {
	JSObject* result = NULL;
	if (structuredExecutable == NULL) {
		return NULL;
	}
	Closure * closure = new Closure();
	closure->initialize();
	context->currentClosure->next = closure;
	closure->previous = context->currentClosure;
	context->currentClosure = context->currentClosure->next;

	bool pre_skipElse = context->skipElse;
	context->skipElse = true;

	result = execute(structuredExecutable->executableBlock, context);

	context->skipElse = pre_skipElse;
	context->currentClosure = context->currentClosure->previous;
	return result;
}

JSObject* execute(StructuredExecutable * structuredExecutable, ExecuteContext * context, LIST* variables) {
	JSObject* result = NULL;
	if (structuredExecutable == NULL) {
		return NULL;
	}
	Closure * closure = new Closure();
	closure->initialize();
	context->currentClosure->next = closure;
	closure->previous = context->currentClosure;
	context->currentClosure = context->currentClosure->next;

	for (int i = 0; i < variables->length; i++) {
		MetaExecutable * variable = (MetaExecutable*) variables->find(i);
		context->currentClosure->set(variable->codeElement->variable_name, variable->value);
	}

	bool pre_skipElse = context->skipElse;
	context->skipElse = true;

	result = execute(structuredExecutable->executableBlock, context);

	context->skipElse = pre_skipElse;
	context->currentClosure = context->currentClosure->previous;
	return result;
}

JSObject* executeNativeFunction(JSFunction* jsFunction, ExecuteContext * context) {
	JSObject* result = NULL;
	if (jsFunction == NULL) {
		return NULL;
	}
	Closure * closure = new Closure();
	closure->initialize();
	context->currentClosure->next = closure;
	closure->previous = context->currentClosure;
	context->currentClosure = context->currentClosure->next;

	for (int i = 0; i < jsFunction->variables->length; i++) {
		MetaExecutable * variable = (MetaExecutable*) jsFunction->variables->find(i);
		context->currentClosure->set(variable->codeElement->variable_name, variable->value);
	}

	bool pre_skipElse = context->skipElse;
	context->skipElse = true;
	result = jsFunction->nativeFunction(context);

	context->skipElse = pre_skipElse;
	context->currentClosure = context->currentClosure->previous;
	return result;
}

JSObject* execute(IfBlock * ifBlock, ExecuteContext * context) {
	if (ifBlock->isElse == true && context->skipElse == true) {
		return NULL;
	}
	JSObject* result = NULL;
	if (ifBlock->condition != NULL) {
		JSObject* jsObject = execute(ifBlock->condition, context);
		if (jsObject != NULL && jsObject->type == JSNUMBER) {
			JSNumber* boolean = (JSNumber*) jsObject;
			if (boolean->number == 1) {
				result = execute((StructuredExecutable *) ifBlock, context);
				context->skipElse = true;
			} else if (ifBlock->next != NULL) {
//				result = execute((IfBlock *) ifBlock->next, context);
				context->skipElse = false;
			}
		}
	} else {
		result = execute((StructuredExecutable *) ifBlock, context);
		context->skipElse = true;
	}

	return result;
}

JSObject* execute(ForBlock * forBlock, ExecuteContext * context) {
	JSObject* result = NULL;

	Closure * closure = new Closure();
	closure->initialize();
	context->currentClosure->next = closure;
	closure->previous = context->currentClosure;

	context->currentClosure = context->currentClosure->next;

//	DEBUGExecutable * iDEBUGExecutable = debugExecutable(forBlock->pre_executable);

	execute(forBlock->pre_executable, context);

	int i = 0;
	while (true) {
		for (i = 0; i < forBlock->conditions->length; i++) {
			JSNumber* conditionResult = (JSNumber*) execute((Executable*) forBlock->conditions->find(i), context);
			if (conditionResult->number == 0) {
				return result;
			}
		}

		result = execute(forBlock->executableBlock, context);

		execute(forBlock->last_executable, context);
	}

	context->currentClosure = context->currentClosure->previous;

	return result;
}

JSObject* execute(ForInBlock * forInBlock, ExecuteContext * context) {
	JSObject* result = NULL;
	JSKeyValue* iterator = (JSKeyValue*) getValueFromClosure(forInBlock->iteratorName, context);

	if (iterator != NULL && iterator->type == JSKEYVALUE && iterator->value->type == JSJSON) {
		Closure * closure = new Closure();
		closure->initialize();
		context->currentClosure->next = closure;
		closure->previous = context->currentClosure;

		context->currentClosure = context->currentClosure->next;

		forInBlock->iterator = (JSON*) iterator->value;
		if (forInBlock->keyName == NULL) {
			for (int i = 0; i < forInBlock->iterator->list->length; i++) {
				JSObject * value = forInBlock->iterator->list->find(i);
				if (value->type == JSKEYVALUE) {
					context->currentClosure->set(forInBlock->valueName, ((JSKeyValue*) value)->value);
				} else {
					context->currentClosure->set(forInBlock->valueName, value);
				}
				result = execute(forInBlock->executableBlock, context);
			}
		} else {
			for (int i = 0; i < forInBlock->iterator->list->length; i++) {
				JSObject * value = forInBlock->iterator->list->find(i);
				if (value->type == JSKEYVALUE) {
					JSKeyValue* jsKeyValue = (JSKeyValue*) value;
					JSString* keyString = new JSString();
					keyString->char_string = jsKeyValue->key;
					keyString->length = strlen(jsKeyValue->key);
					context->currentClosure->set(forInBlock->keyName, keyString);
					context->currentClosure->set(forInBlock->valueName, jsKeyValue->value);
					result = execute(forInBlock->executableBlock, context);
				}
			}
		}

		context->currentClosure = context->currentClosure->previous;
	} else {
		//report error
	}

	return result;
}

void newVariable(MetaExecutable * target, ExecuteContext * context) {
	context->currentClosure->set(target->codeElement->variable_name, context->none_object);
}

JSObject* excuteAssignment(ResultedExecutable * source, ResultedExecutable * target, ExecuteContext * context) {
	if (target->type == META) {
		if (target->keyValue != NULL) {
			JSON * target_json = (JSON *) target->keyValue->json;
			target_json->set(target->keyValue->key, target->keyValue->keyLength, source->value);
			//todo modify the old_value; when the value is JSSting or JSNumber
		}
	} else if (target->type == EXCUTEABLEBLOCK) {
		ExecutableBlock * targetExecutableBlock = (ExecutableBlock *) target;
		ExecutableBlock * sourceExecutableBlock = NULL;
		JSON *source_json = NULL;

		if (source->type == EXCUTEABLEBLOCK) {
			sourceExecutableBlock = (ExecutableBlock *) source;
		} else if (source->value->type == JSJSON) {
			source_json = (JSON*) source->value;
		}

		for (int i = 0; i < targetExecutableBlock->executables->length; i++) {
			ResultedExecutable * targetExecutable = (ResultedExecutable*) targetExecutableBlock->executables->find(i);
			if (targetExecutable->type == META) {
				if (targetExecutable->keyValue != NULL) {
					JSON * target_json = (JSON *) targetExecutable->keyValue->json;
					if (sourceExecutableBlock != NULL) {
						if (i < sourceExecutableBlock->executables->length) {
							ResultedExecutable * source_executable = sourceExecutableBlock->executables->find(i);
							JSObject * source_value = source_executable->value;
							target_json->set(targetExecutable->keyValue->key, targetExecutable->keyValue->keyLength, source_value);
						}
					} else if (source_json != NULL) {
						if (i < source_json->length) {
							JSObject * source_value = source_json->find(i);
							target_json->set(targetExecutable->keyValue->key, targetExecutable->keyValue->keyLength, source_value);
						}
					}
					//todo modify the old_value; when the value is JSSting or JSNumber
				}
			}
		}
	}

	return source->value;
}
