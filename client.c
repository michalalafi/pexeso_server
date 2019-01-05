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
client* create_client(int client_socket, int unique_id){
	client* new_client = (client*) malloc(sizeof(client));
	if(new_client == NULL){
        log_error("CLIENT ERROR - Creating client failed!");
		return NULL;
	}
	new_client->socket = client_socket;
	new_client->next = NULL;
	new_client->previous = NULL;
	new_client->id = unique_id;
    // Dame clientovi jmeno
    new_client->name = (char*) malloc(256 * sizeof(char));
	sprintf(new_client->name,"%s%d",CLIENT_PREFIX,unique_id);

	return new_client;
}
/*
* Function: free_client
* ------------------------
* Uvolnime clienta
*
* actual_client: client na uvolneni
*
* returns: void
*/
void free_client(client* actual_client){
    if(actual_client == NULL){
        log_error("FREE CLIENT - Not valid params");
        return;
    }
    free(actual_client->name);
    actual_client->next = NULL;
    actual_client->previous = NULL;
    free(actual_client);
}



