#include "client.h"

client* create_client(int client_socket,char* client_name){
	client* new_client = (client*) malloc(sizeof(client));
	if(new_client == NULL){
		return NULL;
	}
	new_client->name = client_name;
	new_client->socket = client_socket;
	new_client->next = NULL;
	new_client->previous = NULL; 
	
	
	return new_client;
}
