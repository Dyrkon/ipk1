/*
 * First IPK project
 * project1.h
 * Matěj Mudra
 * xmudra04
 * 7.2. 2022
 * */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>

#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SIZE 1024

#define MAXLINE 1024

#define LINELEN 100

int connfd;

bool legal_port(char port[], int *parsed_port);

void err_n_quit(char *msg);

struct Server {
	int port;
	int backlog;

	struct sockaddr_in address;
};

void signalHandler(int signalValue);

int get_request_type(char msg[]);

void handle_response(char *response, char *out, int out_size);

void get_cpu_id();

void get_hostname(int size, char *arr);