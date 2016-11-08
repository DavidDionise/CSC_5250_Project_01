#ifndef GLOBAL_UTILS
#define GLOBAL_UTILS

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#define R_LEN 5
#define MAX_COMMAND_LENGTH 16
#define MAX_SERVER_RESPONSE_LENGTH 512
#define MAX_USERNAME_LENGTH 64


/*** Status' to send accross network ***/

extern const char* VALID_USER_NAME;
extern const char* USER_NAME_TAKEN;

extern const char* IP_ALREADY_HAS_ACCOUNT;

extern const char* USER_NAME_REGISTERED;
extern const char* ERROR_REGISTERING_USER_NAME;

// Wrapper for the read() system call
void Read(int fd, void * buffer, int bytes);

// Wrapper for the write() system call
void Write(int fd, void * buffer, int bytes);

// Wrapper for the open() system call
void Open(char * file_name, int access, int permissions);

#endif
