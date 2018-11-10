#ifndef CLIENT_H
#define CLIENT_H

typedef struct CLIENT
{
	int socket;
	int id;
	char* name;
	struct CLIENT* next;
	struct CLIENT* previous;
}client;

#include <stdio.h>
#include <stdlib.h>
#include "lobby.h"

client* create_client(int client_socket,char* client_name, int unique_id);

#endif
