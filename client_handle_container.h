#ifndef CLIENT_HANDLE_CONTAINER_H
#define CLIENT_HANDLE_CONTAINER_H

#include "lobby.h"
#include "session_list.h"
#include "utils.h"
#include "disconnected_clients.h"
#include "log.h"

typedef struct CLIENT_HANDLE_CONTAINER{
	struct LOBBY* lobby;
	struct SESSION_LIST* session_list;
	int client_socket;
	char* message;
	char* sounds_folder_path;
	struct DICONNECTED_CLIENTS_LIST* disconnected_clients_list;
	int pexeso_count;
}client_handle_container;

client_handle_container* create_client_handle_container(lobby* actual_lobby, session_list* actual_session_list, int actual_client_socket, char* client_message, disconnected_clients_list* actual_disconnected_clients_list,
char* actual_sounds_folder_path, int pexeso_count);

void free_client_handle_container(client_handle_container* actual_container);

#endif
