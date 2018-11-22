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
void handle_client(client_handle_container* container){
	printf("Spusteni obsluhy clienta\n");
    if(container == NULL)
    {
        printf("Neco se pokazilo u conteineru /n");
        return;
    }
    printf("Container obsahuje: client_socket = %d | message = '%s'\n",container->client_socket,container->message);

    message* client_message = extract_message(container->message);
    if(client_message == NULL){
        printf("NOT VALID MESSAGE \n");
        return;
    }

    //printf("Struct message: \n message->client_id = %d \n message->action = %d \n message->params = %s \n", client_message->client_id,client_message->action,client_message->params);

    client* actual_client  = find_client_by_id(client_message->client_id, container->lobby);
    if(actual_client == NULL){
        printf("Client nenalezen \n");
        return;
    }
    else{
        printf("Client nalezen \n");
    }

    printf("Aktualni zpracovavany client: \n");
    printf("    client->id = %d \n  client->name = %s \n    client->socket = %d\n",actual_client->id, actual_client->name,actual_client->socket);

    execute_client_action(actual_client,client_message->action,client_message->params,container);


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
	return;
}
void execute_client_action(client* actual_client, int action, char* params, client_handle_container* container){
    printf("EXECEUT ACTION: %d \n",action);
    switch(action){
        case NEW_GAME_REQUEST:
            new_game_request(actual_client,container->session_list);
            break;
        case PEXESO_REVEALED_REQUEST:
            pexeso_reveale_request(actual_client, params, container->session_list);
            break;
    }

}
void new_game_request(client* actual_client, session_list* actual_session_list){
    printf("New game request \n");
    session* actual_session = get_open_session(actual_session_list);
    if(actual_session == NULL){
        printf("Vytvarime novou session \n");
        // Vytvorime novou sessionu
        actual_session = create_session(actual_client,NULL,NULL,get_new_session_unique_id(actual_session_list));
        // Pridame ji do listu
        add_session_to_session_list(actual_session, actual_session_list);
    }
    else{
        //TODO second?
        actual_session->second_client = actual_client;
        char** sounds = get_sounds_for_pexeso("../../sounds",8);
        actual_session->game = create_game(sounds,8);
        printf("Pridelana sessiona: %d, hra \n",actual_session->id);
    }
    //TODO pridat do session listu
    //TODO hru pridat az kdyz se pripoji druhy
    printf("Odpoved \n");
    char buff[1024];
    sprintf(buff,"%s%d","Pridelena sessiona:",actual_session->id);
    write(actual_client->socket,buff,sizeof(buff));

}

void pexeso_reveale_request(client* actual_client, char* params, session_list* actual_session_list){
    printf("Pexeso reveal request \n");
    session* actual_session = get_session_by_client(actual_client, actual_session_list);
    if(actual_session == NULL){
        printf("CLIENT NENI V ZADNE SESSIONE \n");
        return;
    }
    //TODO je sessiana validni?
    int is_on_turn = is_client_on_turn(actual_client, actual_session);
    if(is_on_turn == -1){
        printf("A jejda neco se pokazilo, is on turn vratilo -1\n");
        return;
    }
    if(is_on_turn){

        int pexeso_revealed = convert_string_to_long(params);
        if(pexeso_revealed == -1){
            printf("Nevalidni pexeso\n");
            return;
        }
        int is_pexeso_revealed_valid = isValid(pexeso_revealed, actual_session->game);
        if(is_pexeso_revealed_valid){
            reveal(pexeso_revealed, actual_session->game);
            // Je konec hracova kola?
            if(is_end_of_turn(actual_session->game)){
                if(scored(actual_session->game)){
                    char buff[1024];
                    sprintf(buff,"%s%d","Hrac skoroval:",actual_client->id);
                    send_message_both_clients_in_session(buff,actual_session);
                }
                else{
                    char buff[1024];
                    sprintf(buff,"%s%d","Hrac neskoroval:",actual_client->id);
                    send_message_both_clients_in_session(buff,actual_session);
                }

                if(is_game_over(actual_session->game)){
                    //TODO Smazat hru
                    printf("Konec hry \n");
                }
                else{
                    nextTurn(actual_session->game);
                }
            }
            else{
                printf("Hrac hraje jeste jednou\n");
            }


        }
        else{
            printf("Nevalidni tah\n");
        }
        printf("Jo hrac je na rade bude hrat \n");
    }
    else{
        printf("Hrac neni na rade!\n");
    }
}
void send_message_both_clients_in_session(char* buff,session* actual_session){
    printf("Posilani zpravy obema clientum v session: %s \n",buff);
    write(actual_session->first_client->socket,buff,sizeof(buff));
    write(actual_session->second_client->socket,buff,sizeof(buff));
}
void handle_client_connect(int client_socket, lobby* actual_lobby){
    client* new_client = create_client(client_socket, get_new_client_unique_id(actual_lobby));
    if(new_client == NULL){
        return;
    }
    add_client_to_lobby(new_client,actual_lobby);

    print_clients(actual_lobby);

    char buff[1024];
    sprintf(buff,"%s%d","ID:",new_client->id);
    write(new_client->socket,buff,sizeof(buff));
}
