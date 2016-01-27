#ifndef DMLOG_H
#define DMLOG_H

#include <iostream>
#include "../data_mmap/DMJSONHelper.h"

void DMLog(MemorySpace * object);

void DMLog(DMString * dm_string);
void DMLog(DMJSON * dm_json);
void DMLog(DMInt * dm_int);
void DMLog(const char * message);
void DMLog(char * message, int length);

void clearCurrentColor();


#endif /* DMLOG_H */

