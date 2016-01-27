#include "libImage.h"

void test50002() {

	int FD = open("/alidata/swift/web/test1.jpg", O_RDWR, 0777);
	struct stat sb;
	fstat(FD, &sb);
	int fileLength = sb.st_size;

	char * dataBuffer = (char*) mmap(NULL, fileLength, PROT_READ, MAP_SHARED, FD, 0);
//

	char* sha1StrHex = sha1((void*) dataBuffer, fileLength);
//
}

JSON * resolveImage(JSON * image_info) {
//
//
//	resolveFileData.newFilePath = "/alidata/test/";
//	resolveFileData.filePath = "/alidata/swift/web/test1.jpg";
	JSString * js_newFilePath = (JSString *) ~(*image_info)["newFilePath"];
	JSString * js_filePath = (JSString *) ~(*image_info)["filePath"];
	JSString * js_suffixName = (JSString *) ~(*image_info)["suffixName"];

	JSLog(image_info);
	js_filePath->resize(js_filePath->length);
	int FD = open(js_filePath->char_string, O_RDWR, 0777);
	if (FD < 0) {
		return NULL;
	}
	struct stat sb;
	fstat(FD, &sb);
	int fileLength = sb.st_size;

	char * fileBuffer = (char*) mmap(NULL, fileLength, PROT_READ, MAP_SHARED, FD, 0);
	if (fileBuffer == (void *) -1) {
		return NULL;
	}
	char* sha1StrHex = sha1((void*) fileBuffer, fileLength);
	JSString * fileName = new JSString(sha1StrHex);
	(*fileName) + "." + (*js_suffixName);

	(*js_newFilePath) + (*fileName);
	int newFileLength = fileLength;
	int new_FD = open(js_newFilePath->char_string, O_CREAT | O_RDWR, 0777);
	if (new_FD < 0) {
		return NULL;
	}
	ftruncate(new_FD, 1);
	char * newFileBuffer = (char*) mmap(NULL, newFileLength, PROT_READ | PROT_WRITE, MAP_SHARED, new_FD, 0);
	if (newFileBuffer == (void *) -1) {
		return NULL;
	}
	ftruncate(new_FD, newFileLength);

	memcpy(newFileBuffer, fileBuffer, newFileLength);

	JSON *result = new JSON();
	result->initialize();

	(*result)["fileLength"] = fileLength;
	(*result)["fileName"] = fileName;

	js_filePath->~JSString();
	js_newFilePath->~JSString();
	munmap(fileBuffer, fileLength);
	close(FD);
	munmap(newFileBuffer, newFileLength);
	close(new_FD);
	return result;
}
