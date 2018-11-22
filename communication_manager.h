#ifndef COMMUNICATION_MANAGER_H
#define COMMUNICATION_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include "session_list.h"
#include "session.h"
#include "lobby.h"
#include "client_handle_container.h"
#include "message_proccessing.h"
#include "protocol.h"
#include "file_processor.h"
#include <unistd.h>

void handle_client(client_handle_container* container);

void handle_client_connect(int client_socket, lobby* actual_lobby);

void execute_client_action(client* actual_client, int action, char* params, client_handle_container* container);

void new_game_request(client* actual_client, session_list* actual_session_list);

void pexeso_reveale_request(client* actual_client, char* params, session_list* actual_session_list);

void send_message_both_clients_in_session(char* buff,session* actual_session);


#endif
