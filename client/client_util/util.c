#include "util.h"

char* getLine() {
	int length = 10;
	int count = 0;
	char* buffer = (char*)malloc(sizeof(char) * length);
	char* buf_ptr = &buffer[0];
	char c;

	*buf_ptr = '\0';
	
	while((c = getchar()) != '\n') {
		if(count >= (length - 1)) {
			length += 10;
			buffer = (char*)realloc(buffer, length);
		}

		++count;
		*buf_ptr = c;
		*(++buf_ptr) = '\0';
	}

	return buffer;
}

char* getIP(char* domain_name) {
	struct addrinfo res;
	struct addrinfo *res_ptr = &res;
	int error_val;

	memset(&res, 0, sizeof(res));
	
	if((error_val = getaddrinfo(domain_name, 0, 0, &res_ptr)) == 0) {
		struct sockaddr_in *sa = (struct sockaddr_in *) res_ptr->ai_addr;
		socklen_t sa_len = (socklen_t) sizeof(*sa);

		return inet_ntoa(sa->sin_addr);
	}
	else {
		printf("%s", gai_strerror(error_val));
		exit(1);
	}
}

void printMenu() {
	printf("\n\n");
	puts("********************** FTP CLIENT ***********************");
	printf("\n\n");
	puts("Enter one of the following commands :");
	printf("\n");

	puts("reg ..... Register account with server");
	puts("unreg ... Uregister account");
	puts("down .... Download file from another user");
	puts("up ...... Upload file to make available for downloading");
	puts("list .... List available files in server");
	puts("quit .... Quit ftp");
	puts("help .... Re-print instructins");
	printf("\n");
	puts("**********************************************************");
	printf("\n\n");
}

void *serverRoutine(void *port_buffer) {
	// Initialize socket
	int socket_fd, new_socket;
	int port_number = atoi(port_buffer);

	struct sockaddr_in  server_addr, client_addr;
	int client_addr_length = sizeof(client_addr);

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port_number);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Error creating socket");
		exit(1);
	}

	if((bind(socket_fd, (const struct sockaddr *) &server_addr,
		(socklen_t)sizeof(server_addr))) < 0) {
		perror("Error binding");
		exit(1);
	}	

	if(listen(socket_fd, 32) < 0) {
		perror("Error initializing listen");
		exit(1);
	}

	printf("Server Listening on Port %s\n", port_buffer);
	
	while(1){
		if ((new_socket = accept(socket_fd, (struct sockaddr *)&client_addr, 
			(socklen_t*)&client_addr_length)) < 0) {
			perror("Error accepting client");
			exit(1);
		}

		// Initialize thread
		pthread_t tid;
		pthread_attr_t attr;

		pthread_attr_init(&attr);

		// Set thread as detached
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

		pthread_create(&tid, &attr, &handlePeer, NULL);
	}

	close(socket_fd);
	close(new_socket);
	return 0;
}

void *handlePeer(void *arg) {
	puts("in handlePeer");
	return;
}

void registerUser(int fd, int port_number) {
	char buffer[MAX_SERVER_RESPONSE_LENGTH];
	char port_buffer[5];
	char message_buffer[MAX_SERVER_RESPONSE_LENGTH];

	Write(fd, REGISTER_USER, R_LEN);
	Read(fd, buffer, MAX_SERVER_RESPONSE_LENGTH);

	// Check if user has already registered
	if(strcmp(IP_ALREADY_HAS_ACCOUNT, buffer) != 0) {

		puts("Enter a user name :\n");

		char * user_name = getLine();	

		Write(fd, user_name, strlen(user_name) + 1);
		Read(fd, buffer, MAX_SERVER_RESPONSE_LENGTH);

		free(user_name);

		while(strcmp(buffer, USER_NAME_TAKEN) == 0) {
			puts("Username already in use.");
			puts("Enter a different username : \n");

			user_name = getLine();

			Write(fd, user_name, strlen(user_name) + 1);
			Read(fd, buffer, MAX_SERVER_RESPONSE_LENGTH);

			free(user_name);
		}

		sprintf(port_buffer, "%i", port_number);

		Write(fd, port_buffer, 5);
		Read(fd, message_buffer, MAX_SERVER_RESPONSE_LENGTH);

		if(strcmp(message_buffer, USER_NAME_REGISTERED) == 0) {
			printf("User name has been registered with the system");
		}
		else {
			perror("Error registering name with server");
			exit(1);
		}
	}
	else {
		puts("This IP address is already registered with the server");		
	}
}

void unregisterUser(int fd) {
	char* buffer[R_LEN];

	Write(fd, UNREGISTER_USER, R_LEN);
	Read(fd, buffer, R_LEN);

	if(strcmp(buffer, IP_DOES_NOT_EXIST) == 0) {
		puts("This IP is not registered");
	}
	else if(strcmp(buffer, USER_UNREGISTERED) == 0) {
		puts("Unregistered successfully");
	}
	else {
		perror("Error unregistering user");
	}
}

