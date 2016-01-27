#include "parseCodeLine.h"

KeyWords * KeyWords::instance = NULL;
KeyWords::KeyWords(){

	this->keyWordMap = NULL;

	this->string_var = "var";
	this->string_new = "new";

	this->string_if = "if";
	this->string_else = "else";

	this->string_for = "for";
	this->string_in = "in";
	this->string_while = "while";
	this->string_do = "do";


	this->string_func = "func";
	this->string_return = "return";

	this->string_class = "class";
	this->string_instance = "instance";
	this->string_this = "this";
	this->string_self = "self";


}

bool is_initialized = false;
void initializeKeyWordMap(KeyWords * keyWords) {

	JSON * keyWordMap = new JSON();
	keyWordMap->initialize();

	(*keyWordMap)[keyWords->string_var] = 888668;
	(*keyWordMap)[keyWords->string_new] = 888668;

	(*keyWordMap)[keyWords->string_if] = 888668;
	(*keyWordMap)[keyWords->string_else] = 888668;

	(*keyWordMap)[keyWords->string_for] = 888668;
	(*keyWordMap)[keyWords->string_in] = 888668;

	(*keyWordMap)[keyWords->string_while] = 888668;
	(*keyWordMap)[keyWords->string_do] = 888668;

	(*keyWordMap)[keyWords->string_func] = 888668;
	(*keyWordMap)[keyWords->string_return] = 888668;

	(*keyWordMap)[keyWords->string_class] = 888668;
	(*keyWordMap)[keyWords->string_instance] = 888668;
	(*keyWordMap)[keyWords->string_this] = 888668;
	(*keyWordMap)[keyWords->string_self] = 888668;

	keyWords->keyWordMap = keyWordMap;
	is_initialized = true;
}

bool isKeyWord(JSString* name) {
	KeyWords * keyWords = KeyWords::getInstance();
	if (is_initialized == false) {

		initializeKeyWordMap(keyWords);
		is_initialized = true;
	}

	JSObject* number = keyWords->keyWordMap->get(name);
	if (number == NULL) {
		return false;
	} else {
		return true;
	}
}

int BRACKET_element_counter = 0;
int SINGLEQUOTE_element_counter = 0;
int DOUBLEQUOTES_element_counter = 0;
int resolveElementStatus = 1; //[1:normal,2:json,[321,323,311,313]:string,1-2,2-1,1-3,3-1]

