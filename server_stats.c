#include "server_stats.h"

/*
* Function: create_client_handle_container
* ------------------------
* Vytvori novy kontejner pro predani parametru vlaknu
*
* actual_lobby: aktualni lobby serveru
* actual_session_list: aktualni session list serveru
* actual_client_socket: socket klienta
* client_message: zprava od klienta
*
* returns: novy kontejner s vlozenimi parametry
*/
server_stats* create_server_stats(lobby* actual_lobby, session_list* actual_session_list, disconnected_clients_list* actual_disconnected_clients_list){
    if(actual_lobby == NULL || actual_session_list == NULL || actual_disconnected_clients_list == NULL){
        log_error("CREATE SERVER STATS - Not valid parms");
        return NULL;
    }

    server_stats* new_server_stats = (server_stats*) malloc(sizeof(server_stats));
    if(new_server_stats == NULL){
        log_error("CREATE SERVER STATS - Creating server stats failed");
        return NULL;
    }
    new_server_stats->lobby = actual_lobby;
    new_server_stats->session_list = actual_session_list;
    new_server_stats->disconnected_clients_list = actual_disconnected_clients_list;
    new_server_stats->request_handled = 0;
    new_server_stats->run = 1;

    log_info("SERVER STATS CREATED");

    return new_server_stats;
}

void handle_admin_request(char* request, server_stats* actual_server_stats){
    log_trace("HANDLE ADMIN REQUEST");
    log_info(">%s", request);
    if(actual_server_stats == NULL || request == NULL){
        log_error("HANDLE ADMIN REQUEST - Not valid params");
        return;
    }

    if(strcmp(request, "q") == 0){
        log_info("Quiting server - wait for cycle end");
        actual_server_stats->run = 0;
    }
    else if(strcmp(request, "stats") == 0){
        print_clients(actual_server_stats->lobby);
        print_session_list(actual_server_stats->session_list);
        print_disconnected_clients(actual_server_stats->disconnected_clients_list);
        log_info("NUMBER OF REQUEST HANDLED: %d", actual_server_stats->request_handled);
    }
    else if(strcmp(request, "clients") == 0){
        print_clients(actual_server_stats->lobby);
    }
    else if(strcmp(request, "sessions") == 0){

    }
    else{
        log_error("Not valid request");
    }

}
