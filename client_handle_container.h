#ifndef CLIENT_HANDLE_CONTAINER_H
#define CLIENT_HANDLE_CONTAINER_H

#include "lobby.h"
#include "session_list.h"

typedef struct CLIENT_HANDLE_CONTAINER{
	struct LOBBY* lobby;
	struct SESSION_LIST* session_list;
	int client_socket;
}client_handle_container;

client_handle_container* create_client_handle_container(lobby* actual_lobby, session_list* actual_session_list, int actual_client_socket);

#endif
