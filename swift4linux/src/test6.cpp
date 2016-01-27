#include "test6.h"

void logviaCout6(JSString & jsString) {
	char * char_string = jsString.char_string;
	int length = getLength(char_string);
	if (length != jsString.length) {
		char_string = JSMalloc(jsString.length);
		obverse_memcpy(char_string, jsString.char_string, jsString.length);
		std::cout << char_string;
		JSFree(char_string);
	} else {
		std::cout << char_string;
	}
}

void test6001() {

	JSLog("test6001");

	DMLIST * dm_list = newDMLIST();

	for (int i = 0; i < 7; i++) {
		DMString * dm_string1 = newDMString(1);
		*(dm_string1->char_string) = 'a' + i;
		dm_list->add(-1, (MemorySpace *) dm_string1);
	}
	DMString * dm_string1 = newDMString(1);
	*(dm_string1->char_string) = 'x';

	dm_list->add(0, (MemorySpace *) dm_string1);
	dm_list->add(0, (MemorySpace *) dm_string1);

	DMString * dm_string2 = newDMString(1);
	*(dm_string2->char_string) = 'y';
	dm_list->add(-3, (MemorySpace *) dm_string2);

	DMString * dm_string3 = newDMString(1);
	*(dm_string3->char_string) = 'z';
	dm_list->replace(0, (MemorySpace *) dm_string3);
	dm_list->replace(3, (MemorySpace *) dm_string3);
	dm_list->replace(-1, (MemorySpace *) dm_string3);
	dm_list->replace(-3, (MemorySpace *) dm_string3);

	int i = 1;
	i = i + 1;

}

void test6002() {

	JSLog("test6002");

	DMLIST * dm_list = newDMLIST();

	for (int i = 0; i < 7; i++) {
		DMString * dm_string1 = newDMString(1);
		*(dm_string1->char_string) = 'a' + i;
		dm_list->add(-1, (MemorySpace *) dm_string1);
	}
	DMString * dm_string1 = newDMString(1);
	*(dm_string1->char_string) = 'x';

	dm_list->add(0, (MemorySpace *) dm_string1);
	dm_list->add(0, (MemorySpace *) dm_string1);

	DMString * dm_string2 = newDMString(1);
	*(dm_string2->char_string) = 'y';
	dm_list->add(-3, (MemorySpace *) dm_string2);

	dm_list->del(0);
	dm_list->del(3);
	dm_list->del(-1);
	dm_list->del(-3);

	int i = 1;
	i = i + 1;

}

class TestSpace {
public:
	void * pointer;
	unsigned int length;
};

class TestSpaceMini {
public:
	void * pointer;
	unsigned char length_low;
	unsigned char length_high; //length=length_low+length_high*0x100;
	unsigned char check_code; //check_code=(type+67+length_low+length_high)%256+67
	unsigned char type;
};

void test6003() {
	JSLog("test6003");

	int z1 = sizeof(TestSpace);
	int z2 = sizeof(TestSpaceMini);

	TestSpace * t1 = new TestSpace();
	t1->length = 0x00ff30f1;
	TestSpaceMini *t11 = (TestSpaceMini *) t1;

	TestSpaceMini * t2 = new TestSpaceMini();
	t2->length_low = 0x10;
	t2->type = 0xf2;
	TestSpace *t22 = (TestSpace *) t2;

	int z3 = sizeof(DMString);
	DMString * dm_string2 = newDMString(1);

	int a = 23214214;
	short b = 20011;

	int c = a + b;

	bool d = a < b;

	int i = 1;
	i = i + 1;

}

void test6004() {
	JSLog("test6004");

	const char * json_str1 = "[A:'123',b:[abc,-123, c:ppppp],c:[[]]]";
	;
	DMJSON* json = parseDMJSON(json_str1);
	DMString * json_str2 = stringifyJSON2DMString(json);
	JSLog(json_str2);

	int i = 1;
	i = i + 1;
}

