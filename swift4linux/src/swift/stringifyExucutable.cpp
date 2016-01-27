#include "stringifyExecutable.h"

JSON * stringify(Executable * executable) { //runtime polymorphism
	if (executable == NULL) {
		return NULL;
	}
	JSON* json = NULL;
	if (executable->type == META) {
		json = stringify((MetaExecutable*) executable);
	} else if (executable->type == EXPRESSION) {
		json = stringify((Expression*) executable);
	} else if (executable->type == OPERATOR) {
		json = stringify((Operator*) executable);
	} else if (executable->type == FUNCTIONCALL) {
		json = stringify((FunctionCall*) executable);
	} else if (executable->type == FUNCTIONDEFINITION) {
		json = stringify((FunctionDefinition*) executable);
	} else if (executable->type == EXCUTEABLEBLOCK) {
		json = stringify((ExecutableBlock*) executable);
	} else if (executable->type == FUNCTIONRETURN) {
		json = stringify((FunctionReturn*) executable);
	} else if (executable->type == CLASSDEFINITION) {
		json = stringify((ClassDefinition*) executable);
	} else if (executable->type == IFBLOCK) {
		json = stringify((IfBlock*) executable);
	} else if (executable->type == FORBLOCK) {
		json = stringify((ForBlock*) executable);
	} else if (executable->type == FORINBLOCK) {
		json = stringify((ForInBlock*) executable);
	}
	return json;
}

JSON * stringify(FunctionReturn * functionReturn) {
	JSON* json = new JSON();
	(*json)["type"] = "FunctionReturn";

	JSON* variable_json = stringify(functionReturn->variable);
	(*json)["variable"] = variable_json;

	return json;
}

JSON * stringify(ExecutableBlock * executableBlock) {
	JSON* json = new JSON();
	(*json)["type"] = "ExecutableBlock";
	Executable* executable = NULL;
	for (int i = 0; i < executableBlock->executables->length; i++) {
		executable = (Executable*) executableBlock->executables->find(i);
		if (executable != NULL) {
			JSON* sub_json = stringify(executable);
			json->push(sub_json);
		} else {
			JSString * null_str = new JSString();
			*null_str = "NULL";
			json->push(null_str);
		}
	}
	return json;
}

JSON * stringify(Operator * js_operator) {
	JSON* json = new JSON();
	(*json)["type"] = "Operator";

	JSString * operator_str = new JSString();

	operator_str->char_string[0] = js_operator->code_operator;
	operator_str->length = 1;
	if (js_operator->code_operator2 != 0) {
		operator_str->char_string[1] = js_operator->code_operator2;
		operator_str->length = 2;
	}
	(*json)["OPERATOR"] = operator_str;

	return json;
}

JSON * stringify(MetaExecutable * meta) {
	JSON* json = new JSON();
	(*json)["type"] = "Meta";

	CodeElement* codeElement = meta->codeElement;
	if (codeElement->type == CODE_NUMBER) {
		(*json)["NUMBER"] = codeElement->number;
	} else if (codeElement->type == CODE_STRING) {
		(*json)["STRING"] = codeElement->char_string;
	} else if (codeElement->type == CODE_JSON) {
		(*json)["JSONStr"] = codeElement->jsonstr;
	} else if (codeElement->type == NAME) {
		(*json)["NAME"] = codeElement->variable_name;
	} else if (codeElement->type == KEYWORD) {
		(*json)["KEYWORD"] = codeElement->keyword;
	}

	return json;
}

JSON * stringify(Expression * expression) {
	JSON* json = new JSON();
	(*json)["type"] = "Expression";
	if (expression->isNew == true) {
		(*json)["isNew"] = true;
	}
	Executable* executable = NULL;
	for (int i = 0; i < expression->executables->length; i++) {
		executable = (Executable*) expression->executables->find(i);
		JSON* sub_json = stringify(executable);
		json->push(sub_json);

	}

	return json;
}

JSON * stringify(FunctionDefinition * functionDefinition) {
	JSON* json = new JSON();
	(*json)["type"] = "FunctionDefinition";

	(*json)["functionName"] = functionDefinition->functionName;

	JSON* variables_json = new JSON();
	(*json)["variables"] = variables_json;
	Executable* executable = NULL;
	for (int i = 0; i < functionDefinition->variables->length; i++) {
		executable = (Executable*) functionDefinition->variables->find(i);
		JSON* variable_json = stringify(executable);
		variables_json->push(variable_json);
	}

	JSON* executableBlock_json = stringify(functionDefinition->executableBlock);
	(*json)["executableBlock"] = executableBlock_json;

	return json;
}

JSON * stringify(ClassDefinition * classDefinition) {
	return NULL;
}

JSON * stringify(FunctionCall * functionCall) {
	JSON* json = new JSON();
	(*json)["type"] = "FunctionCall";

	(*json)["functionName"] = functionCall->functionName;
	if (functionCall->isNew == true) {
		(*json)["isNew"] = true;
	}

	JSON* variables_json = new JSON();
	(*json)["variables"] = variables_json;
	Executable* executable = NULL;
	for (int i = 0; i < functionCall->variables->length; i++) {
		executable = (Executable*) functionCall->variables->find(i);
		JSON* variable_json = stringify(executable);
		variables_json->push(variable_json);
	}

	return json;

}

JSON * stringify(IfBlock * ifBlock) {
	JSON* json = new JSON();
	(*json)["type"] = "IfBlock";

	JSON* condition_json = stringify(ifBlock->condition);
	(*json)["condition"] = condition_json;

	JSON* executableBlock_json = stringify(ifBlock->executableBlock);
	(*json)["executableBlock"] = executableBlock_json;

	if (ifBlock->next != NULL) {
		(*json)["hasNext"] = true;
	}
	if (ifBlock->isElse == true) {
		(*json)["isElse"] = true;
	}
	return json;
}

JSON * stringify(ForBlock * forBlock) {

	return NULL;
}

JSON * stringify(ForInBlock * forInBlock) {

	return NULL;
}

