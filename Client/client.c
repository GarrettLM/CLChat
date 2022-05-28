#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define USERNAME_LENGTH 10
#define BUFFER_LENGTH 80

static char *username;
static char *serv_ip;
static short serv_port = 22222;

void set_username();

void proc_args(int argc, char *argv[]) {
	for (int i = 0; i < argc; i++) {
		if (!strcmp(argv[i], "-u")) {
			i++;
			username = argv[i];
		} else if (!strcmp(argv[i], "-p")) {
			i++;
			serv_port = atoi(argv[i]);
		} else if (!strcmp(argv[i], "-h")) {
			i++;
			serv_ip = argv[i];
		}
	}
}

int main(int argc, char *argv[]) {
	proc_args(argc, argv);
	if (username == NULL) set_username();
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	if (serv_ip == NULL) {
		inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr.s_addr);
	} else if (inet_pton(AF_INET, serv_ip, &serv_addr.sin_addr.s_addr) <= 0) {
		printf("Failed to convert %s into IP address\n", serv_ip);
		exit(1);
	}
	serv_addr.sin_port = htons(serv_port);

	if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		printf("Failed to connect to server\n");
		exit(0);
	}

	int send_length = (int) strlen(username);
	send_length = htonl(send_length);
	send(sock, &send_length, sizeof(int), 0);
	send_length = ntohl(send_length);
	send(sock, username, send_length, 0);

	char buffer[BUFFER_LENGTH + 1];
	do {
		fgets(buffer, BUFFER_LENGTH, stdin);
		send_length = strlen(buffer);
		send_length = htonl(send_length);
		send(sock, &send_length, sizeof(int), 0);
		send_length = ntohl(send_length);
		send(sock, buffer, send_length, 0);
	} while (strcmp(buffer, "DONE\n"));

	int recv_length;
	while (1) {
		recv_length = recv(sock, buffer, BUFFER_LENGTH, 0);
		if (recv_length == 0) break;
		buffer[recv_length] = '\0';
		printf("%s", buffer);
	}

	close(sock);
	exit(0);
}

void set_username() {
	printf("Enter a username no longer than %d characters long: ", USERNAME_LENGTH);
	username = malloc(USERNAME_LENGTH + 2);
	fgets(username, USERNAME_LENGTH, stdin);
	for (int i = 0; i < USERNAME_LENGTH; i++) {
		if (username[i] == '\n') {
			username[i] = '\0';
			break;
		}
	}
}
