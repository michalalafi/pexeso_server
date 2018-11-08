CC=gcc

all:	clean server

server: main.c
	${CC} -o main main.c

clean:
	rm -f main

