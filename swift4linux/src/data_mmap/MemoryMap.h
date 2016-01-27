#ifndef MEMORYMAP_H
#define MEMORYMAP_H

#ifndef NULL
#define NULL 0
#endif /* NULL */

#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "fcntl.h"
#include <string.h>
#include "DMSpaceType.h"

//#include "../lib/WebSocketLog.h"

//#include "../storage/Storage.h"

class MemorySpace {
public:
	char type;
	char flag1;
	char flag2;
	char flag3;

	void * pointer;
	int length;
};

#define PAGESIZE64K (64*1024)
class MemoryPage64k {
public:
	int page_index;

	int used;
	int freed;

	int max_space_size;
	int space_length;

	int chain_used; //when free the page whose chain_used>0, put it into the used pool instead of the free pool as normal.

	int state; //0:freed, 600060006:used , -1:freeing , 1006001: swapping

	MemorySpace * spaces_container; //note: this is not the list of managing spaces, but the malloced memory space where the indexes of the managing spaces is stored.
	int new_max_space_size;
};

void reverse_memcpy1(void * destination, void * source, int count);

class MemeoryMap {
public:
	void initialize();

	void firstload();
	void reload();
	void finish();
	void resize();
	//---------------------page---------------------------
	int pageFD;
	int max_page_size; //max_page_size=used_page_size+free_page_size
	MemoryPage64k * pages;

	int used_page_size;
	int * used_page_pool;
	int page_power_space[17];

	int free_page_size;
	int unsorted_page_size;
	int * free_page_pool;

	void resizePage(int new_max_page_size);

	int getPageChain(int chain_length);
	MemoryPage64k * findAvailablePage(int size, MemorySpace * space);
	void freePage(MemoryPage64k * page);
	void sortUsedPagePool();
	void sortFreePagePool();
	void switchPage(MemoryPage64k * page);
	void refreshPage(MemoryPage64k * page);

	void enhancePageSpaces(MemoryPage64k * page);
	//---------------------page---------------------------

	//---------------------space---------------------------
	int spaceFD;
	int max_space_size;
	MemorySpace * spaces;

	int used_space_size;
	int * used_space_pool;
	int free_space_size;
	int * free_space_pool;
	void resizeSpace(int new_max_space_size);
	MemorySpace * findAvailableSpace();
	void freeSpace(MemorySpace * space);
	void transferMemeorySpace(MemorySpace * space);

	//---------------------space---------------------------

	int memeoryFD;
	int max_size;
	int threshold;
	int used_size;
	int freeing_size;
	char * memory_base;
	void resizeMemory(int new_max_size);

	char * swap_page_base;
	int used_swap_end;
	MemorySpace swap_spaces[16];
	int swap_space_index;
	MemorySpace * getAvailableSwapSpace(int size);
	MemorySpace * swapUsedSpace(MemorySpace * space);
	void freeUsedSwapSpace(MemorySpace * space);
	//================API======================0
	MemorySpace * malloc(int size);
	void free(MemorySpace * space);

	bool exchange(MemorySpace * left, MemorySpace * right);
	//todo need to resolve when the calling of the APIs concurrently
	//================API======================

	int is_initialized;
	bool is_ready;

	char * base;

	MemoryPage64k * switching_page;

	int freeing_space_size;
	bool is_freeing;
	MemorySpace * freeing_spaces[16];

	void * static_spaces[16];

};

class TraverseCallback {
public:
	TraverseCallback();
	void (*onGetData)(MemorySpace * data_space);
};

static char momory_folder[64];
static int momory_folder_path_length;

static MemeoryMap * memeoryMap;

MemeoryMap * reloadMemeoryMap();

void setMomoryFolderPath(char * path, int length);
char * getFilePath(char * file_name);

MemorySpace * map_malloc(int size);
void map_free(MemorySpace * space);
bool map_exchange(MemorySpace * left, MemorySpace * right);

bool compareMemeorySpace(MemorySpace * left, MemorySpace * right);
void** get_static_spaces();

int getLength1(char *char_string);

#endif /* MEMORYMAP_H */

