CC=gcc

all: clean make

make:
	${CC} -o main main.c file_processor.c session_list.c client.c game.c test.c client_handle_container.c utils.c lobby.c session.c communication_manager.c message_proccessing.c disconnected_clients.c
clean:

