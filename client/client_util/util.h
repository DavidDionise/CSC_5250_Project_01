#ifndef CLIENT_UTILS
#define CLIENT_UTILS

#include <unistd.h>     
#include <sys/types.h>  
#include <sys/socket.h> 
#include <netdb.h>
#include <netinet/in.h> 
#include <arpa/inet.h>  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "../../global_utils/util.h"

// Starts server to listen for peers
void* serverRoutine(void *arg); 

// Handle peer request
void* handlePeer(void *fd);

// Reads a line until new line character
char* getLine();

// Returns an IP address from a domain name 
//	or IP address
char* getIP(char* domain_name);

// Prints a menu of commands to operatre 
// 	the ftp shell
void printMenu();

// Prompts user to register a username
// 	with the server
void registerUser(int fd, char** port_buffer);

// Unregisters user from server
void unregisterUser(int fd);

// List users and files in the system
void listUsersAndFiles(int fd);

// Upload file to systek
void uploadFileInfo(int fd, char** port_buffer);

// Download file from peer
void downloadFile(int fd);

// Gets the command from the user and 
//	executes program accordingly
void handleCommand(int fd, int *deregistering, char** port_buffer);


#endif
