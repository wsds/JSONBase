#include "test5.h"

void logviaCout5(JSString & jsString) {
	char * char_string = jsString.char_string;
	int length = getLength(char_string);
	if (length != jsString.length) {
		char_string = JSMalloc(jsString.length);
		obverse_memcpy(char_string, jsString.char_string, jsString.length);
		std::cout << char_string << std::endl;
		JSFree(char_string);
	} else {
		std::cout << char_string << std::endl;
	}
}

void test5001() {

	JSLog("test5001");

	int size1 = sizeof(MemorySpace);
	int size2 = sizeof(MemoryPage64k);
	int size3 = sizeof(MemeoryMap);

	int i = 1;
	i = i + 1;

}

void test5002() {

	JSLog("test5002");

	int memeoryMapFD = open("/alidata/test/memeory.odb", O_CREAT | O_RDWR, 0777);

	int fileLength = 1024 * 1024 * 128 * 4;

	ftruncate(memeoryMapFD, fileLength);

	char * traceFileBuffer = (char*) mmap(NULL, fileLength, PROT_READ | PROT_WRITE, MAP_SHARED, memeoryMapFD, 0);

	int i = 1;
	i = i + 1;
}

void test5003() {
	JSLog("test5003");

	MemeoryMap * memeoryMap = reloadMemeoryMap();

	MemorySpace * a1 = memeoryMap->malloc(123);
	memset(a1->pointer, 'a', a1->length);
//
	MemorySpace * b1 = memeoryMap->malloc(46655);
	memset(b1->pointer, 'b', b1->length);

	int i = 1;
	i = i + 1;

}

int pool[] = { 2, 3, 6, 7, 8, 11, 12, 13, 14, 15, 19, 22, 23, 34, 35, 46, 47, 48, 49, 50 };
int size = 20;
int used_size1 = 0;

int getPageChain(int chain_length) {

	int macthed_chain_length = 1;
	int last_index = -100;
	int index = -100;
	int position = -100;
	for (int i = used_size1; i < size; i++) {
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
		reverse_memcpy1(pool + used_size1 + chain_length, pool + used_size1, (position - chain_length - used_size1) * sizeof(int));

		memset(pool + used_size1, 0, chain_length * sizeof(int));

		used_size1 = used_size1 + chain_length;
	}

	return index;
}

void test5004() {

	JSLog("test5004");

	int index = 0;
	index = getPageChain(3);
	index = getPageChain(3);
	index = getPageChain(3);
	index = getPageChain(1);
	index = getPageChain(1);
	index = getPageChain(2);

	int i = 1;
	i = i + 1;

}

void test5005() {
	JSLog("test5005");
	MemeoryMap * memeoryMap = reloadMemeoryMap();

	MemorySpace * a1 = memeoryMap->malloc(35 * 1024);
	memeoryMap->free(a1);

	MemorySpace * a2 = memeoryMap->malloc(40 * 1024);
	MemorySpace * a3 = memeoryMap->malloc(45 * 1024);
	MemorySpace * a4 = memeoryMap->malloc(55 * 1024);

	MemorySpace * a5 = memeoryMap->malloc(550 * 1024);

	MemorySpace * a6 = memeoryMap->malloc(65 * 1024);

	memeoryMap->free(a6);

	int i = 1;
	i = i + 1;

}

void test5006() {
	JSLog("test5006");

	MemeoryMap * memeoryMap = reloadMemeoryMap();

	MemorySpace * a0 = memeoryMap->malloc(63 * 1024);

	MemorySpace * a1 = memeoryMap->malloc(35 * 1024);
	MemorySpace * a6 = memeoryMap->malloc(65 * 3 * 1024);
	memeoryMap->free(a6);
	memeoryMap->free(a1);

	MemorySpace * a7 = memeoryMap->malloc(65 * 1024);
	memeoryMap->free(a7);

	int i = 1;
	i = i + 1;

}

void test5007() {
	JSLog("test5007");

	MemeoryMap * memeoryMap = reloadMemeoryMap();

	MemorySpace * a0 = memeoryMap->malloc(63 * 1024 + 1000);
	memeoryMap->free(a0);

	MemorySpace * a1 = memeoryMap->malloc(63 * 1024 + 1000);
	memeoryMap->free(a1);

	MemorySpace * a2 = memeoryMap->malloc(65 * 3 * 1024 + 1000);
	memeoryMap->free(a2);

	int i = 1;
	i = i + 1;

}

void test5008() {

	JSLog("test5008");

	MemeoryMap * memeoryMap = reloadMemeoryMap();

	MemorySpace * a0 = memeoryMap->malloc(30 * 1024);
	memset(a0->pointer, 'a', a0->length);

	MemorySpace * a1 = memeoryMap->malloc(33 * 1024);
	MemorySpace * a2 = memeoryMap->malloc(20 * 1024);
	MemorySpace * a3 = memeoryMap->malloc(33 * 1024);
	MemorySpace * a4 = memeoryMap->malloc(10 * 1024);

	memeoryMap->free(a1);
	memeoryMap->free(a3);
	memeoryMap->free(a2);
	memeoryMap->free(a4);
	memeoryMap->free(a0);

	int i = 1;
	i = i + 1;

}

void test5009() {
	JSLog("test5009");

	MemeoryMap * memeoryMap = reloadMemeoryMap();

	for (int i = 0; i < 10; i++) {
		MemorySpace * a1 = memeoryMap->malloc(33 * 1024);
		MemorySpace * a2 = memeoryMap->malloc(10 * 1024);

		memeoryMap->free(a2);
		memeoryMap->free(a1);
	}

	int i = 1;
	i = i + 1;

}

