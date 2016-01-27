#include "MemoryMap.h"

MemeoryMap * reloadMemeoryMap() {
	char * file_path = getFilePath((char *) "memoryMap.odb");
	int memeoryMapFD = open(file_path, O_CREAT | O_RDWR, 0777);

	int fileLength = 1024 * 4;

	ftruncate(memeoryMapFD, fileLength);
	char * base = (char *) 0x41000000;
	base = (char*) mmap(base, fileLength, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, memeoryMapFD, 0);
	memeoryMap = (MemeoryMap *) base;

	memeoryMap->initialize();
//	JSLog((int) base);
	return memeoryMap;
}

void setMomoryFolderPath(char * path, int length) {
	momory_folder_path_length = length;
	memset(momory_folder, 0, 64);
	reverse_memcpy1(momory_folder, path, length);
}

char * getFilePath(char * file_name) {
	int file_name_length = getLength1(file_name);
	reverse_memcpy1(momory_folder + momory_folder_path_length, file_name, file_name_length);
	*(momory_folder + momory_folder_path_length + file_name_length) = 0;
	return momory_folder;
}

MemorySpace * map_malloc(int size) {
	if (memeoryMap == NULL) {
		memeoryMap = reloadMemeoryMap();
	}

	MemorySpace * memeorySpace = memeoryMap->malloc(size);
	return memeorySpace;
}

void map_free(MemorySpace * space) {
	if (memeoryMap == NULL) {
		memeoryMap = reloadMemeoryMap();
	}
	memeoryMap->free(space);
}

bool map_exchange(MemorySpace * left, MemorySpace * right) {
	if (memeoryMap == NULL) {
		memeoryMap = reloadMemeoryMap();
	}
	return memeoryMap->exchange(left, right);
}

void** get_static_spaces() {
	if (memeoryMap == NULL) {
		memeoryMap = reloadMemeoryMap();
	}
	return memeoryMap->static_spaces;
}

bool compareMemeorySpace(MemorySpace * left, MemorySpace * right) {

	if (left == right) {
		return true;
	}
	if (left == NULL || right == NULL) {
		return false;
	}
	if (left->length != right->length) {
		return false;
	}

	bool isSame = true;

	char * left_pointer = (char *) left->pointer;
	char * right_pointer = (char *) right->pointer;

	for (int i = 0; i < left->length; i++) {
		if (*left_pointer != *right_pointer) {
			isSame = false;
			break;
		}
		left_pointer++;
		right_pointer++;
	}

	return isSame;
}

int getLength1(char *char_string) {
	char *i = char_string;

	while (*i++) {
	}
	int length = (i - char_string - 1);

	return length;
}

void MemeoryMap::initialize() {

	if (this->is_initialized == 600006) {
		this->reload();
		return;
	}

	if (this->is_initialized != 0) {
		this->is_initialized = 0;
		return;
	}
	this->is_ready = false;
	this->firstload();

	this->is_initialized = 600006;

	this->is_ready = true;
}

void MemeoryMap::reload() {
	this->base = (char *) this;

	this->pageFD = -1;
//	this->pages = NULL;
	this->resizePage(this->max_page_size);

	this->spaceFD = -1;
//	this->spaces = NULL;
	this->resizeSpace(this->max_space_size);

	this->memeoryFD = -1;
//	this->memory_base = NULL;
	this->resizeMemory(this->max_size);
}

void MemeoryMap::firstload() {

	this->max_size = 1024 * 1024 * 16;

	this->pageFD = -1;
	this->max_page_size = this->max_size / PAGESIZE64K - 1;
	this->free_page_size = this->max_page_size;
	this->unsorted_page_size = 0;
	this->used_page_size = 0;
	this->pages = NULL;
	this->resizePage(this->max_page_size);
	for (int i = 0; i < this->free_page_size; i++) {
		*(this->free_page_pool + i) = i;
	}

	this->spaceFD = -1;
	this->max_space_size = 256 * 1024;
	this->spaces = NULL;
	this->used_space_size = 1;
	this->free_space_size = this->max_space_size - 1;
	this->resizeSpace(this->max_space_size);
	for (int i = 0; i < this->free_space_size; i++) {
		*(this->free_space_pool + i) = i;
	}

	this->base = (char *) this;
	this->memeoryFD = -1;
	this->threshold = (int) (this->max_size * 0.8);
	this->used_size = 0;
	this->memory_base = NULL;
	this->resizeMemory(this->max_size);
}

