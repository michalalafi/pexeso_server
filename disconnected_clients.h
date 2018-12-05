#ifndef DISCONNECTED_CLIENTS_H
#define DISCONNECTED_CLIENTS_H
#include "time.h"
#include "client.h"


typedef struct DISCONNECTED_CLIENT{
    client* client;
    time_t time_of_disconnected;
    struct DISCONNECTED_CLIENT* next;
    struct DISCONNECTED_CLIENT* previous;
}disconnected_client;


typedef struct DICONNECTED_CLIENTS_LIST{
    disconnected_client* first;
    disconnected_client* last;
}disconnected_clients_list;

disconnected_client* create_disconnected_client(client* actual_client);
disconnected_clients_list* create_disconnected_clients_list();
void add_disconnected_client_to_disconnected_clients_list(disconnected_client* new_disconnected_client, disconnected_clients_list* actual_disconnected_clients_list);

void remove_disconnected_client_from_disconnected_clients_list(disconnected_client* disconnected_client_to_remove, disconnected_clients_list* actual_disconnected_clients_list);
void print_disconnected_clients(disconnected_clients_list* actual_disconnected_clients_list);
#endif