//DMString * newDMString(int length, MemeoryMap * memeoryMap) {
//	MemorySpace * space = memeoryMap->malloc(sizeof(DMString));
//	DMString * dm_string = (DMString *) space->pointer;
//	dm_string->self = space;
//	dm_string->space = memeoryMap->malloc(length);
//	return dm_string;
//}
//
//void test5010() {
//	JSLog("test5010");
//	MemeoryMap * memeoryMap = reloadMemeoryMap();
//	DMString * dm_string = NULL;
//	if (memeoryMap->static_spaces[2] == NULL) {
//		int length = 128;
//		dm_string = newDMString(length, memeoryMap);
//		memeoryMap->static_spaces[2] = dm_string->self;
//
//		memset(dm_string->space->pointer, 'a', dm_string->space->length);
//
//	} else {
//		dm_string = (DMString *) ((MemorySpace *) memeoryMap->static_spaces[2])->pointer;
//	}
//
//	int i = 1;
//	i = i + 1;
//
//}

void test5011() {
	JSLog("test5011");
	MemeoryMap * memeoryMap = reloadMemeoryMap();
	DMString * dm_string1 = NULL;
	if (memeoryMap->static_spaces[3] == NULL) {
		int length = 128;
		dm_string1 = newDMString(length);
		memeoryMap->static_spaces[3] = dm_string1;

		memset(dm_string1->char_string, 'b', dm_string1->max_length);

	} else {
		dm_string1 = (DMString *) memeoryMap->static_spaces[3];
	}

	int i = 1;
	i = i + 1;
}

class Test5012C {
public:
	int a;
};

void test5012() {
	Test5012C * list = NULL;

	Test5012C * a = new Test5012C();

	Test5012C * list15 = list + 15;
	Test5012C ** list_p = &list15;

	*list_p = a;

}

void test5013() {
	JSLog("test5013");

	if (memeoryMap == NULL) {
		memeoryMap = reloadMemeoryMap();
	}

	DMHashTable * dm_hashTable;
	if (memeoryMap->static_spaces[4] == NULL) {
		dm_hashTable = newDMHashTable();
		memeoryMap->static_spaces[4] = dm_hashTable->self;

		DMString * key = newDMString(5);
		memset(key->char_string, 'c', key->max_length);

		DMString * value = newDMString(15);
		memset(value->char_string, 'd', value->max_length);

		dm_hashTable->set((MemorySpace *) key, (MemorySpace *) value);

	} else {
		dm_hashTable = (DMHashTable *) ((MemorySpace *) memeoryMap->static_spaces[4])->pointer;

		DMString * key = newDMString(5);
		memset(key->char_string, 'c', key->max_length);

		DMString * value = (DMString *) dm_hashTable->get((MemorySpace *) key);

		freeDMString(key);
		int i = 1;
		i = i + 1;

	}

	int i = 1;
	i = i + 1;

}
void test5014() {
	JSLog("test5013");

	LocalStorage * storage = LocalStorage::getInstance();
	char * value = NULL;

	storage->savaString("a", "111111111");
	value = storage->loadString("a");

	storage->savaString("bbb", "22222222222222");
	storage->savaString("ccccccccc", "333333333333333333333333333333");
	storage->savaString("dddddddddddd", "4444444444444444444444444444444444444");
	storage->savaString("eeeeeeeeeeeeeeeeeeee", "555555555555555555555555555555555555555555555555");

	value = storage->loadString("eeeeeeeeeeeeeeeeeeee");

	int i = 1;
	i = i + 1;
}

void test5015() {

	JSLog("test5015");

	LocalStorage * storage = NULL;
	char a[6] = { 'a', 'a', 'a', 0, 0, 0 };
	char b[13] = { '1', '1', '1', '1', '1', '1', '1', '1', '1', '@', 0, 0, 0 };

	int key_length = 0;
	int data_length = 0;

	for (int i = 0; i < 100; i++) {
		if (i >= 10) {
			a[3] = i / 10 + '0';
			a[4] = i % 10 + '0';
			b[10] = i / 10 + '0';
			b[11] = i % 10 + '0';
			key_length = 5;
			data_length = 12;
		} else {
			a[3] = i % 10 + '0';
			a[4] = 0;
			b[10] = i % 10 + '0';
			b[11] = 0;
			key_length = 4;
			data_length = 11;
		}

		DMString * key = newDMString(key_length);
		reverse_memcpy1(key->char_string, (void *) a, key_length);
		DMString * value = newDMString(data_length + 1); //the last '\0' is add to fit the JNI call of env->NewStringUTF(result_str);
		reverse_memcpy1(value->char_string, (void *) b, data_length);

		if (storage == NULL) {
			storage = LocalStorage::getInstance();
		}

		storage->savaString(a, b);
	}
	for (int i = 0; i < 100; i++) {
		if (i >= 10) {
			a[3] = i / 10 + '0';
			a[4] = i % 10 + '0';
		} else {
			a[3] = i % 10 + '0';
			a[4] = 0;
		}
		char * value = storage->loadString(a);
//		JSLog(a);
		JSLog(value);
	}

	int i = 1;
	i = i + 1;

}
void test5000() {
	flag_cout = true;
	log2cout = logviaCout5;
	test5015();
}