void MemeoryMap::resizePage(int new_max_page_size) {
	this->is_ready = false;
	int page_map_size = this->max_page_size * (sizeof(MemoryPage64k) + 2 * sizeof(int));
	int new_page_map_size = new_max_page_size * (sizeof(MemoryPage64k) + 2 * sizeof(int));

	if (this->pages != NULL && this->pageFD != -1) {
		int closingFD = this->pageFD;
		munmap(this->pages, page_map_size);
		close(closingFD);
	}

	char * file_path = getFilePath((char*) "memoryPageMap.odb");
	this->pageFD = open(file_path, O_CREAT | O_RDWR, 0777);
	ftruncate(this->pageFD, new_page_map_size);

	char * start = (char *) 0x51000000;
	if (this->pages != NULL) {
		start = (char *) this->pages;
	}

	start = (char *) mmap(start, new_page_map_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, this->pageFD, 0);

	if (this->max_page_size != new_max_page_size) {
		this->max_page_size = new_max_page_size;
		//todo move the inner data
	} else {
		this->pages = (MemoryPage64k *) start;
		this->used_page_pool = (int *) (start + this->max_page_size * sizeof(MemoryPage64k));
		this->free_page_pool = (int *) (start + this->max_page_size * (sizeof(MemoryPage64k) + 1 * sizeof(int)));
	}
//	JSLog((int) start);
	this->is_ready = true;
}

void MemeoryMap::resizeSpace(int new_max_space_size) {
	this->is_ready = false;
	int space_map_size = this->max_space_size * (sizeof(MemorySpace) + 2 * sizeof(int));
	int new_space_map_size = new_max_space_size * (sizeof(MemorySpace) + 2 * sizeof(int));

	if (this->spaces != NULL && this->spaceFD != -1) {
		int closingFD = this->spaceFD;
		munmap(this->spaces, space_map_size);
		close(closingFD);
	}

	char * file_path = getFilePath((char *) "memorySpaceMap.odb");
	this->spaceFD = open(file_path, O_CREAT | O_RDWR, 0777);
	ftruncate(this->spaceFD, new_space_map_size);

	char * start = (char *) 0x52000000;
	if (this->spaces != NULL) {
		start = (char *) this->spaces;
	}
	start = (char *) mmap(start, new_space_map_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, this->spaceFD, 0);

	if (this->max_space_size != new_max_space_size) {
		this->max_page_size = new_max_space_size;
		//todo move the inner data
	} else {
		this->spaces = (MemorySpace *) start;
		this->used_space_pool = (int *) (start + this->max_space_size * sizeof(MemorySpace));
		this->free_space_pool = (int *) (start + this->max_space_size * (sizeof(MemorySpace) + 1 * sizeof(int)));
	}
//	JSLog((int) start);
	this->is_ready = true;
}

void MemeoryMap::resizeMemory(int new_max_size) {
	this->is_ready = false;

	if (this->memory_base != NULL && this->memeoryFD != -1) {
		int closingFD = this->memeoryFD;
		munmap(this->memory_base, this->max_size);
		close(closingFD);
	}

	char * file_path = getFilePath((char*) "memory.odb");
	this->memeoryFD = open(file_path, O_CREAT | O_RDWR, 0777);
	ftruncate(memeoryFD, new_max_size);

	char * start = (char *) 0x42000000;
	if (this->memory_base != NULL) {
		start = this->memory_base;
	}
	start = (char*) mmap(start, new_max_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, this->memeoryFD, 0);

	if (this->max_size != new_max_size) {
		this->max_size = new_max_size - PAGESIZE64K;
	} else {
		this->memory_base = start;
		this->swap_page_base = start + new_max_size - PAGESIZE64K;
	}
//	JSLog((int) start);
	this->is_ready = true;
}

void MemeoryMap::finish() {
	if (this->memory_base != NULL && this->memeoryFD != -1) {
		int closingFD = this->memeoryFD;
		munmap(this->base, this->max_size);
		close(closingFD);
	}
}

