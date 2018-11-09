#include "client_handle_container.h"

client_handle_container* create_client_handle_container(lobby* actual_lobby, session_list* actual_session_list, int actual_client_socket, char* client_message){
	client_handle_container* new_client_handle_container = (client_handle_container*)malloc(sizeof(client_handle_container));
	if(new_client_handle_container == NULL){
		perror("CLIENT HANDLE CONTAINER ERROR - Creating client handle container failed!");
		return NULL;
	}
	new_client_handle_container->lobby = actual_lobby;
	new_client_handle_container->session_list = actual_session_list;
	new_client_handle_container->client_socket = actual_client_socket;
	new_client_handle_container->message = client_message;

	printf("CLIENT HANDLE CONTAINER CREATED \n");
	return new_client_handle_container;
}
