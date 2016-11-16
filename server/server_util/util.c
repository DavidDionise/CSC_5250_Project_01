#include "../../global_utils/util.h"
#include "util.h"

void registerAccount(int fd, struct sockaddr_in *client_addr,
	struct clients_list *c_list) {
	int unique_ip_port_combo = 1;

	char *client_ip = inet_ntoa(client_addr->sin_addr);
	char port_buffer[6];
	struct client_user *iterator = c_list->head; 

	Write(fd, READY_TO_RECEIVE, R_LEN); // ** 1
	Read(fd, port_buffer, 6); // ** 2
	
	// Check if client IP/Port combination has been registered
	while(iterator) {
		if(strcmp(iterator->ip, client_ip) == 0) {
			if(strcmp(iterator->port_number, port_buffer) == 0) {
				unique_ip_port_combo = 0;
				break;
			}
		}
		iterator = iterator->next;
	}

	while(!unique_ip_port_combo) {
		unique_ip_port_combo = 1;

		Write(fd, IP_ALREADY_HAS_ACCOUNT, R_LEN); // ** 3
		Read(fd, port_buffer, 6); 

		// Check if client IP/Port combination has been registered
		while(iterator) {
			if(strcmp(iterator->ip, client_ip) == 0) {
				if(strcmp(iterator->port_number, port_buffer) == 0) {
					unique_ip_port_combo = 0;
					break;
				}
			}
			iterator = iterator->next;
		}
	}

	// IP not yet registered => register client
	Write(fd, VALID_IP, R_LEN); // ** 3

	int unique_username = 1;
	char username_buffer[MAX_USERNAME_LENGTH];  

	Read(fd, username_buffer, MAX_USERNAME_LENGTH);	// ** 4

	//Check if user name is unique
	iterator = c_list->head;
	while(iterator) {
		if(strcmp(iterator->username, username_buffer) == 0) {
			unique_username = 0;
			break;
		}
		iterator = iterator->next;
	}

	while(!unique_username) {
		Write(fd, USER_NAME_TAKEN, R_LEN); // ** 5

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
	struct client_user *new_user = malloc(sizeof(struct client_user));

	new_user->username = malloc(sizeof(char) * strlen(username_buffer));
	strcpy(new_user->username, username_buffer);

	new_user->ip = malloc(sizeof(char) * strlen(client_ip));
	strcpy(new_user->ip, client_ip);

	new_user->port_number = malloc(sizeof(char) * strlen(port_buffer) + 1);
	strcpy(new_user->port_number, port_buffer);

	new_user->files = malloc(sizeof(struct files_list));
	new_user->files->head = 0;
	new_user->files->tail = 0;

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
	Write(fd, USER_NAME_REGISTERED, R_LEN); // ** 5
}

void unregisterAccount(int fd, struct sockaddr_in *client_addr, 
	struct clients_list *c_list) {

	int ip_exists = 0;
	char*client_ip = inet_ntoa(client_addr->sin_addr);
	struct client_user *iterator, *iterator_trailer;

	iterator = c_list->head;

	while(iterator) {
		if(strcmp(iterator->ip, client_ip) == 0) {
			ip_exists = 1;

			if(c_list->head == c_list->tail) { // only one name registered
				c_list->head = 0;	
				c_list->tail = 0;
			}
			else if(iterator == c_list->head) 
				c_list->head = iterator->next;
			else if(iterator == c_list->tail) 
				c_list->tail = iterator_trailer;
			else 
				iterator_trailer->next = iterator->next;

			struct file_node *c_file = iterator->files->head;
			// Free file data
			while(c_file) {
				free(c_file->file_name);
				free(c_file->path);

				c_file = c_file->next;	
			}

			free(iterator->username);
			free(iterator->ip);
			free(iterator);
			//TO DO : Free all files
			
			break;
		}
		else {
			// increment iterator and iterator_trailer
			iterator_trailer = iterator;
			iterator = iterator->next;
		}
	}

	if(!ip_exists) 
		Write(fd, IP_DOES_NOT_EXIST, R_LEN);
	else
		Write(fd, USER_UNREGISTERED, R_LEN);
}

void listUsersAndFiles(int fd, struct clients_list *c_list) {
	struct client_user *iterator = c_list->head;
	struct file_node *c_file;  
	char print_file_buf[MAX_PATH_LENGTH + MAX_USERNAME_LENGTH];

	char buffer[R_LEN];

	Write(fd, BEGIN_DATA_BUFFER_SEND, R_LEN);
	Read(fd, buffer, R_LEN);

	if(strcmp(buffer, DATA_RECEIVED) != 0) {
		perror("No confirmation from server");
		return;
	}
	
	while(iterator) {
		c_file = iterator->files->head;

		while(c_file) {
			// Construct filename
			
			strcat(print_file_buf, iterator->username);	
			strcat(print_file_buf, ": ");
			strcat(print_file_buf, c_file->file_name);

			Write(fd, print_file_buf, MAX_PATH_LENGTH + MAX_USERNAME_LENGTH);
			Read(fd, buffer, R_LEN);

			if(strcmp(buffer, DATA_RECEIVED) != 0) {
				perror("No confirmation from client");
				return;
			}
			c_file = c_file->next;
			memset((void*)print_file_buf, '\0', 
				MAX_PATH_LENGTH + MAX_USERNAME_LENGTH);
		}
		iterator = iterator->next;
	}
	Write(fd, END_DATA_BUFFER_SEND, R_LEN);
}

void addFileInfo(int fd, struct clients_list *c_list,
	struct sockaddr_in *client_addr) {
	int user_found = 0;
	char port_buffer[6];
	char* client_ip = inet_ntoa(client_addr->sin_addr);
	struct client_user *client = c_list->head;
	char file_path[MAX_PATH_LENGTH];

	Write(fd, READY_TO_RECEIVE, R_LEN);
	// Get port number to check for uniqueness
	Read(fd, port_buffer, 6);

	while(client) {
		if(strcmp(client->ip, client_ip) == 0) {
			if(strcmp(client->port_number, port_buffer) == 0) {
				user_found = 1;
				break;
			}
		}
		if(user_found)
			break;

		client = client->next;
	}

	printf("user found = %i\n", user_found);

	if(!user_found) {
		Write(fd, IP_DOES_NOT_EXIST, R_LEN);
	}
	else {
		ssize_t read_length;
		int i = 0;
		if(read_length = read(fd, file_path, MAX_PATH_LENGTH) < 0) {
			perror("Error reading file");
			exit(1);
		}

		int file_length = strlen(file_path);

		// Extract name from file path
		char *file_name = &file_path[file_length - 1];
	
		// Check if first char is /
		if(*file_name == '/') { 
			file_name--;
			i++;
		}

		for(i; i < file_length; i++) {
			if(*file_name == '/') {
				file_name++;
				break;
			}
			else
				file_name--;
		}
	
		struct file_node *new_file = malloc(sizeof(struct file_node));
		new_file->file_name = malloc(strlen(file_name) + 1);
		strcpy(new_file->file_name, file_name);

		printf("new file = %s\n", new_file->file_name);

		new_file->path = malloc(strlen(file_path) + 1);
		strcpy(new_file->path, file_path);

		new_file->path = file_path;
		new_file->next = 0;

		if(client->files->head == 0) {
			client->files->head = new_file;
			client->files->tail = new_file;
		}
		else {
			client->files->tail->next = new_file;
			client->files->tail = new_file;
		}
	}
}

void enableDownloadFile(int fd, struct sockaddr_in *client_addr, 
	struct clients_list *c_list) {

	char message_buffer[MAX_SERVER_RESPONSE_LENGTH];
	char user_name_buffer[MAX_USERNAME_LENGTH];
	char file_name_buffer[MAX_PATH_LENGTH];
	int file_found = 0;

	Write(fd, READY_TO_RECEIVE, R_LEN); // ** 1

	Read(fd, user_name_buffer, MAX_USERNAME_LENGTH); // ** 2
	Write(fd, READY_TO_RECEIVE, R_LEN); // ** 3

	Read(fd, file_name_buffer, MAX_PATH_LENGTH); // ** 4

	struct client_user *client = c_list->head;
	struct file_node *c_file;

	// Search for user entered by client
	while(client) {
		if(strcmp(client->username, user_name_buffer) == 0) {
			c_file = client->files->head;
			while(c_file) {
				if(strcmp(c_file->file_name, file_name_buffer) == 0) {
					file_found = 1;
					break;
				}
				else
					c_file = c_file->next;
			}
		}
		if (file_found)
			break;
		else
			client = client->next;
	}

	if(!file_found) {
		Write(fd, FILE_DOES_NOT_EXIST, R_LEN); // ** 5a
		return;
	}

	Write(fd, client->ip, 16); // ** 5b
	Read(fd, message_buffer, R_LEN); // ** 6
	if(strcmp(message_buffer, DATA_RECEIVED) != 0) {
		perror("Error communicating with client");
		return;
	}

	Write(fd, client->port_number, 5); // ** 7
	Read(fd, message_buffer, MAX_SERVER_RESPONSE_LENGTH); // ** 8
	if(strcmp(message_buffer, DATA_RECEIVED) != 0) {
		perror("Error communicating with client");
		return;
	}

	printf("c_file->file_name = %s\n", c_file->file_name);
	printf("c_file->path = %s\n", c_file->path);
	printf("length = %s\n", strlen(c_file->path));

	Write(fd, c_file->path, strlen(c_file->path) + 1); // ** 9
	Read(fd, message_buffer, MAX_SERVER_RESPONSE_LENGTH); // ** 10
	if(strcmp(message_buffer, DATA_RECEIVED) != 0) {
		perror("Error communicating with client");
		return;
	}

}

void *handleClientCommand(void * args_list) {
	struct args *args = (struct args *)args_list;

	// Unpack arguments
	int fd = args->socket_fd;
	struct sockaddr_in *client_addr = args->client_addr;
	struct clients_list *c_list = args->c_list;
	struct files_list *f_list = args->f_list;

	char buffer[R_LEN];

	Read(fd, buffer, R_LEN);

	if(strcmp(buffer, REGISTER_USER) == 0) {
		registerAccount(fd, client_addr, c_list);
	}
	else if(strcmp(buffer, UNREGISTER_USER) == 0) {
		unregisterAccount(fd, client_addr, c_list);
	}
	else if(strcmp(buffer, LIST_AVAILABLE_FILES) == 0) {
		listUsersAndFiles(fd, c_list);
	}	
	else if(strcmp(buffer, UPLOAD_FILE) == 0) {
		addFileInfo(fd, c_list, client_addr);
	}	
	else if(strcmp(buffer, DOWNLOAD_FILE) == 0) {
		enableDownloadFile(fd, client_addr, c_list);
	}

	struct client_user *client = c_list->head;
	while(client) {
		printf("name : %s\n", client->username);
		printf("ip : %s\n", client->ip);
		printf("port : %s\n\n", client->port_number);
		client = client->next;
	}

	return;
}