void MemeoryMap::switchPage(MemoryPage64k * page) {
	if (page->state != 600060006) {
		return;
	}

	if (page->spaces_container == NULL) {
		return;
	}

	int * space_indexes = NULL;
	MemorySpace * temp_spaces_container = NULL;
	int spaces_page_index = ((char *) page->spaces_container->pointer - this->memory_base) / PAGESIZE64K;
	if (spaces_page_index == page->page_index) {

		temp_spaces_container = this->getAvailableSwapSpace(page->spaces_container->length);
		reverse_memcpy1(temp_spaces_container->pointer, page->spaces_container->pointer, page->spaces_container->length);
		space_indexes = ((int *) temp_spaces_container->pointer);

	} else {
		space_indexes = ((int *) page->spaces_container->pointer);
	}

	int space_index;
	MemorySpace * space;

	this->switching_page = page;
	for (int i = 0; i < page->space_length; i++) {
		space_index = *(space_indexes + i);
		if (space_index <= 0) {
			continue;
		}
		space = this->spaces + space_index;
		if (space == page->spaces_container) {
			continue;
		}
		if (space->length > 0) {
			this->transferMemeorySpace(space);
		} else {
			this->freeSpace(space);
		}

		*(space_indexes + i) = -1;
	}
	this->switching_page = NULL;

	if (spaces_page_index == page->page_index) {
		this->freeUsedSwapSpace(temp_spaces_container);
	}

	MemorySpace * old_spaces_container = NULL;
	if (spaces_page_index == page->page_index) {
		this->freeUsedSwapSpace(temp_spaces_container);
		memset(page->spaces_container->pointer, 0, page->spaces_container->length);
		this->freeSpace(page->spaces_container);
	} else {
		old_spaces_container = page->spaces_container;
	}
	page->spaces_container = NULL;

	this->freePage(page);

	if (old_spaces_container != NULL) {
		this->free(page->spaces_container);
	}

}

void MemeoryMap::refreshPage(MemoryPage64k * page) {
	if (page->spaces_container == NULL) {
		return;
	}

	int * space_indexes = ((int *) page->spaces_container->pointer);
	int space_index;
	MemorySpace * space;

	int j = 0;
	for (int i = 0; i < page->space_length; i++) {
		space_index = *(space_indexes + i);
		if (space_index <= 0) {
			continue;
		}
		space = this->spaces + space_index;

		if (space->length > 0) {
			*(space_indexes + j) = space_index;
			j++;
		} else {
			this->freeSpace(space);
		}
	}

	for (int i = j; i < page->space_length; i++) {
		*(space_indexes + i) = 0;
	}

	page->space_length = j;

}

MemorySpace * MemeoryMap::findAvailableSpace() {
	MemorySpace * space;
	int index = this->free_space_pool[this->used_space_size];
	space = this->spaces + index;
	space->pointer = NULL;
	space->length = 0;

	this->free_space_pool[this->used_space_size] = -1;
	this->used_space_pool[this->used_space_size] = index;

	this->used_space_size++;
	this->free_space_size--;

	//todo resizeSpace the spaces pool when the used_space_size>max_space_size
	return space;
}

void MemeoryMap::freeSpace(MemorySpace * space) {
	if (space == NULL) {
		return;
	}
	space->length = 0;
	space->pointer = NULL;
	int space_index = space - this->spaces;
	if (space_index == 0) {
		return;
	}
	this->free_space_pool[this->used_space_size - 1] = space_index;
	this->used_space_pool[this->used_space_size - 1] = -1;

	this->used_space_size--;
	this->free_space_size++;
}

void reverse_memcpy1(void * destination, void * source, int count) {
	if (destination == NULL || source == NULL || count <= 0) {
		return;
	}

	char * destination_char = (char *) destination;
	char * source_char = (char *) source;
	for (int i = count - 1; i >= 0; i--) {
		*(destination_char + i) = *(source_char + i);
	}
}

int MemeoryMap::getPageChain(int chain_length) {

	int used = this->used_page_size;
	int size = this->max_page_size;
	int * pool = this->free_page_pool;

	int macthed_chain_length = 1;
	int last_index = -100;
	int index = -100;
	int position = -100;

	if (this->unsorted_page_size > 0) {
		this->sortFreePagePool();
		this->unsorted_page_size = 0;
	}

	for (int i = used; i < size; i++) {
		index = pool[i];

		if (last_index + 1 != index) {
			macthed_chain_length = 1;
		} else {
			macthed_chain_length++;
		}

		if (macthed_chain_length >= chain_length) {
			position = i + 1;
			break;
		}

		last_index = index;
		index = -100;
	}

	if (index >= 0) {
		index = index - chain_length + 1;
		reverse_memcpy1(pool + used + chain_length, pool + used, (position - chain_length - used) * sizeof(int));

		memset(pool + used, -1, chain_length * sizeof(int));
	}
	MemoryPage64k * page = NULL;
	for (int i = 0; i < chain_length; i++) {
		this->used_page_pool[this->used_page_size + i] = index + i;
		page = this->pages + (index + i);
		page->page_index = (index + i);
		page->state = 600060006;
	}
	this->used_page_size = this->used_page_size + chain_length;
	this->free_page_size = this->free_page_size - chain_length;

	return index;

	//todo resizePage the pages pool when the index get -100
}