void test6005() {
	JSLog("test6005");

	const char * json_str1 = "[A:'123',b:[abc,-123, c:ppppp],c:[[]]]";
	;
	DMJSON* json = parseDMJSON(json_str1);

	void ** static_spaces;
	static_spaces = get_static_spaces();

	if (static_spaces[12] == NULL) {
		static_spaces[12] = json->self;
		JSLog("json saving");
		DMString * json_str2 = stringifyJSON2DMString(json);
		JSLog(json_str2);

	} else {
		MemorySpace * static_space = (MemorySpace *) static_spaces[12];
		DMJSON* saved_json = (DMJSON*) static_space->pointer;
		DMString * saved_json_str = stringifyJSON2DMString(saved_json);
		JSLog(saved_json_str);
		JSLog("json saved");
	}

	int i = 1;
	i = i + 1;

}

void test6006() {
	JSLog("test6006");

//	JSONBase * json_base = JSONBase::getInstance();
//
//	DMString * secret = NULL;
//
//	RootNode * root_node = json_base->createRootNode(secret);

	int i = 1;
	i = i + 1;
}

void test6007() {
	JSLog("test6007");

	JSONBase * json_base = JSONBase::getInstance();
	int root_id = -2146926591;
	DMString * access_key = NULL;
//	char * key_chain_str = "a123.b222.c88";
//	char * key_chain_str = "a123.b222.c89.d11.e22.f55";
//	DMString * key_chain = newDMString(0);
//	key_chain->char_string = key_chain_str;
//	key_chain->length = getLength(key_chain_str);

//	char * value_str = "i love lixin";
//	DMString * value = newDMString(getLength(value_str));
//	value->copy(value_str, getLength(value_str));
//
//	json_base->set(root_id, access_key, 0, key_chain, (MemorySpace *) value);

//	key_chain->length = 9;
//	MemorySpace * value1 = json_base->get(root_id, access_key, 0, key_chain);
//
//	DMJSON* saved_json = (DMJSON*) value1->pointer;
//	DMString * saved_json_str = stringifyJSON2DMString(saved_json);
//	JSLog(saved_json_str);

	int i = 1;
	i = i + 1;
}

char input[64];
void test6008() {

	JSLog("test6008\n");

	printf("\033[?25h");
	printf("\033[1;4;%dmHello,	world.\033[0m", 32);
	char a;

//	while (gets(input)) {
	while (true) {
		a = getchar();
		putchar(a);
//		JSLog(a);
		JSLog("\n\033[1;34m<> \033[1;32m");
	}

	int i = 1;
	i = i + 1;

}
void test6009() {
	JSLog("test6009\n");
	FILE * output = fopen("/dev/tty", "w");
	fprintf(output, "\033[1;36mhello world\n");

	FILE * input = fopen("/dev/tty", "r");
	int selected;
	do {
		selected = fgetc(input);
	} while (selected == '\n');

	fprintf(output, "\033[1;32mhello world\n");

}

#include <termios.h>

void test6010() {
	JSLog("test6010\n");
	int c;
	static struct termios oldt, newt;

	/* 将当前设置写入oldt。
	 */
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	/* ICANON 如果在输入中看到"\n"或者EOF，会返回缓冲区内容
	 * ECHO 用还控制回显
	 */
	newt.c_lflag &= ~(ICANON | ECHO); // 设置新的终端属性
	newt.c_cc[2] = 0;
	/*
	 *TCSANOW 告诉函数立即改变终端的STDIN_FILENO属性值
	 */
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	// 当输入e时结束循环
	while ((c = getchar()) != 'e') {
		putchar(c);
	}

	// 恢复终端设置
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

}

void test6011() {
//	JSLog("test6011\n");

	WebsocketShell * websocket_shell = WebsocketShell::getInstance();
	websocket_shell->initialize();

	JSONBaseShell * json_base_shell = JSONBaseShell::getInstance();
	json_base_shell->shell();

//	char * key_chain_str = "a123.b222.c89.d11.e22.f55";
//	DMString * key_chain = newDMString(getLength(key_chain_str));
//	key_chain->copy(key_chain_str);
//
//	DMLog(key_chain);

	int i = 1;
	i = i + 1;
}
void test6000() {
	flag_cout = true;
	log2cout = logviaCout6;
	test6011();
}
