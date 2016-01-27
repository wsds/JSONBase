#include "DMNumber.h"

DMInt * newDMInt(){
	DMInt * dm_int = (DMInt *) map_malloc(0);//todo get the pointer only but malloc the real space.
	dm_int->type=0xd0;
	dm_int->bit=32;
	return dm_int;
}

void freeDMInt(DMInt * dm_int){
	map_free((MemorySpace *)dm_int);
}
