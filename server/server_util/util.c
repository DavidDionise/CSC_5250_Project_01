#include "../../global_utils/util.h"
#include "util.h"

void registerAccount(int fd, struct sockaddr_in * client_addr,
	struct clients_list * c_list) {

	char* client_ip = inet_ntoa(client_addr->sin_addr);
	struct client_user * iterator = c_list->head; 

	// Check if client IP has been registered
	while(iterator) {
		if(strcmp(iterator->ip, client_ip) == 0) {
			Write(fd, IP_ALREADY_HAS_ACCOUNT, R_LEN);
			return;
		}
		iterator = iterator->next;
	}

	// IP not yet registered => register client
	Write(fd, VALID_IP, R_LEN);

	int unique_username = 1;
	char username_buffer[MAX_USERNAME_LENGTH];  

	Read(fd, username_buffer, MAX_USERNAME_LENGTH);
	
	//Check if user name is unique
	
	iterator = c_list->head;
	while(iterator) {
		if(strcmp(iterator->username, username_buffer) == 0) {
			unique_username = 0;
			break;
		}
	}

	while(!unique_username) {
		Write(fd, USER_NAME_TAKEN, R_LEN);

		unique_username = 0;
		bzero(username_buffer, MAX_USERNAME_LENGTH);
		
		Read(fd, username_buffer, MAX_USERNAME_LENGTH);
		
		//Check if user name is unique
		
		iterator = c_list->head;
		while(iterator) {
			if(strcmp(iterator->username, username_buffer) == 0) {
				unique_username = 1;
				break;
			}
		}
	}
	
	// All is well, initialize new user
	struct client_user * new_user = malloc(sizeof(struct client_user));
	new_user->username = username_buffer;
	new_user->ip = client_ip;

	// Attach new user to linked list
	if(c_list->head == 0) {
		c_list->head = new_user;
		c_list->tail = new_user;

		new_user->next = 0;
	}
	else {
		new_user->next = 0;

		c_list->tail->next = new_user;
		c_list->tail = new_user;
	}
	
	// Send confirmation to client
	Write(fd, USER_NAME_REGISTERED, R_LEN);
}

void * handleClientCommand(void * args_list) {
	struct args * args = (struct args *)args_list;

	// Unpack arguments
	int fd = args->socket_fd;
	struct sockaddr_in * client_addr = args->client_addr;
	struct clients_list * c_list = args->c_list;

	char buffer[MAX_COMMAND_LENGTH];

	Read(fd, buffer, MAX_COMMAND_LENGTH);
	
	if(strcmp(buffer, REGISTER_USER) == 0) {
		registerAccount(fd, client_addr, c_list);
	}
}
