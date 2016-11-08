#include "util.h"

const char* VALID_USER_NAME = "10001";
const char* USER_NAME_TAKEN = "10010";

const char* IP_ALREADY_HAS_ACCOUNT = "10011";


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