void listUsersAndFiles(int fd) {
	char buffer[MAX_SERVER_RESPONSE_LENGTH];
	
	Write(fd, LIST_AVAILABLE_FILES, R_LEN);
	Read(fd, buffer, MAX_SERVER_RESPONSE_LENGTH);

	Write(fd, DATA_RECEIVED, R_LEN);
	if(strcmp(buffer, BEGIN_DATA_BUFFER_SEND) != 0) {
		perror("Error getting data from server");
		exit(1);
	}

	while(strcmp(buffer, END_DATA_BUFFER_SEND) != 0) {
		Read(fd, buffer, MAX_SERVER_RESPONSE_LENGTH);
		Write(fd, DATA_RECEIVED, R_LEN);

		if(strcmp(buffer, END_DATA_BUFFER_SEND) != 0)
			printf("  %s\n", buffer);
	}
}

void uploadFileInfo(int server_fd) {
	char* path;
	char buffer[R_LEN];
	int file_fd;
	struct stat file_stats;

	Write(server_fd, UPLOAD_FILE, R_LEN);
	Read(server_fd, buffer, R_LEN);

	if(strcmp(buffer, READY_TO_RECEIVE) != 0) {
		if(strcmp(buffer, IP_DOES_NOT_EXIST) == 0) {
			puts("Must register an account before uploading files");
		}
	}

	puts("Enter the path to the file");
	path = getLine();

	if(file_fd = open(path, O_RDONLY, 0) < 0) {
		if(errno == ENOENT)
			puts("No file at path");
		else {
			perror("Error opening file");
		}
	}

	if(fstat(file_fd, &file_stats) < 0) {
		perror("Error getting file stats");
	}

	Write(server_fd, path, strlen(path) + 1);
}

void downloadFile(int fd) {
	char* file_name;
	char message_buffer[MAX_SERVER_RESPONSE_LENGTH];
	char path_buffer[MAX_PATH_LENGTH];
	char port_buffer[5];
	char ip_buffer[16];
	int peer_socket_fd;

	Write(fd, DOWNLOAD_FILE, R_LEN);
	Read(fd, message_buffer, MAX_SERVER_RESPONSE_LENGTH);

	if(strcmp(message_buffer, READY_TO_RECEIVE) != 0) {
		perror("Error communicating with server");
		return;
	}
	
	puts("Enter a filename to download :");
	file_name = getLine();

	Write(fd, file_name, strlen(file_name) + 1);

	// Get IP
	Read(fd, ip_buffer, MAX_SERVER_RESPONSE_LENGTH);
	if(strcmp(message_buffer, FILE_DOES_NOT_EXIST) != 0) {
		perror("File does not exist");
		return;
	}
	Write(fd, DATA_RECEIVED, R_LEN);

	// Get port number
	Read(fd, port_buffer, 5);
	Write(fd, DATA_RECEIVED, R_LEN);

	// Get path
	Read(fd, path_buffer, MAX_SERVER_RESPONSE_LENGTH);
	Write(fd, DATA_RECEIVED, R_LEN);

	peer_socket_fd = socket(AF_INET, SOCK_STREAM, 0);	
	struct sockaddr_in peer_addr;

	int port_number = atoi(port_buffer);

	// Initialize socket address
	peer_addr.sin_family = AF_INET;
	peer_addr.sin_port = htons(port_number);
	if(inet_aton(ip_buffer, &peer_addr.sin_addr.s_addr) == 0) {
		perror("Error connecting to peer\n");
		exit(1);
	}

	// Connect socket
	connect(peer_socket_fd, (struct sockaddr*)&peer_addr, 
		sizeof(peer_addr));

}

void handleCommand(int fd, int *deregistering, int my_port_number) {
	int valid_command = 0;
	char buffer[MAX_COMMAND_LENGTH];

	while(!valid_command) {
		Read(STDIN_FILENO, buffer, MAX_COMMAND_LENGTH);
		
		if(strcmp(buffer, "reg\n") == 0) {
			registerUser(fd, my_port_number);
			valid_command = 1;
		}
		else if(strcmp(buffer, "unreg\n") == 0) {
			unregisterUser(fd);
			valid_command = 1;
		}
		else if(strcmp(buffer, "down\n") == 0) {
			downloadFile(fd);
			valid_command = 1;
		}
		else if(strcmp(buffer, "up\n") == 0) {
			uploadFileInfo(fd);
			valid_command = 1;
		}
		else if(strcmp(buffer, "list\n") == 0) {
			listUsersAndFiles(fd);
			valid_command = 1;
		}
		else if(strcmp(buffer, "quit\n") == 0) {
			puts("quit command");
			valid_command = 1;
		}
		else {
			puts("ERROR : Invalid command");
			printMenu();
		}
	}
}
