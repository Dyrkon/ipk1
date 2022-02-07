/*
 * First IPK project
 * project1.c
 * Matěj Mudra
 * xmudra04
 * 7.2. 2022
 */

#include "project1.h"


int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "The port needs to be specified.\n");
		return 1;
	}

	struct Server server;

	if (!legal_port(argv[1], &(server.port))) {
		fprintf(stderr, "Port number contains illegal characters.\n");
		return 1;
	}

	int listenfd, n;
	uint8_t buff[MAXLINE+1];
	uint8_t recvline[MAXLINE+1];

	// Domain is internet, type is tcp socket
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_n_quit("Failed to create new socket.\n");

	// Setting up address
	bzero(&(server.address), sizeof(server.address));
	server.address.sin_family = AF_INET;
	server.address.sin_addr.s_addr = htonl(INADDR_ANY);
	server.address.sin_port = htons(server.port);

	// Associating(binding) socket to address
	if ((bind(listenfd, (const struct sockaddr *) &(server.address), sizeof(server.address))) < 0)
		err_n_quit("Binding error.\n");

	// Start listening on address
	if ((listen(listenfd, 10)) < 0)
		err_n_quit("Listen error");

	for ( ; ; ) {
		// Waiting until connection arrives, then returns "file descriptor" to the connection
		printf("Waiting for a connection on port %d.\n", server.port);
		fflush(stdout);
		signal(SIGINT, signalHandler);
		connfd = accept(listenfd, (struct sockaddr *) NULL, NULL);

		memset(recvline, 0, MAXLINE);

		while ((n = read(connfd, recvline, MAXLINE-1)) > 0) {
			fprintf(stdout, "\n%s\n", recvline);

			if (recvline[n-1] == '\n') {
				break;
			}
			memset(recvline, 0, MAXLINE);
		}

		if (n < 0)
			err_n_quit("Error while reading");

		char out_msg[LINELEN];

		handle_response(recvline, out_msg, LINELEN);

		snprintf((char *)buff, sizeof(buff), "HTTP/1.0 200 OK\r\n\r\n%s", out_msg);

		write(connfd, (char *)buff, strlen((char *)buff));
		close(connfd);
	}
}


bool legal_port(char port[], int *parsed_port) {
	for (int i = 0; port[i] != '\0'; ++i) {
		if (!isdigit(port[i])) {
			return false;
		}
	}

	*parsed_port = strtol(port, NULL, 10);

	return true;
}

void err_n_quit(char *msg) {
	fprintf(stderr, "%s", msg);
	exit(1);
}

void signalHandler(int signalValue)
{
	close(connfd);
	exit(0);
}

int get_request_type(char msg[]) {
	char command[MAXLINE];

	int i = 5;
	for (; !isspace(msg[i]); ++i) {
		command[i-5] = msg[i];
	}
	command[i-5] = '\0';

	if (!strcmp(command, "hostname")) {
		return 0;
	}
	else if (!strcmp(command, "cpu-name")){
		return 1;
	}
	else if (!strcmp(command, "load")){
		return 2;
	}
	else {
		return -1;
	}
}

void handle_response(char *response, char *out, int out_size) {
	switch (get_request_type(response)) {
		case 0:
			get_hostname(LINELEN, out);
			break;
		case 1:
			get_cpu_id(out_size, out);
			break;
		case 2:
			printf("load\n");
			break;
		default:
			fprintf(stderr, "Unknown command\n");
			break;
	}
}

void get_cpu_id(int size, char *arr) {
	FILE *cpu_info = fopen("/proc/cpuinfo", "r");

	if (cpu_info == NULL) {
		err_n_quit("Failed to get CPU info");
	}

	int c;

	while (1) {
		for (int i = 0; i < 4; ) {
			c = getc(cpu_info);
			if (c == '\n')
				i++;
		}

		while (getc(cpu_info) != ':') {}

		getc(cpu_info); c = 0;

		int i = 0;
		for (; (c = getc(cpu_info)) != '\n'; i++) {
			if (i > size - 1) {
				err_n_quit("CPU name is too long\n");
			}
			arr[i] = c;
		}

		arr[i+1] = '\0';

		break;
	}
}

void get_hostname(int size, char *arr) {
	FILE *hostname = fopen("/etc/hostname", "r");

	if (hostname == NULL) {
		err_n_quit("Failed to get hostname");
	}

	fscanf(hostname, "%s", arr);
}
