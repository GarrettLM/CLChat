#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <clargparse.h> //Compile and install from https://github.com/GarrettLM/clarg

#define BUFFER_LENGTH 80
#define MAXPENDING 5

static short port = 22222;

#define ARG_TABLE_SIZE 1
argte arg_table[ARG_TABLE_SIZE] = {
	{"-p", 1, &port, proc_int_arg}
};

void handle_connection(int client_sock);

int main(int argc, char *argv[]) {
	proc_args(argc, argv, arg_table, ARG_TABLE_SIZE);

	int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serv_sock < 0) {
		printf("Failed to create sockedt!\n");
		exit(1);
	}

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);

	if (bind(serv_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		printf("Bind failed!\n");
		exit(1);
	}
	printf("Bind success\n");

	if (listen(serv_sock, MAXPENDING) < 0) {
		printf("Listen fialed!\n");
		exit(1);
	}
	printf("Listen success\n");

	while (1) {
		struct sockaddr_in client_addr;
		socklen_t client_addr_size = sizeof(struct sockaddr_in);
		int client_sock = accept(serv_sock, (struct sockaddr *) &client_addr, &client_addr_size);
		if (client_sock < 0) {
			printf("Accept failed! %d\n", client_sock);
			exit(1);
		}
		printf("Accepted client\n");
		handle_connection(client_sock);
	}
}

void handle_connection(int client_sock) {
	int recv_length;
	recv(client_sock, &recv_length, sizeof(int), 0);
	recv_length = ntohl(recv_length);
	char *username = malloc(recv_length + 1);
	recv(client_sock, username, recv_length, 0);
	username[recv_length] = ':';

	char buffer[BUFFER_LENGTH + 1];

	while (1) {
		recv(client_sock, &recv_length, sizeof(int), 0);
		recv_length = ntohl(recv_length);

		while (recv_length > BUFFER_LENGTH) {
			printf("In the loop\n");
			recv_length -= recv(client_sock, buffer, BUFFER_LENGTH, 0);
			buffer[BUFFER_LENGTH] = '\0';
			send(client_sock, buffer, BUFFER_LENGTH, 0);
		}
		if (recv_length > 0) {
			recv(client_sock, buffer, recv_length, 0);
			buffer[recv_length] = '\0';
			send(client_sock, buffer, recv_length, 0);
		}
		if (!strcmp(buffer, "DONE\n")) break;
	}
	close(client_sock);
	printf("Client handled\n");
}
