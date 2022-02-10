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

	if (!is_legal_port(argv[1], &(server.port))) {
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


bool is_legal_port(char port[], int *parsed_port) {
	for (int i = 0; port[i] != '\0'; ++i) {
		if (!isdigit(port[i])) {
			return false;
		}
	}

	*parsed_port = (int)strtol(port, NULL, 10);

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

int get_request_type(const char msg[]) {
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
			get_hostname2(out, HOST_NAME_LENGTH);
			break;
		case 1:
			get_cpu_id(out, out_size);
			break;
		case 2:
			get_cpu_load(out, out_size);
			break;
		default:
			fprintf(stderr, "Unknown command\n");
			break;
	}
}

void get_cpu_id(char *arr, int size) {
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
			arr[i] = (char)c;
		}

		arr[i] = '\0';

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

int get_hostname2(char *out, int size) {
	struct addrinfo hints, *info, *p;
	int gai_result;

	char hostname[HOST_NAME_LENGTH];
	hostname[HOST_NAME_LENGTH-1] = '\0';
	gethostname(hostname, HOST_NAME_LENGTH-1);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_CANONNAME;

	if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 0) {
		err_n_quit("Failed to obtain hostname");
	}

	for(p = info; p != NULL; p = p->ai_next) {
		if (p->ai_canonname != NULL) {
			memcpy(out, p->ai_canonname, size);
		}
	}

	freeaddrinfo(info);

	return 0;
}

int get_cpu_load(char *out, int size)
{
	char str[100];
	const char d[2] = " ";
	char* token;
	int i = 0, times = 2, lag = 1;
	long int sum = 0, idle, lastSum = 0,lastIdle = 0;
	long double idleFraction;

	while(times>0){
		FILE* fp = fopen("/proc/stat","r");
		i = 0;
		fgets(str,100,fp);
		fclose(fp);
		token = strtok(str,d);
		sum = 0;
		while(token!=NULL){
			token = strtok(NULL,d);
			if(token!=NULL){
				sum += strtol(token, NULL, 10);

				if(i==3)
					idle = strtol(token, NULL, 10);

				i++;
			}
		}
		idleFraction = 100 - (double)(idle-lastIdle)*100.0/(double)(sum-lastSum);
		snprintf(out, size, "%d%%", (int)idleFraction);

		lastIdle = idle;
		lastSum = sum;


		times--;
		sleep(lag);
	}

	return 0;
}
