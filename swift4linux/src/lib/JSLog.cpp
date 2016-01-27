#include "JSLog.h"

bool flag_MyLog = false;
void (*log2MyLog)(JSString & jsString) = NULL;

bool flag_cout = false;
void (*log2cout)(JSString & jsString)= NULL;

bool flag_Java = false;
void (*log2Java)(JSString & jsString)= NULL;

bool flag_Android = false;
void (*log2Android)(JSString & jsString)= NULL;

bool flag_TraceFile = false;

void JSLog(JSObject * object) {
	if (object == NULL) {
		return;
	}
	if (object->type == JSNUMBER) {
		int number = ((JSNumber*) object)->number;
		JSLog(number);
	} else if (object->type == JSSTRING) {
		JSString *js_string = (JSString*) object;
		JSLog(*js_string);
	} else if (object->type == JSJSON) {
		JSON *json = (JSON*) object->number;
		JSLog(json);
	} else if (object->type == EMPTY) {
		JSON *json = (JSON*) object->number;
		JSLog("@EMPTY@");
	}
}

void JSLog(JSString & jsString) {
	JSString * jsString1_p = &jsString;

	if (flag_MyLog == true && log2MyLog != NULL) {
		log2MyLog(jsString);
	}
	if (flag_cout == true && log2cout != NULL) {
		log2cout(jsString);
	}
	if (flag_Java == true && log2Java != NULL) {
		log2Java(jsString);
	}
	if (flag_Android == true && log2Android != NULL) {
		log2Android(jsString);
	}
	if (flag_TraceFile == true) {
		log2TraceFile(jsString);
	}
}
void JSLog(DMString * dm_string) {
	JSString * js_string = new JSString(0);
	js_string->char_string = dm_string->char_string;
	js_string->length = dm_string->max_length;
	JSLog(js_string);
}

void JSLog(char * message) {
	JSString jsString;
	jsString + message;
	JSLog(jsString);
}

void JSLog(JSON * json) {
	if (json == NULL) {
		JSLog("NULL");
		return;
	}
	JSString jsString;
	jsString + stringifyJSON(json);
	JSLog(jsString);
}

void JSLog(const char * message) {
	JSString jsString;
	jsString + (char *) message;
	JSLog(jsString);
}

void JSLog(int number) {
	JSString jsString;
	jsString + number;
	JSLog(jsString);
}

int traceFD = -1;
bool isTraceInitialized = false;
void initTrace() {
	traceFD = open("/alidata/test/test10001.txt", O_CREAT | O_RDWR, 0777);
	if (traceFD >= 0) {
		isTraceInitialized = true;
	}
}

void log2TraceFile(JSString & jsString) {

	if (isTraceInitialized == false) {
		initTrace();
	}

	if (traceFD < 0) {
		return;
	}

	struct stat sb;
	fstat(traceFD, &sb);

	int fileLength = sb.st_size;

	if (fileLength == 0) {
		ftruncate(traceFD, 1);
	} else if (fileLength > 0) {
		ftruncate(traceFD, fileLength + jsString.length);
	} else {
		return;
	}

	char * traceFileBuffer = (char*) mmap(NULL, fileLength + jsString.length, PROT_READ | PROT_WRITE, MAP_SHARED, traceFD, 0);

	if (traceFileBuffer == (void *) -1) {
		int n = errno;
		return;
	}

	ftruncate(traceFD, fileLength + jsString.length);

	memcpy((traceFileBuffer + fileLength), jsString.char_string, jsString.length);

	munmap(traceFileBuffer, jsString.length);

}

