#include "../../global_utils/util.h"
#include "util.h"

void registerAccount(int fd, struct sockaddr_in client_addr,
	struct clients_list * c_list) {

	char* client_ip = inet_ntoa(client_addr.sin_addr);
	struct client_user * iterator = c_list->head; 
	
	// Check if client IP has been registered
	while(iterator) {
		if(strcmp(iterator->ip, client_ip) == 0) {
			Write(fd, "IP already registered\0", 22);
			return;
		}
		iterator = iterator->next;
	}

	// IP not yet registered => register client
	int unique_username = 0;
	char username_buffer[MAX_USERNAME_LENGTH];  

	Write(fd, "Enter a user name\0", 18);
	Read(fd, username_buffer, MAX_USERNAME_LENGTH);
	
	//Check if user name is unique
	
	iterator = c_list->head;
	while(iterator) {
		if(strcmp(iterator->username, username_buffer) == 0) {
			unique_username = 1;
			break;
		}
	}

	while(!unique_username) {
		Write(fd, "Username already taken. Enter a new username\n\0", 47);

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
}

void handleClientCommand(int fd, struct sockaddr_in client_addr,
	struct clients_list * c_list) {
	Write(fd, "test", 4);
	char buffer[MAX_COMMAND_LENGTH];

	if(read(*fd, buffer, MAX_COMMAND_LENGTH) < 0) {
		perror("Error reading command");
	}
	else {
		if(strcmp(buffer, "reg") == 0) {
			registerAccount(fd, client_addr, c_list);
		}
	}
}
