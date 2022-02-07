CC=gcc

all: server

server: project1.c
	$(CC) project1.c -o hinfosvc