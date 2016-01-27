#ifndef DMNUMBER_H
#define DMNUMBER_H

#ifndef NULL
#define NULL 0
#endif /* NULL */

#include "MemoryMap.h"

class DMInt {
public:
	char type; //0xd0
	char bit; //32
	char flag2;
	char flag3;

	int number;

	int empty;
};

class DMDouble {
public:
	char type; //0xd0
	char bit; //64
	char flag2;
	char flag3;

	double number;
};

class DMBigNumber {
public:
	MemorySpace * self;

	MemorySpace * number_space;
	int bit; //>>64
	int range; //2^range?
	int precision; //0.5^precision?
};

DMInt * newDMInt();
void freeDMInt(DMInt * dm_int);

DMDouble * newDMDouble();
void freeDMDouble(DMDouble * dm_int);

#endif /* DMNUMBER_H */

