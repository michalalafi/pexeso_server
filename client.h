#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>

#define CLIENT_PREFIX "guest"

typedef struct CLIENT
{
	int socket;
	int id;
	char* name;
	struct CLIENT* next;
	struct CLIENT* previous;
}client;

client* create_client(int client_socket, int unique_id);

void free_client(client* actual_client);

#endif