int log2i(int x) {
	//need test.
	x = x | 1;
	int n = 31;
	if (x & 0xffff0000) {
		n -= 16;
		x >>= 16;
	}
	if (x & 0xff00) {
		n -= 8;
		x >>= 8;
	}
	if (x & 0xf0) {
		n -= 4;
		x >>= 4;
	}
	if (x & 0xc) {
		n -= 2;
		x >>= 2;
	}
	if (x & 0x2) {
		n--;
	}

	return 31 - n;
}

MemoryPage64k * MemeoryMap::findAvailablePage(int size, MemorySpace * space) {

	MemoryPage64k * page = NULL;
	int page_index = -1;
	int chain_length = 1;

	if (size > PAGESIZE64K) {

		chain_length = size / PAGESIZE64K + 1;
		if (size % PAGESIZE64K == 0) {
			chain_length--;
		}

		page_index = this->getPageChain(chain_length);

		page = this->pages + page_index;
	} else {
		int power = log2i(size) + 1;
		if (power < 5) {
			power = 5;
		}
		int page_power_index = this->page_power_space[power];
		MemoryPage64k * used_page = NULL;
		int used_page_index = 0;
		bool get_next = false;
		for (int i = page_power_index; i < this->used_page_size; i++) {
			used_page_index = this->used_page_pool[i];
			used_page = this->pages + used_page_index;
			if (used_page->used + size <= PAGESIZE64K) {
				page_index = used_page_index;
				page = used_page;
				page_power_index = i;

				if (page != this->switching_page || get_next == true) {
					break;
				} else {
					page = NULL;
					get_next = true;
				}

			}
		}

		if (page == NULL) {
			page_index = this->getPageChain(1);
			page = this->pages + page_index;
			page_power_index = this->used_page_size - 1;
		}

		if (this->page_power_space[power] != page_power_index) {
			for (int j = 16; j >= power; j--) {
				if (this->page_power_space[j] < page_power_index) {
					this->page_power_space[j] = page_power_index;
				}
			}
		}
	}

	space->length = size;
	space->pointer = this->memory_base + (PAGESIZE64K * (int) (page - this->pages)) + page->used;

	MemoryPage64k * chain_page;
	for (int i = 0; i < chain_length - 1; i++) {
		chain_page = page + i;
		chain_page->used = PAGESIZE64K;
	}
	if (size % PAGESIZE64K != 0) {
		chain_page = page + chain_length - 1;
		chain_page->used = chain_page->used + size % PAGESIZE64K;
		if (chain_length > 1) {
			chain_page->chain_used = chain_page->used;
		}
	}

	if (page_index != -1 && size < PAGESIZE64K && page->spaces_container == NULL) {
		page->max_space_size = 64;
		page->spaces_container = getAvailableSwapSpace(page->max_space_size * sizeof(int));
	}

	this->sortUsedPagePool();

	return page;
}

MemorySpace * MemeoryMap::getAvailableSwapSpace(int size) {
	MemorySpace * space = &(this->swap_spaces[this->swap_space_index]);
	this->swap_space_index = (this->swap_space_index + 1) % 16;
	space->length = size;
	if (this->used_swap_end + size < PAGESIZE64K) {
		space->pointer = this->swap_page_base + this->used_swap_end;
		this->used_swap_end = this->used_swap_end + size;
	} else {
		space->pointer = this->swap_page_base;
		this->used_swap_end = size;
	}

	return space;
}

MemorySpace * MemeoryMap::swapUsedSpace(MemorySpace * space) {

	MemorySpace * new_space = this->malloc(space->length);
	reverse_memcpy1(new_space->pointer, space->pointer, space->length);
	memset(space->pointer, 0, space->length);

	return new_space;
}

void MemeoryMap::freeUsedSwapSpace(MemorySpace * space) {

	memset(space->pointer, 0, space->length);
}

