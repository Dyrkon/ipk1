CC=gcc

all: server

server: project1.c
	$(CC) -Wextra -Wall -pedantic project1.c -o hinfosvc

clean:
	rm hinfosvc *.zip

pack:
	zip xmudra04.zip  *.c *.h *.md Makefile
