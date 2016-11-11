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

void handlePeer() {
	puts("in handlePeer");
}

void registerUser(int fd) {
	char buffer[MAX_SERVER_RESPONSE_LENGTH];

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

		if(strcmp(buffer, USER_NAME_REGISTERED) == 0) {
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
	void *buffer[MAX_DATA_BUFFER_SIZE];

	puts("a");
	Write(fd, LIST_AVAILABLE_FILES, R_LEN);
	Read(fd, buffer, MAX_DATA_BUFFER_SIZE);
puts("b");

	if(strcmp(buffer, BEGIN_DATA_BUFFER_SEND) != 0) {
		perror("Error reading data from server");
		exit(1);
	}
puts("c");
Write(fd, "cool", 5);
	Read(fd, buffer, MAX_DATA_BUFFER_SIZE);
puts("d");
	if(strcmp(buffer, END_DATA_BUFFER_SEND) == 0) {
		puts("No users in the system");
		return;
	}

puts("e");
	if(strcmp(buffer, END_DATA_BUFFER_SEND) == 0) {
		puts("No users in system");
	}
	else {
		puts("f");
		while(strcmp(buffer, END_DATA_BUFFER_SEND) != 0) {
			printf("  %s\n", buffer);
			Read(fd, buffer, MAX_DATA_BUFFER_SIZE);
		}
	}
}

void handleCommand(int fd, int * deregistering) {
	int valid_command = 0;
	char buffer[MAX_COMMAND_LENGTH];

	while(!valid_command) {
		Read(STDIN_FILENO, buffer, MAX_COMMAND_LENGTH);
		
		if(strcmp(buffer, "reg\n") == 0) {
			registerUser(fd);
			valid_command = 1;
		}
		else if(strcmp(buffer, "unreg\n") == 0) {
			unregisterUser(fd);
			valid_command = 1;
		}
		else if(strcmp(buffer, "down\n") == 0) {
			puts("down command");
			valid_command = 1;
		}
		else if(strcmp(buffer, "up\n") == 0) {
			puts("up command");
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
		else if(strcmp(buffer, "help\n") == 0) {
			puts("help command");
			valid_command = 1;
		}
		else {
			puts("ERROR : Invalid command");
			printMenu();
		}
	}
}
