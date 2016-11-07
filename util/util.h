
#include <unistd.h>     
#include <sys/types.h>  
#include <sys/socket.h> 
#include <netdb.h>
#include <netinet/in.h> 
#include <arpa/inet.h>  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COMMAND_LENGTH 16

// Returns an IP address from a domain name 
//	or IP address
char* getIP(char* domain_name);

// Prints a menu of commands to operatre 
// 	the ftp shell
void printMenu();

// Gets the command from the user and 
//	executes program accordingly
void handleCommand();

// Routine to manage incoming download requests
// 	from peers
void handlePeer();
