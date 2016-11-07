#include "util.h"

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
	puts("******* FTP CLIENT ********");
	printf("\n\n");
	puts("Enter one of the following commands :");
	printf("\n");

	puts("reg ..... Register account with server");
	puts("dereg ... Deregister account");
	puts("down .... Download file from another user");
	puts("up ...... Upload file to make available for downloading");
	puts("list .... List available files in server");
	puts("quit .... Quit ftp");
	puts("help .... Re-print instructins");
	printf("\n");
	puts("****************************");
}

void handlePeer() {
	puts("in handlePeer");
}

void handleCommand() {
	int valid_command = 0;
	char buffer[MAX_COMMAND_LENGTH];

	bzero(buffer, MAX_COMMAND_LENGTH);

	while(!valid_command) {
		if(read(STDIN_FILENO, buffer, MAX_COMMAND_LENGTH) < 0) {
			perror("Error reading command");
			exit(1);
		}

		if(strcmp(buffer, "reg") == 0) {
			puts("reg command");
			valid_command = 1;
		}
		else if(strcmp(buffer, "dereg") == 0) {
			puts("dereg command");
			valid_command = 1;
		}
		else if(strcmp(buffer, "down") == 0) {
			puts("down command");
			valid_command = 1;
		}
		else if(strcmp(buffer, "up") == 0) {
			puts("up command");
			valid_command = 1;
		}
		else if(strcmp(buffer, "list") == 0) {
			puts("list command");
			valid_command = 1;
		}
		else if(strcmp(buffer, "quit") == 0) {
			puts("quit command");
			valid_command = 1;
		}
		else if(strcmp(buffer, "help") == 0) {
			puts("help command");
			valid_command = 1;
		}
	}
}
