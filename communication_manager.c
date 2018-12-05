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
    printf("------CLIENT HANDLE STARTED------\n");
    clock_t t = clock();

    if(container == NULL)
    {
        printf("    CLIENT HANDLE ERROR - No container passed!/n");
        return;
    }
    //printf("    Container obsahuje: client_socket = %d | message = '%s'\n",container->client_socket,container->message);
    //TODO je client v lobby?
    //TODO zprava neprejde pres parsovani bez id
    //Zpracujeme zpravu od clienta
    message* client_message = extract_message(container->message);
    // Neposlal message podle formatu
    if(client_message == NULL){
        // Je message zadost o pripojeni do lobby?
        if(is_raw_message_login_to_lobby_request(container->message)){
            // Je client uz v lobby?
            if(is_client_in_lobby_by_socket(container->client_socket, container->lobby)){
                // Posleme zpravu ze uz je v lobby
                printf("CLIENT ALREADY IN LOBBY\n");
                send_client_message(container->client_socket,ALREADY_LOGGED_IN,"");
            }
            else{
                // Pridame do lobby
                handle_client_connect(container->client_socket, container->lobby);
                printf("    Client pridan do lobby\n");
                //send_client_message(container->client_socket,LOGGED_IN_LOBBY,"");
            }
        }
        else{
            printf("    NOT VALID MESSAGE \n");
            send_client_message(container->client_socket, NOT_VALID_MESSAGE,"");
        }
    }
    // Zprava je ve spravnem formatu, tudiz zpracujeme pozadavek
    else{
        //printf("Struct message: \n message->client_id = %d \n message->action = %d \n message->params = %s \n", client_message->client_id,client_message->action,client_message->params);
        //Zkusime najit clienta podle id ve zprave
        client* actual_client  = find_client_by_id(client_message->client_id, container->lobby);
        if(actual_client == NULL){
            printf("    Client nenalezen \n");
            // Je clientovi odeslana zprava ze neni prihlasen
            send_client_message(container->client_socket, NOT_LOGGED_IN_LOBBY,"");
            return;
        }

      /*  printf("    Client nalezen \n");
        printf("    Aktualni zpracovavany client: \n");
        printf("        client->id = %d \n",actual_client->id);
        printf("        client->name = %s \n",actual_client->name);
        printf("        client->socket = %d\n",actual_client->socket); */

        execute_client_action(actual_client,client_message->action,client_message->params,container);
    }

    t = clock() - t;

    double time_taken = ((double)t)/CLOCKS_PER_SEC;
    print_session_list(container->session_list);
    print_clients(container->lobby);
    printf("------CLIENT HANDLE ENDED WITH TIME: %fs------\n", time_taken);
	return;
}
void handle_client_connect(int client_socket, lobby* actual_lobby){
    log_info("HANDLE CONNECT\n");
    client* new_client = create_client(client_socket, get_new_client_unique_id(actual_lobby));
    if(new_client == NULL){
        return;
    }
    add_client_to_lobby(new_client,actual_lobby);

    send_client_message(client_socket, CLIENTS_NAME_RESPONSE, new_client->name);

    char* id_string = (char*) malloc(sizeof(char) * 10);
    sprintf(id_string,"%d",new_client->id);
    send_client_message(client_socket, CLIENTS_ID_RESPONSE, id_string);
}

void handle_client_disconnect(int client_socket, lobby* actual_lobby, disconnected_clients_list* actual_disconnected_clients_list){
    //Najdeme clienta v lobby
    client* actual_client = find_client_by_socket(client_socket, actual_lobby);
    if(actual_client == NULL){
        //TODO
        return;
    }
    //Odstranimeho z lobby
    remove_client_from_lobby(actual_client, actual_lobby);
    //Vlozime do disconnected_list
    disconnected_client* disconnected_client = create_disconnected_client(actual_client);
    if(disconnected_client == NULL){
        //TODO
        return;
    }
    add_disconnected_client_to_disconnected_clients_list(disconnected_client, actual_disconnected_clients_list);

    //Dame vedet uzivateli pokud je sessiona?

}

void handle_disconnected_clients_list(disconnected_clients_list* actual_disconnected_clients_list){
    disconnected_client* pom = actual_disconnected_clients_list->first;
    print_disconnected_clients(actual_disconnected_clients_list);
    while(pom != NULL){
        double diff_t = difftime(pom->time_of_disconnected, clock());
        log_info("Diff time is %f", diff_t);
        //if(pom->time_of_disconnected )

        pom = pom->next;
    }
    printf("K h disco clients list \n");
}
void execute_client_action(client* actual_client, int action, char* params, client_handle_container* container){
    printf("EXECUTE ACTION: %d \n",action);
    switch(action){
        case NEW_SESSION_REQUEST:
            new_session_request(actual_client, container->session_list);
            break;
        case NEW_GAME_REQUEST:
            new_game_request(actual_client,container->session_list);
            break;
        case PEXESO_REVEALED_REQUEST:
            pexeso_reveale_request(actual_client, params, container->session_list);
            break;
    }

}


