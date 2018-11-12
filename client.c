#include "client.h"

/*
* Function:
* ------------------------
* Comment of function
* Params
* returns:
*/

/*
* Function: create_client
* ------------------------
* Vytvori noveho clienta a nastavi mu atributy
*
* client_socket: clientovo socket
* client_name: clientovo jmeno
* unique_id: id clienta
*
* returns: noveho clienta
*/
client* create_client(int client_socket,char* client_name, int unique_id){
	client* new_client = (client*) malloc(sizeof(client));
	if(new_client == NULL){
        printf("CLIENT ERROR - Creating client failed! \n");
		return NULL;
	}
	new_client->name = client_name;
	new_client->socket = client_socket;
	new_client->next = NULL;
	new_client->previous = NULL;
	new_client->id = unique_id;


	return new_client;
}



