#ifndef COMMUNICATION_MANAGER_H
#define COMMUNICATION_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include "session.h"
#include "lobby.h"
#include "client_handle_container.h"
#include "message_proccessing.h"

void *handle_client(void *arg);

void handle_client_connect(int client_socket, lobby* actual_lobby);


#endif
