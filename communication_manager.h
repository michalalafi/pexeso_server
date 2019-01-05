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
#include "disconnected_clients.h"
#include "log.h"
#include "game.h"

#define DISCONECTED_TIME_VAL 10

void handle_client(client_handle_container* container);

void handle_client_connect(int client_socket, lobby* actual_lobby);

void handle_client_disconnect(int client_socket, lobby* actual_lobby, disconnected_clients_list* actual_disconnected_clients_list, session_list* actual_session_list);

void handle_client_correct_disconnect(int client_socket, lobby* actual_lobby, session_list* actual_session_list);

void handle_client_reconnect(int client_socket, message* client_message, disconnected_clients_list* actual_disconnected_clients_list, lobby* actual_lobby, session_list* actual_session_list);

void new_session_request(client* actual_client, session_list* actual_session_list);

void handle_disconnected_clients_list(session_list* actual_session_list, disconnected_clients_list* actual_disconnected_clients_list);

void throw_away_connection_with_client(disconnected_client* actual_disconnected_client, session_list* actual_session_list, disconnected_clients_list* actual_disconnected_clients_list);

void handle_client_remove_from_session(client* actual_client, session* actual_session, session_list* actual_session_list);

void execute_client_action(client* actual_client, int action, char* params, client_handle_container* container, char* sounds_folder_path, message* client_message);

void new_game_request(client* actual_client, char* params, session_list* actual_session_list, char* sounds_folder_path);

void pexeso_reveale_request(client* actual_client, char* params, session_list* actual_session_list);

void session_id_request(client* actual_client, session_list* actual_session_list);

void client_name_request(client* actual_client);

void number_of_clients_online_request(client* actual_client, lobby* actual_lobby);

void is_alive_request(client* actual_client);

void send_who_is_on_turn_both_clients_in_session(session* actual_session);

void send_message_both_clients_in_session(int action, char* params,session* actual_session);

void send_client_message(int client_socket, int action, char* params);

void send_client_message_with_int_param(int client_socket, int action, int param);

void send_client_message_with_ints_params(int client_socket, int action, int param1, int param2);

void send_message_both_clients_in_session_with_int_param(session* actual_session, int action, int param);

void send_message_both_clients_in_session_with_ints_params(session* actual_session, int action, int param1, int param2);

int is_raw_message_login_to_lobby_request(char* raw_message);

#endif
