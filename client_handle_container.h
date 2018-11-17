#ifndef CLIENT_HANDLE_CONTAINER_H
#define CLIENT_HANDLE_CONTAINER_H

#include "lobby.h"
#include "session_list.h"
#include "utils.h"

typedef struct CLIENT_HANDLE_CONTAINER{
	struct LOBBY* lobby;
	struct SESSION_LIST* session_list;
	int client_socket;
	char* message;
}client_handle_container;

client_handle_container* create_client_handle_container(lobby* actual_lobby, session_list* actual_session_list, int actual_client_socket, char* client_message);

#endif
