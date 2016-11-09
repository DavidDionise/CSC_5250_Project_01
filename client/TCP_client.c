#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <netdb.h>

#include "client_util/util.h"

#define MAX_READ_LENGTH 2048

int main(int argc, char* argv[]) {
	if (argc < 3) {
		perror("Missing hostname and/or port number\n");
		exit(1);
	}

	pid_t pid;

	int socket_fd;
	int port_number = atoi(argv[2]);
	struct sockaddr_in server_addr;
	
	char read_buffer[MAX_READ_LENGTH];
	int length, read_length;

	int deregistering;
	
	/*if(pid = fork() < 0) {
		perror("Error forking new process");
		exit(1);	
	}

	// Child process
	else if(pid == 0) {
		handlePeer();
	}*/
	
	while(!deregistering) {
		socket_fd = socket(AF_INET, SOCK_STREAM, 0);	

		// Initialize socket address
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(port_number);
		if(inet_aton(getIP(argv[1]), &server_addr.sin_addr.s_addr) == 0) {
			perror("Error initializing server address\n");
			exit(1);
		}

		// Connect socket
		connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

		printMenu();

		handleCommand(socket_fd, &deregistering);

		close(socket_fd);
	}

	close(socket_fd);
	return 0;
}


