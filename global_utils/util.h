#ifndef GLOBAL_UTILS
#define GLOBAL_UTILS

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#define R_LEN 7
#define MAX_COMMAND_LENGTH 16
#define MAX_SERVER_RESPONSE_LENGTH 512
#define MAX_DATA_BUFFER_SIZE 512
#define MAX_USERNAME_LENGTH 64
#define MAX_PATH_LENGTH 256

// Struct to pass arguments to pthread routine
struct args {
	int socket_fd;
	struct sockaddr_in *client_addr;
	struct clients_list *c_list;
	struct files_list *f_list;
};

/*** Requests from user ***/

extern const char* REGISTER_USER;
extern const char* UNREGISTER_USER;
extern const char* DOWNLOAD_FILE;
extern const char* UPLOAD_FILE;
extern const char* LIST_AVAILABLE_FILES;
extern const char* QUIT;

extern const char* DATA_RECEIVED;
extern const char* READY_TO_RECEIVE;

/*** Responses from server ***/

extern const char* BEGIN_DATA_BUFFER_SEND;
extern const char* END_DATA_BUFFER_SEND;
extern const char* VALID_USER_NAME;
extern const char* USER_NAME_TAKEN;
extern const char* VALID_IP;
extern const char* IP_ALREADY_HAS_ACCOUNT;
extern const char* USER_NAME_REGISTERED;
extern const char* ERROR_REGISTERING_USER_NAME;
extern const char* IP_DOES_NOT_EXIST;
extern const char* USER_UNREGISTERED;
extern const char* FILE_DOES_NOT_EXIST;

// Wrapper for the read() system call
void Read(int fd, void * buffer, int bytes);

// Wrapper for the write() system call
void Write(int fd, void * buffer, int bytes);

// Wrapper for the open() system call
void Open(char * file_name, int access, int permissions);

#endif
