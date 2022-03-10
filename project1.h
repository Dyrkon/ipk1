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
#include <netdb.h>

#define HOST_NAME_LENGTH 255

#define MAXLINE 1024

#define LINELEN 200

int connfd;

enum request{HOSTNAME, CPU_ID, LOAD};

bool is_legal_port(char port[], int *parsed_port);

void err_n_quit(char *msg);

void signalHandler();

int get_request_type(unsigned char msg[]);

int handle_response(unsigned char *response, char *out, int out_size);

void get_cpu_id(char *arr, int size);

int get_hostname(char *out, int size);

int get_cpu_load(char *out, int size);
