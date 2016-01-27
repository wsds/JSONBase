#include "analyzeCodeLine.h"

MetaExecutable::MetaExecutable() {
	this->type = META;
}

StructuredExecutable::StructuredExecutable() {
	this->executableBlock = NULL;
}

ResultedExecutable::ResultedExecutable() {
	this->value = NULL;
	this->keyValue = NULL;
	this->type = RESULTED;
}

Operator::Operator() {
	this->type = OPERATOR;
	this->code_operator = 0;
	this->code_operator2 = 0;
	this->left = NULL;
	this->right = NULL;
	this->operator_type = -1;
}

Expression::Expression() {
	this->type = EXPRESSION;
	this->isNew = false;
	this->executables = new LIST();
}
FunctionCall::FunctionCall() {
	this->type = FUNCTIONCALL;
	this->isNew = false;
	this->variables = new LIST();
	this->functionNameChain = NULL;
	this->functionName = NULL;
}
FunctionReturn::FunctionReturn() {
	this->type = FUNCTIONRETURN;
//	this->variables = new LIST();
	this->variable = NULL;
}
FunctionDefinition::FunctionDefinition() {
	this->type = FUNCTIONDEFINITION;
	this->executableBlock = NULL;
	this->variables = new LIST();

}
ClassDefinition::ClassDefinition() {
	this->type = CLASSDEFINITION;
	this->executableBlock = NULL;
	this->variables = new LIST();
}
InstanceDefinition::InstanceDefinition() {
	this->type = INSTANCEDEFINITION;
	this->executableBlock = NULL;
	this->variables = new LIST();

}

ExecutableBlock::ExecutableBlock() {
	this->executables = new LIST();
	this->type = EXCUTEABLEBLOCK;
}

IfBlock::IfBlock() {
	this->type = IFBLOCK;

	this->next = NULL;

	this->isElse = false;

	this->executableBlock = NULL;

}

ForBlock::ForBlock() {
	this->type = FORBLOCK;
	this->conditions = new LIST();
	this->executableBlock = NULL;
}

ForInBlock::ForInBlock() {
	this->type = FORINBLOCK;
	this->executableBlock = NULL;
}

Context::Context() {

	this->rootExecutableBlock = new ExecutableBlock();

	this->currentExecutableBlock = this->rootExecutableBlock;

	this->structuredExecutableStack = new LIST();

	this->pre_structuredExecutable = NULL;

	this->structuredExecutable = new StructuredExecutable();
	this->structuredExecutable->executableBlock = this->rootExecutableBlock;

	this->structuredLevel = 0;
}

Executable* analyzeCodeLine(CodeLine * codeLine, Context * context) {
	context->pre_structuredExecutable = context->structuredExecutable;
	Executable * executable = analyzeCodeLine(codeLine, 0, codeLine->codeElementList->length, context);
	context->currentExecutableBlock->executables->push(executable);

	if (context->structuredLevel == -1) {
		context->structuredExecutable = context->structuredExecutableStack->pop();

	} else if (context->structuredLevel == 1) {
		context->structuredExecutableStack->push(context->pre_structuredExecutable);
	}

	if (context->structuredExecutable != NULL) {
		context->currentExecutableBlock = context->structuredExecutable->executableBlock;
		context->structuredLevel = 0;
	} else {
		JSLog("swift error: invalid structuredExecutableStack");
	}
	return executable;
}

