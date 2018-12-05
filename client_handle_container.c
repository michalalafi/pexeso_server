#include "client_handle_container.h"

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
client_handle_container* create_client_handle_container(lobby* actual_lobby, session_list* actual_session_list, int actual_client_socket, char* client_message, disconnected_clients_list* actual_disconnected_clients_list){
    if(actual_lobby == NULL || actual_session_list == NULL || actual_session_list == NULL ){
        printf("CLIENT HANDLE CONTAINER ERROR - Creating client handle container failed, arguments not valid! \n");
		return NULL;
	}
	client_handle_container* new_client_handle_container = (client_handle_container*)malloc(sizeof(client_handle_container));
	if(new_client_handle_container == NULL){
		printf("CLIENT HANDLE CONTAINER ERROR - Creating client handle container failed! \n");
		return NULL;
	}
	new_client_handle_container->lobby = actual_lobby;
	new_client_handle_container->session_list = actual_session_list;
	new_client_handle_container->client_socket = actual_client_socket;
	new_client_handle_container->disconnected_clients_list = actual_disconnected_clients_list;


    new_client_handle_container->message = (char*) malloc(strlen(client_message) * sizeof(client_message));
	strcpy(new_client_handle_container->message, client_message);
	replace_new_line_char(new_client_handle_container->message);

	return new_client_handle_container;
}
