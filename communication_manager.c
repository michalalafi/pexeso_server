#include "communication_manager.h"

/*
* Function: handle_client
* ------------------------
* Zpracuje zpravu od klienta
*
* arg: argument, ktery obsahuje client_handle_container
*
* returns: void
*/
void *handle_client(void *arg){
	printf("Spusteni obsluhy clienta\n");
    client_handle_container* container = (client_handle_container*) arg;
    if(container == NULL)
    {
        printf("Neco se pokazilo u conteineru /n");
        return (void*) NULL;
    }
    printf("Container obsahuje: client_socket = %d | message = '%s'\n",container->client_socket,container->message);

    message* client_message = extract_message(container->message);
    if(client_message == NULL){
        printf("NOT VALID MESSAGE \n");
    }

    printf("Struct message: \n message->client_id = %d \n message->action = %d \n message->params = %s \n", client_message->client_id,client_message->action,client_message->params);

    client* actual_client  = find_client_by_id(client_message->client_id, container->lobby);
    if(actual_client == NULL){
        printf("Client nenalezen \n");
        return (void*) NULL;
    }
    else{
        printf("Client nalezen \n");
    }

    printf("Aktualni zpracovavany client: \n");
    printf("POMMMMMMMMMMMMM NAME = %s \n",actual_client->name);
    //printf("    client->id = %d \n  client->name = %s \n    client->socket = %d\n",actual_client->id/*,actual_client->name,actual_client->socket*/);




	/*int client_sock =
	char buff[1024];
	int nbytes;

	//pretypujem parametr z netypoveho ukazate na ukazatel na int
	client_sock = *(int *) arg;

	while(1){
		if ((recv(client_sock, &buff, sizeof(buff), 0))< 0) {
			perror("Error cteni zpravy od clienta!");
			close(client_sock);
			break;
		}
		else
		{
			printf("Client: %s \n",buff);
		}
	} */
	return (void*) NULL;
}
void handle_client_connect(int client_socket, lobby* actual_lobby){
    client* new_client = create_client(client_socket, get_new_client_unique_id(actual_lobby));
    if(new_client == NULL){
        return;
    }
    add_client_to_lobby(new_client,actual_lobby);

    print_clients(actual_lobby);
}
