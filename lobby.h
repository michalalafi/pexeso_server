#ifndef LOBBY_H
#define LOBBY_H

#include <stdio.h>
#include <stdlib.h>
#include "client.h"
#include "log.h"

typedef struct LOBBY{
    int number_of_connected_clients;
	client* first;
	client* last;
}lobby;

lobby* create_lobby();

void add_client_to_lobby(client* new_client, lobby* actual_lobby);

void remove_client_from_lobby(client* client_to_remove, lobby* actual_lobby);

client* find_client_by_id(int client_id, lobby* actual_lobby);

client* find_client_by_socket(int client_socket, lobby* actual_lobby);

int get_number_of_clients_online(lobby* actual_lobby);

int get_new_client_unique_id(lobby* actual_lobby);

int is_client_in_lobby_by_socket(int socket,lobby* actual_lobby);

void print_clients(lobby* actual_lobby);

#endif