/*###########################REQUESTS#########################*/

void new_session_request(client* actual_client, session_list* actual_session_list){
    printf(" NEW SESSION REQUEST\n");
    // Pokud je uz v sessione, tak mu napiseme zpravu
    if(is_client_in_session_list(actual_client, actual_session_list)){
        printf(" USER ALREADY IN SESSION\n");
        send_client_message(actual_client->socket, ALREADY_IN_SESSION,"");
        return;
    }
    printf(" Vytvorime/ pridame sessionu \n");
    session* actual_session = get_open_session(actual_session_list);
    if(actual_session == NULL){
        printf("Nenalezena otevrena sessiona, zalozime novou\n");
        // Vytvorime novou sessionu
        actual_session = create_session(actual_client,NULL,NULL,get_new_session_unique_id(actual_session_list));
        // Pridame ji do listu
        add_session_to_session_list(actual_session, actual_session_list);

        printf("    Vytvorena nova sessiona\n");
        //Posleme zpravu
    }
    else{
        printf("Nalezena otevrena sessiona \n");
        //TODO second?
        //TODO zjistit jaky hrac chybi
        //TODO poslat obema hracum
        actual_session->second_client = actual_client;
        char** sounds = get_sounds_for_pexeso("../../sounds",8);
        //Hru vytvorime az pri requestu?
        //actual_session->game = create_game(sounds,8);
        printf("    Pridelana sessiona: %d, hra \n",actual_session->id);
    }

    //TODO poslat id sessiony
    char* id_session_string = (char*) malloc(sizeof(char) * 10);
    sprintf(id_session_string,"%d",actual_session->id);
    send_client_message(actual_client->socket, SESSION_ID_RESPONSE, id_session_string);
}
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
void new_game_request(client* actual_client, session_list* actual_session_list){
    printf(" NEW GAME REQUEST\n");
    session* actual_session = get_session_by_client(actual_client, actual_session_list);
    if(actual_session == NULL){
        //Posleme clientovi ze jeste neni v sessione
        return;
    }
    if(is_session_open(actual_session)){
        printf("Client musi pockat na dalsiho hrace\n");
        //Posleme ze cekame na dalsiho clienta
        send_client_message(actual_client->socket,WAIT_FOR_PLAYER_TO_JOIN,"");
    }
    else{
        //TODO pokud maji uz zalozenou?
        printf("    Zakladame hru\n");
        char** sounds = get_sounds_for_pexeso("../../sounds",8);
        actual_session->game = create_game(sounds,8);
        //Napiseme ze zacla hra
        send_message_both_clients_in_session(NEW_GAME_BEGIN_RESPONSE,"",actual_session);
    }


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
            char* sound = reveal(pexeso_revealed, actual_session->game);
            if(sound == NULL){
                //Neco se stalo spatne
            }
            // Posleme obema ze byla revealnuta puzzle
            send_message_both_clients_in_session(PEXESO_REVEAL_RESPONSE,sound,actual_session);

            // Je konec hracova kola?
            if(is_end_of_turn(actual_session->game)){
                //TODO jak poslat score?
                //TODO mozna poslat neco jako prvnimu 0:1
                //TODO druhemu 1:0
                if(scored(actual_session->game)){
                    char buff[1024];
                    sprintf(buff,"%s%d","Hrac skoroval:",actual_client->id);
                    send_message_both_clients_in_session(PEXESO_REVEALED_REQUEST,buff,actual_session);
                }
                else{
                    char buff[1024];
                    sprintf(buff,"%s%d","Hrac neskoroval:",actual_client->id);
                    send_message_both_clients_in_session(PEXESO_REVEALED_REQUEST,buff,actual_session);
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

void send_message_both_clients_in_session(int action, char* params,session* actual_session){
    printf("Posilani zpravy obema clientum v session \n");
    send_client_message(actual_session->first_client->socket, action, params);
    send_client_message(actual_session->second_client->socket, action, params);
}

/*
* Function: send_client_message
* ------------------------
* Posle clientovi zpravu s akci a parametrama
*
* client_socket: clientovo socket
* action: cislo akce
* params: parametry
*
* returns: void
*/
void send_client_message(int client_socket, int action, char* params){
    const char message_terminator = '\n';
    char* buff = create_raw_message_for_client(action,params);
    printf("    Odeslana zprava: %s \n",buff);
    send(client_socket, buff, (size_t) strlen(buff),0);
    send(client_socket, &message_terminator, 1,0);
}

int is_raw_message_login_to_lobby_request(char* raw_message){
    long action = convert_string_to_long(raw_message);
    if(action == -1){
        return 0;
    }

    if(action == LOGIN_TO_LOBBY_REQUEST){
        return 1;
    }

    return 0;
}



