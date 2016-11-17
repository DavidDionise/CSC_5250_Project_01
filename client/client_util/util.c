#include "util.h"

char* getLine() {
	int length = 40;
	int count = 0;
	char* buffer = malloc(length);
	char* buf_ptr = &buffer[0];
	char c;

	*buf_ptr = '\0';
	
	while((c = getchar()) != '\n') {
		if(count >= (length - 1)) {
			length += 10;
			buffer = realloc(buffer, length);
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

	// Wait for port to be initialized
	while(!PORT_INIT) 
		;

	char** port_ptr = (char**)port_buffer;

	// Initialize socket
	int socket_fd, new_socket;
	int port_number = atoi(*port_ptr);

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
;	
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

		pthread_create(&tid, &attr, &handlePeer, &new_socket);
	}

	close(socket_fd);
	return 0;
}

void *handlePeer(void *fd) {
	char path_buffer[MAX_PATH_LENGTH];
	int file_fd;
	int *peer_fd;
	char message_buffer[R_LEN];

	peer_fd = (int*)fd;

	void *buffer[MAX_DATA_BUFFER_SIZE];
	int read_length;

	struct stat file_stats;
	off_t file_size;
	int curs_pos = 0; 
	
	// Get path from peer
	Read(*peer_fd, path_buffer, MAX_PATH_LENGTH); // ** 1

	if((file_fd = open(path_buffer, O_RDONLY)) < 0) {
		perror("Error opening file for reading");
		return;
	}

	if(fstat(file_fd, &file_stats) < 0) {
		perror("Error getting file stats");
	}

	file_size = file_stats.st_size;

	Write(*peer_fd, BEGIN_DATA_BUFFER_SEND, R_LEN); // ** 2
	Read(*peer_fd, message_buffer, R_LEN); // ** 3

	if(strcmp(message_buffer, READY_TO_RECEIVE) != 0) {
		perror("Error communicating with peer");
		exit(1);
	}

	while(curs_pos < file_size) {
		if((read_length = read(file_fd, buffer, MAX_DATA_BUFFER_SIZE)) < 0) {
			perror("Error reading file");
			return;
		}

		curs_pos += read_length;
		
		Write(*peer_fd, buffer, read_length); // ** 4
		Read(*peer_fd, buffer, R_LEN); // ** 5

		if(strcmp(buffer, READY_TO_RECEIVE) != 0) {
			perror("Error communicating with peer");
			return;
		}
	}

	Write(*peer_fd, END_DATA_BUFFER_SEND, R_LEN); // ** 6
	close(file_fd);
	close(*peer_fd);

	return;
}

void registerUser(int fd, char** port_buffer) {
	char buffer[MAX_SERVER_RESPONSE_LENGTH];

	Write(fd, REGISTER_USER, R_LEN);
	Read(fd, buffer, R_LEN); // ** 1

	if(strcmp(buffer, READY_TO_RECEIVE) != 0) {
		perror("Error communicating with server");
		exit(1);
	}

	puts("Enter a port number to accept peer requests :");
	char* temp_buf = getLine();
	strcpy(*port_buffer, temp_buf);

	Write(fd, *port_buffer, 6); // ** 2
	Read(fd, buffer, R_LEN); // ** 3

	while(strcmp(buffer, IP_ALREADY_HAS_ACCOUNT) == 0) {
		puts("IP/port combination not unique."); 
		puts("Enter a new port number to accept peer requests:");

		temp_buf = getLine();

		Write(fd, temp_buf, strlen(buffer) + 1); // ** 3a
		Read(fd, buffer, R_LEN); // ** 3b

		if(strcmp(buffer, VALID_IP))
			strcpy(*port_buffer, temp_buf);
	}
	// Flag port as initialized
	PORT_INIT = 1;

	// Check if user has already registered
	puts("Enter a user name :\n");

	char *user_name = getLine();	

	Write(fd, user_name, strlen(user_name) + 1); // ** 4
	Read(fd, buffer, MAX_SERVER_RESPONSE_LENGTH); // ** 5

	free(user_name);

	while(strcmp(buffer, USER_NAME_TAKEN) == 0) {
		puts("Username already in use.");
		puts("Enter a different username : \n");

		user_name = getLine();

		Write(fd, user_name, strlen(user_name) + 1);
		Read(fd, buffer, MAX_SERVER_RESPONSE_LENGTH);

		free(user_name);
	}

	if(strcmp(buffer, USER_NAME_REGISTERED) == 0) {
		printf("User name has been registered with the system\n\n");
	}
	else {
		perror("Error registering name with server");
		exit(1);
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

void uploadFileInfo(int server_fd, char** port_buffer) {
	char* path;
	char buffer[R_LEN];
	int file_fd;
	struct stat file_stats;

	Write(server_fd, UPLOAD_FILE, R_LEN);
	Read(server_fd, buffer, R_LEN);

	if(strcmp(buffer, READY_TO_RECEIVE) != 0) {
		perror("Error communicating with server");
		exit(1);
	}

	Write(server_fd, *port_buffer, strlen(*port_buffer) + 1);

	printf("\nEnter the path to the file : \n");
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
	char* user_name;
	char* file_name;
	char message_buffer[MAX_SERVER_RESPONSE_LENGTH];
	char path_buffer[MAX_PATH_LENGTH];
	char port_buffer[6];
	char ip_buffer[16];
	int peer_socket_fd;

	Write(fd, DOWNLOAD_FILE, R_LEN);
	Read(fd, message_buffer, MAX_SERVER_RESPONSE_LENGTH); // ** 1

	if(strcmp(message_buffer, READY_TO_RECEIVE) != 0) {
		perror("Error communicating with server");
		return;
	}

	puts("Enter user to download from");
	user_name = getLine();

	Write(fd, user_name, strlen(user_name) + 1); // ** 2
	Read(fd, message_buffer, MAX_SERVER_RESPONSE_LENGTH); // ** 3

	if(strcmp(message_buffer, READY_TO_RECEIVE) != 0) {
		perror("Error communicating with server");
		return;
	}

	puts("Enter a filename to download :");
	file_name = getLine();

	Write(fd, file_name, strlen(file_name) + 1); // ** 4

	// Get IP
	Read(fd, ip_buffer, MAX_SERVER_RESPONSE_LENGTH); // ** 5
	if(strcmp(message_buffer, FILE_DOES_NOT_EXIST) == 0) {
		perror("File does not exist");
		return;
	}
	Write(fd, DATA_RECEIVED, R_LEN); // ** 6

	// Get port number
	Read(fd, port_buffer, 6); // ** 7
	Write(fd, DATA_RECEIVED, R_LEN); // ** 8

	// Get path
	Read(fd, path_buffer, MAX_PATH_LENGTH); // ** 9
	Write(fd, DATA_RECEIVED, R_LEN); // ** 10

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

	// ***** CONNECTING TO PEER **** //

	connect(peer_socket_fd, (struct sockaddr*)&peer_addr, 
		sizeof(peer_addr));

	int file_fd;
	int read_length;
	char downloaded_file_path[MAX_PATH_LENGTH];
	void* download_buffer[MAX_DATA_BUFFER_SIZE];
	
	strcpy(downloaded_file_path, "../downloads/");
	strcat(downloaded_file_path, file_name);
	
	if((file_fd = open(downloaded_file_path, 
		O_CREAT | O_EXCL | O_RDWR | O_APPEND, 0664)) < 0) {
		perror("Error opening file");
		return;
	}

	Write(peer_socket_fd, path_buffer, strlen(path_buffer) + 1); // ** 1

	// Get file size
	Read(peer_socket_fd, message_buffer, R_LEN); // ** 2
	if(strcmp(message_buffer, BEGIN_DATA_BUFFER_SEND) != 0) {
		perror("Error communicating with peer");
		exit(1);
	}
	Write(peer_socket_fd, READY_TO_RECEIVE, R_LEN);

	while(strcmp(download_buffer, END_DATA_BUFFER_SEND) != 0) {
		memset(download_buffer, '\0', MAX_DATA_BUFFER_SIZE);
		if((read_length = read(peer_socket_fd, 
			download_buffer, MAX_DATA_BUFFER_SIZE)) < 0) { 

			perror("Error reading from peer");
			exit(1);
		}
		printf("in while with : %s\n", download_buffer);

		if(strcmp(download_buffer, END_DATA_BUFFER_SEND) != 0) {
			Write(file_fd, download_buffer, read_length);
		
			// Confirm with peer
			Write(peer_socket_fd, READY_TO_RECEIVE, R_LEN);
		}
	}
	close(file_fd);
	close(peer_socket_fd);
}

void quit(int fd, char* port) {
	char message_buffer[R_LEN];

	Write(fd, QUIT, R_LEN);
	Read(fd, message_buffer, R_LEN);

	if(strcmp(message_buffer, READY_TO_RECEIVE) != 0) {
		perror("Error communicating with server");
		exit(1);
	}

	Write(fd, port, 6);
	Read(fd, message_buffer, R_LEN);

	if(strcmp(USER_UNREGISTERED, message_buffer) == 0) {
		puts("Unregistered successfully");
	}
	else {
		puts("Error unregistering user");
	}
}

void handleCommand(int fd, int *registered, int *deregistering, 
	char** port_buffer) {

	int valid_command = 0;
	char* buffer;

	while(!valid_command) {
		buffer = getLine();
		
		if(strcmp(buffer, "unreg") == 0) {
			quit(fd, *port_buffer);
			valid_command = 1;
		}
		else if(strcmp(buffer, "down") == 0) {
			downloadFile(fd);
			valid_command = 1;
		}
		else if(strcmp(buffer, "up") == 0) {
			uploadFileInfo(fd, port_buffer);
			valid_command = 1;
		}
		else if(strcmp(buffer, "list") == 0) {
			listUsersAndFiles(fd);
			valid_command = 1;
		}
		else if(strcmp(buffer, "quit") == 0) {
			quit(fd, *port_buffer);
			*deregistering = 1;
			valid_command = 1;
		}
		else {
			puts("ERROR : Invalid command");
		}
	}
}
