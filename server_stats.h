#ifndef SERVER_STATS_H
#define SERVER_STATS_H

#include "lobby.h"
#include "session_list.h"
#include "disconnected_clients.h"
#include "log.h"

typedef struct SERVER_STATS{
	struct LOBBY* lobby;
	struct SESSION_LIST* session_list;
	struct DICONNECTED_CLIENTS_LIST* disconnected_clients_list;
    int request_handled;
    int run;

}server_stats;

server_stats* create_server_stats(lobby* actual_lobby, session_list* actual_session_list, disconnected_clients_list* actual_disconnected_clients_list);

void handle_admin_request(char* request, server_stats* actual_server_stats);

#endif
