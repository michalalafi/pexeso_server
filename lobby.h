#ifndef LOBBY_H
#define LOBBY_H

#include <stdio.h>
#include <stdlib.h>
#include "client.h"

typedef struct LOBBY{
	client* first;
	client* last;
}lobby;

lobby* create_lobby();

void add_client_to_lobby(client* new_client, lobby* actual_lobby);

void remove_client_from_lobby(client* client_to_remove, lobby* actual_lobby);

client* find_client_by_id(int client_id, lobby* actual_lobby);

int get_new_client_unique_id(lobby* actual_lobby);

void print_clients(lobby* actual_lobby);

#endif
