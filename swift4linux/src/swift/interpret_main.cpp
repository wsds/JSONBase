#include "interpret_main.h"

Interpreter::Interpreter() {
	this->is_initialized = false;
	this->initialize();
}

Interpreter * Interpreter::instance = NULL;
Interpreter * Interpreter::getInstance() {
	if (instance == NULL) {
		instance = new Interpreter();
	}
	return instance;
}

bool Interpreter::initialize() {
	if (this->is_initialized == true) {
		return true;
	}
	this->is_initialized = true;

	this->contextIDStart = 0;

	this->contextMap = new HashTable2();
	this->contextMap->initialize();

	this->executeContextMap = new HashTable2();
	this->executeContextMap->initialize();

	return true;
}

int Interpreter::newContext() {
	this->contextIDStart++;

	Context * context = new Context();
	ExecuteContext * executeContext = new ExecuteContext();

	int contextID = this->contextIDStart;

	this->contextMap->set(contextID, context);
	this->executeContextMap->set(contextID, executeContext);

	importModules(executeContext);
	return contextID;
}
void Interpreter::inputCodeLine(int contextID, JSString * codeLineStr) {
	if (codeLineStr->length > 0) {
//		JSLog(codeLineStr);
		Context * context = this->contextMap->get(contextID);
		CodeLine* codeLine = resolveCodeLine(codeLineStr);
		Executable* executable = analyzeCodeLine(codeLine, context);

//		JSON * json = stringify(context->rootExecutableBlock);
//		JSLog(json);
	} else {
//		JSLog("");
	}
}

void Interpreter::inputCode(int contextID, JSString * code) {
	Context * context = this->contextMap->get(contextID);
	if (context == NULL) {
		JSLog("swift error: invalid context");
		return;
	}
	char * lastLine = code->char_string;
	char localChar;
//	char next_localChar;
	int BRACKET_Counter = 0;
	int SINGLEQUOTE_Counter = 0;
	int DOUBLEQUOTES_Counter = 0;
	JSString * codeLineStr = new JSString(0);

	for (int i = 0; i < code->length; i++) {
		localChar = code->char_string[i];
//		next_localChar = code->char_string[i + 1];
		if (localChar == SINGLEQUOTE) {
			SINGLEQUOTE_Counter = (SINGLEQUOTE_Counter + 1) % 2;
		} else if (localChar == DOUBLEQUOTES) {
			DOUBLEQUOTES_Counter = (DOUBLEQUOTES_Counter + 1) % 2;
		} else if (localChar == LEFTBRACKET) {
			BRACKET_Counter++;
		} else if (localChar == RIGHTBRACKET) {
			BRACKET_Counter--;
		}

		if (SINGLEQUOTE_Counter != 0 || DOUBLEQUOTES_Counter != 0 || BRACKET_Counter != 0) {
			continue;
		}

		if (localChar == STREND) {
			codeLineStr->char_string = lastLine;
			char * newLine = code->char_string + i + 1;
			codeLineStr->length = newLine - lastLine;
			lastLine = newLine;
			this->inputCodeLine(contextID, codeLineStr);
		} else if (localChar == BR) {
			codeLineStr->char_string = lastLine;
			char * newLine = code->char_string + i + 1;
			codeLineStr->length = newLine - lastLine - 1;
			lastLine = newLine;
			this->inputCodeLine(contextID, codeLineStr);
		} else if (i == code->length - 1) {
			codeLineStr->char_string = lastLine;
			char * newLine = code->char_string + i + 1;
			codeLineStr->length = newLine - lastLine;
			lastLine = newLine;
			this->inputCodeLine(contextID, codeLineStr);
		}
	}

	JSON * json = stringify(context->rootExecutableBlock);
	JSLog(json);
}

void Interpreter::importCode(int contextID, JSString * code) {

}
void Interpreter::runCode(int contextID, JSString * code) {

}

void Interpreter::run(int contextID) {
	Context * context = this->contextMap->get(contextID);
	if (context == NULL) {
		JSLog("swift error: invalid context");
		return;
	}
	ExecuteContext * executeContext = this->executeContextMap->get(contextID);
	execute(context->rootExecutableBlock, executeContext);
}
void Interpreter::clear(int contextID) {
	Context * context = this->contextMap->get(contextID);
	if (context == NULL) {
		JSLog("swift error: invalid context");
		return;
	}
	context->rootExecutableBlock->executables->length = 0;
}

void Interpreter::logClosure(int contextID) {
	ExecuteContext * executeContext = this->executeContextMap->get(contextID);
	if (executeContext == NULL) {
		JSLog("swift error: invalid context");
		return;
	}
	JSLog(executeContext->rootClosure->variables);
}
void interpret_main() {

//	Import* import = Import::getInstance();
//	import->regiditFunctions();
}

//void resolveCodeLine(char* line) {
//	JSObject* result = NULL;
//
//	CodeLine* codeLine = new CodeLine();
//	int string_length = strlen(line);
//
//	int resolveResult = resolveElement(line, string_length, codeLine);
//
//	resolveOperators(codeLine);
//	resolveBracket(codeLine);
//	resolveDot(codeLine);
//
//	Executable * executable = analyzeCodeLine(codeLine, 0, codeLine->codeElementList->length);
//
////	DEBUGExecutable * iDEBUGExecutable = debugExecutable(executable);
//
//	//currentExecutableBlock->isHolded = true
//	if (currentExecutableBlockHoldingStatus == STARTBLOCK) {
//		currentExecutableBlockHoldingStatus = NONE;
//		return;
//	}
//	if (executableBlocksIndex <= 0) {
//
//		if (executable == NULL) {
//			if (currentExecutableBlockHoldingStatus == ENDBLOCK) {
//				ExecutableBlock* executableBlock = executableBlocks[executableBlocksIndex];
//				if (executableBlock != NULL && executableBlock->holder != NULL) {
//					executable = executableBlock->holder;
//				}
//			}
//		}
//		result = excute(executable);
//	} else {
//		if (executable == NULL) {
//			if (currentExecutableBlockHoldingStatus == ENDBLOCK) {
//				ExecutableBlock* executableBlock = executableBlocks[executableBlocksIndex];
//				if (executableBlock != NULL && executableBlock->holder != NULL) {
//					executable = executableBlock->holder;
//				}
//			}
//		}
//		if (executable != NULL) {
//			ExecutableBlock* executableBlock = executableBlocks[executableBlocksIndex - 1];
//			executableBlock->executables[executableBlock->executable_index] = executable;
//			executableBlock->executable_index++;
//		}
//	}
//
//	if (result != NULL) {
////		log(result);
//		JSLog(result);
//	}
//}