void MemeoryMap::sortUsedPagePool() { //bubble sort
	int i, j, n, flag = 1;
	int temp;
	int index_left, index_right = 0;

	n = this->used_page_size;
	for (i = 0; i < n - 1 && flag == 1; ++i) {
		flag = 0;
		for (j = 0; j < n - i - 1; ++j) {
			index_left = *(this->used_page_pool + j);
			index_right = *(this->used_page_pool + j + 1);

			if ((this->pages + index_left)->used < (this->pages + index_right)->used) {
				*(this->used_page_pool + j) = index_right;
				*(this->used_page_pool + j + 1) = index_left;
				flag = 1;
			}
		}
	}
}

void MemeoryMap::sortFreePagePool() {
	int i, j, n, flag = 1;
	int temp;
	int index_left, index_right = 0;

	n = this->unsorted_page_size;
	for (i = 0; i < this->unsorted_page_size - 1 && flag == 1; ++i) {
		flag = 0;
		for (j = this->used_page_size; j < this->free_page_size - i - 1; ++j) {
			index_left = *(this->free_page_pool + j);
			index_right = *(this->free_page_pool + j + 1);

			if (index_left > index_right) {
				*(this->free_page_pool + j) = index_right;
				*(this->free_page_pool + j + 1) = index_left;
				flag = 1;
			}
		}
	}
}

void MemeoryMap::freePage(MemoryPage64k * page) {

	if (page->state != 600060006) {
		return;
	}
	page->state = -1000;

	int old_page_index = page->page_index;
	memset(page, 0, sizeof(MemoryPage64k));

	this->free_page_pool[this->used_page_size - 1] = old_page_index;

	bool need_move = false;
	int used_page_pool_index = 0;
	for (int i = 0; i < this->used_page_size; ++i) {
		if (need_move == true) {
			this->used_page_pool[i - 1] = this->used_page_pool[i];
		}
		if (this->used_page_pool[i] == old_page_index) {
			used_page_pool_index = i;
			need_move = true;
		}
	}

	this->used_page_pool[this->used_page_size - 1] = 0;

	if (need_move == true) {
		for (int j = 16; j >= 5; j--) {
			if (this->page_power_space[j] > used_page_pool_index) {
				this->page_power_space[j]--;
			}
		}
	}

	this->used_page_size--;
	this->free_page_size++;

	this->unsorted_page_size++;

}

void MemeoryMap::transferMemeorySpace(MemorySpace * space) {

	void * old_pointer = space->pointer;
	MemoryPage64k * page = this->findAvailablePage(space->length, space);

	reverse_memcpy1(space->pointer, old_pointer, space->length);
	memset(old_pointer, 0, space->length);

	if (page->spaces_container != NULL) {
		if (page->space_length + 1 > page->max_space_size) {
			this->enhancePageSpaces(page);
		}

		int * space_indexes = ((int *) page->spaces_container->pointer);
		*(space_indexes + page->space_length) = (int) (space - this->spaces);
		page->space_length++;
	}

	if (page->spaces_container->pointer >= this->swap_page_base) {
		if (page->state != 1006001) {
			page->state = 1006001;
			page->spaces_container = this->swapUsedSpace(page->spaces_container);
			page->state = 600060006;
		}
	}
}

void MemeoryMap::enhancePageSpaces(MemoryPage64k * page) {

	if (this->pages == NULL) {
		return;
	}

	this->refreshPage(page);

	if (page->space_length + 1 > page->max_space_size) {
		MemorySpace * old_spaces = page->spaces_container;

		page->max_space_size = page->max_space_size * 2;
		page->spaces_container = getAvailableSwapSpace(page->max_space_size * sizeof(int));

		reverse_memcpy1(page->spaces_container->pointer, old_spaces->pointer, old_spaces->length);
		this->free(old_spaces); //todo free the old space in the end
	}
}
MemorySpace * MemeoryMap::malloc(int size) {

	MemorySpace * space = this->findAvailableSpace();
	if (size == 0) {
		return space;
	}

	MemoryPage64k * page = this->findAvailablePage(size, space);

	if (page->spaces_container != NULL) {
		if (page->space_length + 1 > page->max_space_size) {
			this->enhancePageSpaces(page);
		}

		int * space_indexes = ((int *) page->spaces_container->pointer);
		*(space_indexes + page->space_length) = (int) (space - this->spaces);
		page->space_length++;
	}

	this->used_size = this->used_size + size;
	//todo resizeMemory the memeory  when the used_size>threshold

	if (page->spaces_container->pointer >= this->swap_page_base) {
		if (page->state != 1006001) {
			page->state = 1006001;
			page->spaces_container = this->swapUsedSpace(page->spaces_container);
			page->state = 600060006;
		}
	}
	return space;

}

