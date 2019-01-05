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
* actual_disconnected_clients_list: aktualni list odpojenych clientu
* actual_sounds_folder_path: cesta ke slozce kde jsou zvuky na pexeso
*
* returns: novy kontejner s vlozenimi parametry
*/
client_handle_container* create_client_handle_container(lobby* actual_lobby, session_list* actual_session_list, int actual_client_socket, char* client_message, disconnected_clients_list* actual_disconnected_clients_list,
char* actual_sounds_folder_path){
    if(actual_lobby == NULL || actual_session_list == NULL || actual_session_list == NULL || client_message == NULL || actual_sounds_folder_path == NULL){
        log_error("CREATE CLIENT HANDLE CONTAINER - Not valid params");
		return NULL;
	}
	client_handle_container* new_client_handle_container = (client_handle_container*)malloc(sizeof(client_handle_container));
	if(new_client_handle_container == NULL){
		log_error("CREATE CLIENT HANDLE CONTAINER - Creating container failed!");
		return NULL;
	}
	new_client_handle_container->lobby = actual_lobby;
	new_client_handle_container->session_list = actual_session_list;
	new_client_handle_container->client_socket = actual_client_socket;
	new_client_handle_container->disconnected_clients_list = actual_disconnected_clients_list;
    // Cesta ke slozce se zvuky
	new_client_handle_container->sounds_folder_path = (char*) malloc((strlen(actual_sounds_folder_path)+1) * sizeof(char));
	strcpy(new_client_handle_container->sounds_folder_path, actual_sounds_folder_path);
	replace_new_line_char(new_client_handle_container->sounds_folder_path);
    // Clientova zprava
    new_client_handle_container->message = (char*) malloc((strlen(client_message)+1) * sizeof(char));
	strcpy(new_client_handle_container->message, client_message);
	replace_new_line_char(new_client_handle_container->message);

	return new_client_handle_container;
}
/*
* Function: free_client_handle_container
* ------------------------
* Uvolni prepravku pro parametry
*
* actual_container: aktualni prepravka ktera se ma uvolnit
*
* returns: novy kontejner s vlozenimi parametry
*/
void free_client_handle_container(client_handle_container* actual_container){
    if(actual_container == NULL){
        log_error("FREE CLIENT HANDLE CONTAINER - Not valid params");
        return;
    }

    free(actual_container->message);
    free(actual_container);
}