int resolveElement(char* from, int length, CodeLine* codeLine) {

	char localChar;
	int codeElementType = CODE_NUMBER;

	BRACKET_element_counter = 0;

	int pre_blank = 0;
	if (resolveElementStatus != 1) {
		localChar = 10;
	}

	for (int i = 0; i < length; i++) {
		localChar = from[i];
		if (localChar == BLANK) {
			pre_blank++;
		} else {
			break;
		}
	}

	int tail_blank = 0;

	for (int i = length - 1; i > 0; i--) {
		localChar = from[i];
		if (localChar == BLANK) {
			tail_blank++;
		} else {
			break;
		}
	}
	if (pre_blank + tail_blank == length) {
		return 0;
	}

	for (int i = pre_blank; i < length - tail_blank; i++) {
		localChar = from[i];
		if (resolveElementStatus == 1) {
			if (localChar == BLANK) {
				//resolve the left code
				int elementCount = resolveElement(from, i, codeLine);

				//resolve the right code
				elementCount = elementCount + resolveElement(from + i, length - i, codeLine);

				return elementCount;

			} else if (localChar == COMMA || localChar == SEMICOLON) {

				//resolve the left code
				int elementCount = resolveElement(from, i, codeLine);

				CodeElement * delimiter = new CodeElement();
				delimiter->isResolvedDelimiter = false;
				codeLine->codeElementList->push(delimiter);
				delimiter->type = DELIMITER;
				//resolve the right code
				elementCount = elementCount + 1 + resolveElement(from + i + 1, length - i - 1, codeLine);

				return elementCount;
			} else if (localChar == SINGLEQUOTE) {
				resolveElementStatus = 311;
			} else if (localChar == DOUBLEQUOTES) {
				resolveElementStatus = 321;
			} else if (localChar == LEFTBRACKET) {
				BRACKET_element_counter++;
				resolveElementStatus = 2;
			} else if (localChar == RIGHTBRACKET) {
				//report error
			} else if (localChar == LEFTSMALLBRACKET || localChar == RIGHTSMALLBRACKET || localChar == LEFTBIGBRACKET || localChar == RIGHTBIGBRACKET) {
				//resolve the left code
				int elementCount = resolveElement(from, i, codeLine);

				CodeElement * bracket = new CodeElement();
				codeLine->codeElementList->push(bracket);
				bracket->bracket = localChar;
				bracket->type = BRACKET;

				//resolve the right code
				elementCount = elementCount + 1 + resolveElement(from + i + 1, length - i - 1, codeLine);

				return elementCount;

			} else if ((64 >= localChar && localChar >= 58) || (47 >= localChar && localChar >= 32) || localChar == 94 || localChar == 124) {
				//resolve the left code
				int elementCount = resolveElement(from, i, codeLine);

				CodeElement * code_operator = new CodeElement();
				codeLine->codeElementList->push(code_operator);
				code_operator->code_operator = localChar;
				code_operator->type = CODEOPERATOR;

				//resolve the right code
				elementCount = elementCount + 1 + resolveElement(from + i + 1, length - i - 1, codeLine);

				return elementCount;
			}
		} else if (resolveElementStatus == 2) {
			if (localChar == RIGHTBRACKET) {
				BRACKET_element_counter--;
				if (BRACKET_element_counter == 0) {
					resolveElementStatus = 1;
					codeElementType = CODE_JSON;
				}
			} else if (localChar == LEFTBRACKET) {
				BRACKET_element_counter++;
			}

		} else if (int(resolveElementStatus / 100) == 3) {
			if (resolveElementStatus == 311) {
				if (localChar == SINGLEQUOTE) {
					resolveElementStatus = 1;
					codeElementType = CODE_STRING;
				}
			}

			else if (resolveElementStatus == 321) {
				if (localChar == DOUBLEQUOTES) {
					resolveElementStatus = 1;
					codeElementType = CODE_STRING;
				}
			}
		}

	}
	CodeElement * element = new CodeElement();
	codeLine->codeElementList->push(element);

	if (codeElementType == CODE_STRING) {

		JSString* string = new JSString(0);
		string->set(from + pre_blank + 1, length - pre_blank - tail_blank - 2);

		element->type = CODE_STRING;
		element->char_string = string;

	} else {
		JSString* name = new JSString(0);
		name->set(from + pre_blank, length - pre_blank - tail_blank);
		if (codeElementType == CODE_JSON) {

			element->type = CODE_JSON;
			element->jsonstr = name;

		} else if (isNumber(name->char_string, name->length)) {

			element->type = CODE_NUMBER;
			element->number = parseStringToNubmer(name->char_string, name->length);

		}

		else if (isKeyWord(name)) {

			element->type = KEYWORD;
			element->keyword = name;

		} else {
			element->type = NAME;
			element->variable_name = name;
			element->child = NULL;
		}
	}

	return 1;
}

void resolveOperators(CodeLine* codeLine) {

	for (int i = 0; i < codeLine->codeElementList->length - 1; i++) {
		if (codeLine->codeElements[i]->type == CODEOPERATOR && codeLine->codeElements[i + 1]->type == CODEOPERATOR) {
			if ((codeLine->codeElements[i]->code_operator == '=' && codeLine->codeElements[i + 1]->code_operator == '=') || (codeLine->codeElements[i]->code_operator == '!' && codeLine->codeElements[i + 1]->code_operator == '=') || (codeLine->codeElements[i]->code_operator == '+' && codeLine->codeElements[i + 1]->code_operator == '=') || (codeLine->codeElements[i]->code_operator == '-' && codeLine->codeElements[i + 1]->code_operator == '=') || (codeLine->codeElements[i]->code_operator == '*' && codeLine->codeElements[i + 1]->code_operator == '=') || (codeLine->codeElements[i]->code_operator == '/' && codeLine->codeElements[i + 1]->code_operator == '=') || (codeLine->codeElements[i]->code_operator == '%' && codeLine->codeElements[i + 1]->code_operator == '=')
					|| (codeLine->codeElements[i]->code_operator == '>' && codeLine->codeElements[i + 1]->code_operator == '=') || (codeLine->codeElements[i]->code_operator == '<' && codeLine->codeElements[i + 1]->code_operator == '=') || (codeLine->codeElements[i]->code_operator == '&' && codeLine->codeElements[i + 1]->code_operator == '&') || (codeLine->codeElements[i]->code_operator == '|' && codeLine->codeElements[i + 1]->code_operator == '|') || (codeLine->codeElements[i]->code_operator == '+' && codeLine->codeElements[i + 1]->code_operator == '+') || (codeLine->codeElements[i]->code_operator == '-' && codeLine->codeElements[i + 1]->code_operator == '-')) {
				codeLine->codeElements[i]->code_operator2 = codeLine->codeElements[i + 1]->code_operator;
				codeLine->codeElements[i + 1]->type = SKIP;
			}
		}
	}

}

