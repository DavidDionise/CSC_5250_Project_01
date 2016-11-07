
#include <stdio.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <strings.h>

#include "../util/util.h"

#define MAX_READ_LENGTH 2096


int main(int argc, char* argv[]) {
	if (argc != 2) {
		perror("Must include port number");
		exit(1);
	}

	int socket_fd, new_socket;
	int port_number = atoi(argv[1]);
	pid_t pid;

	struct sockaddr_in  server_addr, client_addr;

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

	int client_addr_length = sizeof(client_addr);

	puts("Server Starting");

	if(listen(socket_fd, 10) < 0) {
		perror("Error initializing listen");
		exit(1);
	}
	
	int len;
	while(1){
		if ((new_socket = accept(socket_fd, (struct sockaddr *)&client_addr, 
		(socklen_t*)&client_addr_length)) < 0) {
			perror("Error accepting client");
			exit(1);
		}

		pid = fork();

		// Child process
		if(pid == 0) {
			char read_buffer[MAX_READ_LENGTH];

			if((len = read(new_socket, read_buffer, MAX_READ_LENGTH)) < 0) {
				perror("Error reading from client\n");
				exit(1);
			}
		
			if ((write(new_socket, r_str, len)) < 0) {
				perror("Error writing to client\n");
				exit(1);
			}
			exit(0);
		}

		// Error forking
		else if(pid < 0)
			perror("Error forking new process.");
	}

	close(socket_fd);
	close(new_socket);
	return 0;
}


