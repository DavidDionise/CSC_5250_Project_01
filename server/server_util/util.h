#ifndef SERVER_UTILS
#define SERVER_UTILS

#include <unistd.h>     
#include <sys/types.h>  
#include <sys/socket.h> 
#include <netdb.h>
#include <netinet/in.h> 
#include <arpa/inet.h>  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*** LINKED LIST FOR STORING FILES IN SERVER ***/

struct file_node {
	char* file_name;
	char* path;

	struct file_node* next;
};

struct files_list {
	struct file_node* head;
	struct file_node* tail;
};

/**************************************************/


/*** LINKED LIST FOR STORING USERS IN SERVER ***/

struct client_user {
	char* ip;
	char* username;
	struct files_list files;

	struct client_user * next;
};

struct clients_list {
	struct client_user* head;
	struct client_user* tail;
};

/**************************************************/

// Interprets the command entered from a client
void handleClientCommand(int fd, struct sockaddr_in client_addr,
	struct clients_list * c_list);

// Registers a users account into server
void registerAccount(int fd, struct sockaddr_in client_addr,
	struct clients_list * c_list);

#endif

