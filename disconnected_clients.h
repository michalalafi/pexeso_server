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

#endif
