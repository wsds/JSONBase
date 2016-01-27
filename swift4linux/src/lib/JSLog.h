#ifndef JSLOG_H
#define JSLOG_H

#include "../data_core/JSString.h"
#include "../data_mmap/DMString.h"
#include "../data_core/JSON.h"
#include "fcntl.h"
#include <sys/stat.h>
#include <errno.h>

void JSLog(JSObject * object);

void JSLog(JSString & jsString);
void JSLog(DMString * dm_string);
void JSLog(char * message);
void JSLog(const char * message);
void JSLog(int number);
void JSLog(JSON * json);

extern bool flag_MyLog;
extern void (*log2MyLog)(JSString & jsString);

extern bool flag_cout;
extern void (*log2cout)(JSString & jsString);

extern bool flag_Java;
extern void (*log2Java)(JSString & jsString);

extern bool flag_Android;
extern void (*log2Android)(JSString & jsString);

extern bool flag_TraceFile;
void log2TraceFile(JSString & jsString);

#endif /* JSLOG_H */