void MemeoryMap::free(MemorySpace * space) {

	if (this->is_freeing == true) {
		this->freeing_spaces[this->freeing_space_size] = space;
		this->freeing_space_size++;
		return;
	}

	if (space->length == 0 || space->type == TYPE_NUMBER) {
		this->freeSpace(space);
		return;
	}

	this->is_freeing = true;

	//todo check whether the space is in the scope of the maintaining spaces
	int page_index = ((char *) space->pointer - this->memory_base) / PAGESIZE64K;
	int page_offset = (char *) space->pointer - (this->memory_base + PAGESIZE64K * page_index);
	MemoryPage64k * page = this->pages + page_index;

	int chain_length = space->length / PAGESIZE64K + 1;
	MemoryPage64k * chain_page;
	for (int i = 0; i < chain_length - 1; i++) {
		chain_page = page + i;
		this->freePage(chain_page);
	}

	if (space->length % PAGESIZE64K != 0) {
		chain_page = page + chain_length - 1;
		if (page->state == 600060006) {
			if (page_offset + space->length == chain_page->used) {
				chain_page->used = chain_page->used - space->length % PAGESIZE64K;
			} else {
				chain_page->freed = chain_page->freed + space->length % PAGESIZE64K;
			}
			if (chain_length > 1) {
				chain_page->chain_used = 0;
				page = chain_page;
			}
		}
	}

	int space_length = space->length;
	void * space_pointer = space->pointer;
	space->length = 0;

	if (page->used == page->freed) {
		this->switchPage(chain_page);
	} else if (page->freed > (PAGESIZE64K / 2) && page->chain_used == 0) {
		this->switchPage(page);
	} else if (page->spaces_container != NULL && page->used == page->freed + page->spaces_container->length) {
		int spaces_page_index = ((char *) page->spaces_container->pointer - this->memory_base) / PAGESIZE64K;
		if (spaces_page_index == page->page_index) {
			this->switchPage(page);
		}
	}

	if (space_pointer != NULL && space_length != 0) {
		memset(space_pointer, 0, space_length);
	}

	MemorySpace * freeing_space = NULL;
	if (this->freeing_space_size > 0) {
		this->freeing_space_size--;
		freeing_space = this->freeing_spaces[this->freeing_space_size];
		this->freeing_spaces[this->freeing_space_size] = NULL;
	}
	this->is_freeing = false;

	if (freeing_space != NULL) {
		this->free(freeing_space);
	}

}

bool MemeoryMap::exchange(MemorySpace * left, MemorySpace * right) {

	int left_page_index = 0;
	int left_space_index = 0;
	if (left->length == 0 || left->type == TYPE_NUMBER) {
		left_page_index = -1;
	} else {
		left_page_index = ((char *) left->pointer - this->memory_base) / PAGESIZE64K;
	}

	int right_page_index = 0;
	int right_space_index = 0;
	if (right->length == 0 || right->type == TYPE_NUMBER) {
		right_page_index = -1;
	} else {
		right_page_index = ((char *) right->pointer - this->memory_base) / PAGESIZE64K;
	}

	if (left_page_index != right_page_index) {

		if (left_page_index != -1) {
			right_space_index = right - this->spaces;
			MemoryPage64k * left_page = this->pages + left_page_index;
			int * left_space_indexes = ((int *) left_page->spaces_container->pointer);
			for (int i = left_page->space_length - 1; i > 0; i--) {
				if (left_space_index == *(left_space_indexes + i)) {
					*(left_space_indexes + i) = right_space_index;
					break;
				}
			}
		}

		if (right_page_index != -1) {
			left_space_index = left - this->spaces;
			MemoryPage64k * right_page = this->pages + right_page_index;
			int * right_space_indexes = ((int *) right_page->spaces_container->pointer);
			for (int i = right_page->space_length - 1; i > 0; i--) {
				if (right_space_index == *(right_space_indexes + i)) {
					*(right_space_indexes + i) = left_space_index;
					break;
				}
			}
		}
	}

	int temp[3];
	reverse_memcpy1(temp, left, 12);
	reverse_memcpy1(left, right, 12);
	reverse_memcpy1(right, temp, 12);
	return true;
}

//
//void JSFree(void* pointer) {
//	if (pointer != NULL) {
//		free(pointer);
//	}
//}