Executable* analyzeCodeLine(CodeLine * codeLine, int from, int end, Context * context) {

	KeyWords * keyWords = KeyWords::getInstance();

	ExecutableBlock * executableBlock = NULL;
	MetaExecutable * metaExecutable = NULL;

	CodeElement * codeElement = NULL;
	Expression * expression = NULL;

	FunctionCall * functionCall = NULL;

	FunctionDefinition * functionDefinition = NULL;
	ClassDefinition * classDefinition = NULL;
	InstanceDefinition * instanceDefinition = NULL;

	ForInBlock * forInBlock = NULL;
	ForBlock * forBlock = NULL;
	IfBlock * ifBlock = NULL;
	FunctionReturn * functionReturn = NULL;

	int skipToIndex = -1;
	for (int i = from; i < end; i++) {
		if (i < skipToIndex) {
			continue;
		}
		codeElement = codeLine->codeElements[i];
		if (codeElement->type == KEYWORD) {
			if (*codeElement->keyword == keyWords->string_var) {
				if (expression == NULL) {
					expression = new Expression();
					expression->isNew = true;
				}
			} else if (*codeElement->keyword == keyWords->string_func) {
				functionDefinition = new FunctionDefinition();
				context->structuredExecutable = functionDefinition;
				if (i + 1 < end && codeLine->codeElements[i + 1]->type == NAME) {
					functionDefinition->functionName = codeLine->codeElements[i + 1]->variable_name;
				}
			} else if (*codeElement->keyword == keyWords->string_class) {
				classDefinition = new ClassDefinition();
				context->structuredExecutable = classDefinition;

				if (i + 1 < end && codeLine->codeElements[i + 1]->type == NAME) {
					classDefinition->className = codeLine->codeElements[i + 1]->variable_name;
				}
			} else if (*codeElement->keyword == keyWords->string_instance) {
				instanceDefinition = new InstanceDefinition();
				context->structuredExecutable = instanceDefinition;

				if (i + 1 < end && codeLine->codeElements[i + 1]->type == NAME) {
					instanceDefinition->instanceName = codeLine->codeElements[i + 1]->variable_name;
				}
			} else if (*codeElement->keyword == keyWords->string_for) {
				bool isForIn = false;
				for (int ii = i + 1; ii < end; ii++) {
					CodeElement * innerElement = codeLine->codeElements[ii];
					if (innerElement->type == KEYWORD && innerElement->keyword == keyWords->string_in) {
						isForIn = true;
					}
				}

				if (isForIn == true) {
					forInBlock = new ForInBlock();
					context->structuredExecutable = forInBlock;

				} else {
					forBlock = new ForBlock();
					context->structuredExecutable = forBlock;

				}
			} else if (*codeElement->keyword == keyWords->string_if) {
				if (context->structuredExecutable == NULL || context->structuredExecutable->executableBlock != NULL) {
					ifBlock = new IfBlock();
					context->structuredExecutable = ifBlock;
				}
			} else if (*codeElement->keyword == keyWords->string_else) {
				LIST* currentExecutableList = context->currentExecutableBlock->executables;
				Executable * lastExecutable = currentExecutableList->find(currentExecutableList->length - 1);
				if (lastExecutable->type == IFBLOCK) {
					IfBlock * pre_ifBlock = (IfBlock *) lastExecutable;
					ifBlock = new IfBlock();
					ifBlock->isElse = true;
					pre_ifBlock->next = ifBlock;
					context->structuredExecutable = ifBlock;
				} else {
					//report error
				}
			} else if (*codeElement->keyword == keyWords->string_return) {
				functionReturn = new FunctionReturn();
			}
		} //KEYWORD
		else if (codeElement->type == BRACKET) {

			if (codeElement->bracket == LEFTSMALLBRACKET) {
				skipToIndex = codeElement->nextBracketIndex;
			} else if (codeElement->bracket == RIGHTSMALLBRACKET) {
				CodeElement * preBracket = codeLine->codeElements[codeElement->preBracketIndex];
				if (codeElement->preBracketIndex - 1 >= 0 && (codeLine->codeElements[codeElement->preBracketIndex - 1]->type == NAME || codeLine->codeElements[codeElement->preBracketIndex - 1]->type == CHILDNAME)) { //FunctionCall
					functionCall = new FunctionCall();
					if (codeLine->codeElements[codeElement->preBracketIndex - 1]->type == CHILDNAME) {
						for (int ii = codeElement->preBracketIndex - 1; ii >= 0; ii--) {
							if (codeLine->codeElements[ii]->type == CHILDNAME) {
								continue;
							} else if (codeLine->codeElements[ii]->type == NAME) {
								functionCall->functionNameChain = codeLine->codeElements[ii];
								break;
							}
						}
					} else {
						functionCall->functionName = codeLine->codeElements[codeElement->preBracketIndex - 1]->variable_name;
					}

					if (codeElement->preBracketIndex - 2 >= 0 && codeLine->codeElements[codeElement->preBracketIndex - 2]->type == KEYWORD) {
						if (codeLine->codeElements[codeElement->preBracketIndex - 2]->keyword == keyWords->string_new) {
							functionCall->isNew = true;
						}
					}
					int lastDelimiterindex = preBracket->preBracketIndex + 1;

					int backetIndex = 0;

					for (int ii = lastDelimiterindex; ii < preBracket->nextBracketIndex; ii++) {

						CodeElement * innerElement = codeLine->codeElements[ii];
						if (innerElement->type == BRACKET && innerElement->bracket == LEFTSMALLBRACKET) {
							backetIndex++;
						} else if (innerElement->type == BRACKET && innerElement->bracket == RIGHTSMALLBRACKET) {
							backetIndex--;
						}
						if (innerElement->type == DELIMITER && innerElement->isResolvedDelimiter == false && backetIndex == 0) {

							Executable* innerExecutable = analyzeCodeLine(codeLine, lastDelimiterindex, ii, context);
							functionCall->variables->push(innerExecutable);

							lastDelimiterindex = ii + 1;
							innerElement->isResolvedDelimiter = true;
						} else if (ii == preBracket->nextBracketIndex - 1) {
							Executable* innerExecutable = analyzeCodeLine(codeLine, lastDelimiterindex, ii + 1, context);
							functionCall->variables->push(innerExecutable);
						}
					}

					if (expression == NULL) {
						expression = new Expression();
					}

					expression->executables->push(functionCall);

				} //FunctionCall
				else {
					if (executableBlock == NULL) {
						executableBlock = new ExecutableBlock();
					}

					int backetIndex = 0;
					int lastDelimiterindex = preBracket->preBracketIndex + 1;
					for (int ii = lastDelimiterindex; ii < preBracket->nextBracketIndex; ii++) {
						CodeElement * innerElement = codeLine->codeElements[ii];
						if (innerElement->type == BRACKET && innerElement->bracket == LEFTSMALLBRACKET) {
							backetIndex++;
						} else if (innerElement->type == BRACKET && innerElement->bracket == RIGHTSMALLBRACKET) {
							backetIndex--;
						}
						if (innerElement->type == DELIMITER && innerElement->isResolvedDelimiter == false) {

							Executable* innerExecutable = analyzeCodeLine(codeLine, lastDelimiterindex, ii, context);
							executableBlock->executables->push(innerExecutable);

							lastDelimiterindex = ii + 1;
							innerElement->isResolvedDelimiter = true;
						} else if (ii == preBracket->nextBracketIndex - 1) {
							Executable* innerExecutable = analyzeCodeLine(codeLine, lastDelimiterindex, ii + 1, context);
							executableBlock->executables->push(innerExecutable);
						}
					}

					if (expression == NULL) {
						expression = new Expression();
					}
					if (executableBlock->executables->length == 1) {
						Executable* innerExecutable = executableBlock->executables->find(0);
						if (innerExecutable->type == EXPRESSION) {
							expression->executables->push(innerExecutable);
						}
					} else {
						expression->executables->push(executableBlock);
					}
					executableBlock = NULL;
				}

			} else if (codeElement->bracket == LEFTBIGBRACKET) {
				if (context->structuredExecutable->executableBlock == NULL) {
					context->structuredExecutable->executableBlock = new ExecutableBlock();
					;

					context->structuredLevel = 1;

				} else {
					//report error
				}

			} else if (codeElement->bracket == RIGHTBIGBRACKET) {
				context->structuredLevel = -1;

			}

		} //BRACKET

		else if (codeElement->type == CODEOPERATOR) {
			if (expression == NULL) {
				expression = new Expression();
			}

			Operator * code_operator = new Operator();
			code_operator->code_operator = codeElement->code_operator;
			code_operator->code_operator2 = codeElement->code_operator2;

			expression->executables->push(code_operator);

		}

		else if (codeElement->type == NAME || codeElement->type == CODE_NUMBER || codeElement->type == CODE_STRING || codeElement->type == CODE_JSON) {
			if (i + 1 < end && codeLine->codeElements[i + 1]->type == BRACKET) {
				if (codeLine->codeElements[i + 1]->bracket == LEFTSMALLBRACKET) {
					continue;
				}
			}

			metaExecutable = new MetaExecutable();

			metaExecutable->codeElement = codeElement;

			if (end - from <= 1) {
				//executable = metaExecutable;
			} else {
				if (expression == NULL) {
					expression = new Expression();
				}

				expression->executables->push(metaExecutable);

			}
		} //CODEOPERATOR
		else if (codeElement->type == DELIMITER) {
			if (i + 1 >= end || expression == NULL) {
				continue;
			}
			if (executableBlock == NULL) {
				executableBlock = new ExecutableBlock();
			}
			executableBlock->executables->push(expression);
			expression = NULL;
		} //DELIMITER
	}
	//end for

	Executable * executable = NULL;
	if (executableBlock != NULL && expression != NULL) {
		executableBlock->executables->push(expression);
	}

	if (functionDefinition != NULL) {
		if (!(*functionCall->functionName == *functionDefinition->functionName)) {
			//report error;
		}
		functionDefinition->variables = functionCall->variables;
		executable = functionDefinition;
	} else if (forInBlock != NULL) {
		if (expression->executables->length == 2) {
			if (((Executable*) expression->executables->find(0))->type == EXCUTEABLEBLOCK) {
				ExecutableBlock* executableBlock = (ExecutableBlock*) expression->executables->find(0);
				if (executableBlock->executables->length == 2) {
					MetaExecutable* keyNameExecutable = (MetaExecutable*) executableBlock->executables->find(0);
					MetaExecutable* valueNameExecutable = (MetaExecutable*) executableBlock->executables->find(1);

					forInBlock->keyName = keyNameExecutable->codeElement->variable_name;
					forInBlock->valueName = valueNameExecutable->codeElement->variable_name;
				}
			} else if (((Executable*) expression->executables->find(0))->type == META) {
				MetaExecutable* valueNameExecutable = (MetaExecutable*) expression->executables->find(0);
				forInBlock->keyName = NULL;
				forInBlock->valueName = valueNameExecutable->codeElement->variable_name;
			}
			if (((Executable*) expression->executables->find(1))->type == META) {
				MetaExecutable* iteratorNameExecutable = (MetaExecutable*) expression->executables->find(1);
				forInBlock->iteratorName = iteratorNameExecutable->codeElement->variable_name;
			}
		}
		executable = forInBlock;
		int i = 1 + 1;
	} else if (forBlock != NULL) {
		if (executableBlock != NULL) {
			if (executableBlock->executables->length >= 3) {
				forBlock->pre_executable = executableBlock->executables->find(0);
				forBlock->conditions->push(executableBlock->executables->find(1));
				forBlock->last_executable = executableBlock->executables->find(2);

				executable = forBlock;
			} else {
				//report error

			}
		}

		executable = forBlock;
	} else if (ifBlock != NULL) {
		if (expression != NULL) {
			ifBlock->condition = expression;
		}
		executable = ifBlock;
	} else if (functionReturn != NULL) {
		if (expression != NULL) {
			functionReturn->variable = expression->executables->find(0);
			executable = functionReturn;
		}
	} else if (executable == NULL) {
		if (end - from <= 1) {
			executable = metaExecutable;
		} else {
			executable = expression;
		}
	}

	return executable;

}

