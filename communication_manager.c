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
        return;
    }
    else
        printf("Container obsahuje: client_socket = %d | message = '%s'/n",container->client_socket,container->message);

    //find_client_by_id();




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
}
