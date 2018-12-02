#ifndef COMMUNICATION_MANAGER_H
#define COMMUNICATION_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include "time.h"

#include "session_list.h"
#include "session.h"
#include "lobby.h"
#include "client_handle_container.h"
#include "message_proccessing.h"
#include "protocol.h"
#include "file_processor.h"

void handle_client(client_handle_container* container);

void handle_client_connect(int client_socket, lobby* actual_lobby);

void new_session_request(client* actual_client, session_list* actual_session_list);

void execute_client_action(client* actual_client, int action, char* params, client_handle_container* container);

void new_game_request(client* actual_client, session_list* actual_session_list);

void pexeso_reveale_request(client* actual_client, char* params, session_list* actual_session_list);

void send_message_both_clients_in_session(int action, char* params,session* actual_session);

void send_client_message(int client_socket, int action, char* params);

int is_raw_message_login_to_lobby_request(char* raw_message);

#endif
