#include "util.h"

const char* REGISTER_USER = "000000";
const char* UNREGISTER_USER = "000001";
const char* DOWNLOAD_FILE = "000010";
const char* UPLOAD_FILE = "000011";
const char* LIST_AVAILABLE_FILES = "000100";
const char* QUIT = "000101";
const char* HELP = "000110";

const char* DATA_RECEIVED = "010001"; // LATEST NUMBER //

const char* BEGIN_DATA_BUFFER_SEND = "000111";
const char* END_DATA_BUFFER_SEND = "001000";
const char* VALID_USER_NAME = "001001";
const char* USER_NAME_TAKEN = "001010";
const char* VALID_IP = "001011";
const char* IP_ALREADY_HAS_ACCOUNT = "001100";
const char* USER_NAME_REGISTERED = "001101";
const char* ERROR_REGISTERING_USER_NAME = "001110";
const char* IP_DOES_NOT_EXIST = "001111";
const char* USER_UNREGISTERED = "010000";

void Read(int fd, void * buffer, int bytes) {
	if(read(fd, buffer, bytes) < 0) {
		perror("Error reading file");
		exit(1);
	}
}

void Write(int fd, void * buffer, int bytes) {
	if(write(fd, buffer, bytes) < 0) {
		perror("Error writing file");
		exit(1);
	}
}

void Open(char * file_name, int access, int permissions) {
	if(open(file_name, access, permissions) < 0) {
		perror("Error opening file");
		exit(1);
	}
}