void resolveChild(CodeElement * codeElement) {
	int length = codeElement->variable_name->length;

	for (int i = 0; i < length; i++) {
		if (*(codeElement->variable_name->char_string + i) == '.') {

			JSString* string = new JSString(0);
			string->set(codeElement->variable_name->char_string + i, length - i);
			CodeElement* childElement = new CodeElement();
			codeElement->child = childElement;
			childElement->type = NAME;
			childElement->variable_name = string;
			childElement->child = NULL;

			resolveChild(codeElement->child);
		}
	}
}
void resolveChildren(CodeLine* codeLine) {

	for (int i = 0; i < codeLine->codeElementList->length - 1; i++) {
		if (codeLine->codeElements[i]->type == NAME) {
			resolveChild(codeLine->codeElements[i]);
		}
	}

	for (int i = 0; i < codeLine->codeElementList->length - 1; i++) {
		//if (codeLine->codeElements[i]->type == CODEOPERATOR&&codeLine->codeElements[i + 1]->type == CODEOPERATOR){
		//	codeLine->codeElements[i]->code_operator2 = codeLine->codeElements[i + 1]->code_operator;
		//	codeLine->codeElements[i + 1]->type = SKIP;
		//}
	}

}

void resolveDot(CodeLine* codeLine) {

	for (int i = codeLine->codeElementList->length - 1; i >= 0; i--) {
		if (codeLine->codeElements[i]->type == CODEOPERATOR && codeLine->codeElements[i]->code_operator == '.') {
			if (codeLine->codeElementList->length - 1 > i && i > 0) {
				if (codeLine->codeElements[i + 1]->type == NAME && codeLine->codeElements[i - 1]->type == NAME) {

					codeLine->codeElements[i - 1]->child = codeLine->codeElements[i + 1];
					codeLine->codeElements[i + 1]->type = CHILDNAME;
					codeLine->codeElements[i]->type = CHILDNAME;
				}
			}
		}
	}
}

void resolveBracket(CodeLine* codeLine) {
//	ExecutableBlock* executableBlock;
	int index_LEFTSMALLBRACKET = 0;
	int index_RIGHTSMALLBRACKET = 0;

	CodeElement * codeElement;

	CodeElement * smallBrackets[5];
	int smallBracketsStackTop = 0;

	for (int ii = 0; ii < codeLine->codeElementList->length; ii++) {
		codeElement = codeLine->codeElements[ii];
		if (codeElement->type != BRACKET) {
			continue;
		}
		if (codeElement->bracket == LEFTSMALLBRACKET) {
			smallBrackets[smallBracketsStackTop] = codeElement;
			codeElement->preBracketIndex = ii;
			smallBracketsStackTop++;
		} else if (codeElement->bracket == RIGHTSMALLBRACKET) {
			codeElement->preBracketIndex = smallBrackets[smallBracketsStackTop - 1]->preBracketIndex;
			codeElement->nextBracketIndex = ii;
			smallBrackets[smallBracketsStackTop - 1]->nextBracketIndex = ii;

			smallBracketsStackTop--;
			if (smallBracketsStackTop < 0) {
				break;
				//report error
			}
		} //todo many bracket >5
		else if (codeElement->bracket == LEFTBIGBRACKET) {
			codeLine->funtionLevel++;
		} else if (codeElement->bracket == RIGHTBIGBRACKET) {
			codeLine->funtionLevel--;
		}
	}

}
CodeLine* resolveCodeLine(JSString * codeLineStr) {
	CodeLine* codeLine = new CodeLine();
	codeLine->codeElementList = new LIST();
	codeLine->codeLineStr = codeLineStr;
	int resolveResult = resolveElement(codeLineStr->char_string, codeLineStr->length, codeLine);
	codeLine->codeElements = (CodeElement**) codeLine->codeElementList->elements;
	resolveOperators(codeLine);
	resolveBracket(codeLine);
	resolveDot(codeLine);
	return codeLine;
}
