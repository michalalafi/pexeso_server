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
    log_trace("------CLIENT HANDLE STARTED");
    clock_t t = clock();

    if(container == NULL)
    {
        log_error("CLIENT HANDLE ERROR - Not valid params!");
        return;
    }
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
                log_info("Client pridan do lobby\n");
            }
        }
        else{
            log_info("Client send - NOT VALID MESSAGE");
            send_client_message(container->client_socket, NOT_VALID_MESSAGE,"");
        }
    }
    // Zprava je ve spravnem formatu, tudiz zpracujeme pozadavek
    else{
        //Zkusime najit clienta podle id ve zprave
        client* actual_client  = find_client_by_id(client_message->client_id, container->lobby);
        if(actual_client == NULL){
            log_info("Client nenalezen");
            // Je clientovi odeslana zprava ze neni prihlasen
            send_client_message(container->client_socket, NOT_LOGGED_IN_LOBBY,"");
            return;
        }
        else{
            execute_client_action(actual_client,client_message->action,client_message->params,container);
        }
    }

    t = clock() - t;

    double time_taken = ((double)t)/CLOCKS_PER_SEC;
    print_session_list(container->session_list);
    print_clients(container->lobby);
    log_trace("------CLIENT HANDLE ENDED WITH TIME: %fs\n", time_taken);
	return;
}
void handle_client_connect(int client_socket, lobby* actual_lobby){
    log_trace("HANDLE CONNECT\n");
    client* new_client = create_client(client_socket, get_new_client_unique_id(actual_lobby));
    if(new_client == NULL){
        log_error("HANDLE CLIENT CONNECT - Create client failed");
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
        log_error("HANDLE CLIENT DISCONNECT - Client is not in lobby");
        return;
    }
    //Odstranimeho z lobby
    remove_client_from_lobby(actual_client, actual_lobby);
    //Vlozime do disconnected_list
    disconnected_client* disconnected_client = create_disconnected_client(actual_client);
    if(disconnected_client == NULL){
        log_error("HANDLE CLIENT DISCONNECT - Creating disconnected client failed");
        return;
    }
    add_disconnected_client_to_disconnected_clients_list(disconnected_client, actual_disconnected_clients_list);
    log_info("Client added to disconnected clients list");
}

void handle_disconnected_clients_list(session_list* actual_session_list, disconnected_clients_list* actual_disconnected_clients_list){
    log_trace("HANDLE DISCONNECTED CLIENTS LIST");
    disconnected_client* pom = actual_disconnected_clients_list->first;
    print_disconnected_clients(actual_disconnected_clients_list);
    time_t now_t = time(0);
    while(pom != NULL){
        double diff_t = difftime(now_t, pom->time_of_disconnected);
        disconnected_client* pom_to_throw_away = pom;
        pom = pom->next;
        if(diff_t > LOGGED_OUT_TIMEOUT){
            throw_away_connection_with_client(pom_to_throw_away,actual_session_list, actual_disconnected_clients_list);
        }
    }
}
void throw_away_connection_with_client(disconnected_client* actual_disconnected_client, session_list* actual_session_list, disconnected_clients_list* actual_disconnected_clients_list){
        log_trace("Client: %s will be thrown away",actual_disconnected_client->client->name);
        //Odebereme ho z listu
        remove_disconnected_client_from_disconnected_clients_list(actual_disconnected_client, actual_disconnected_clients_list);
        //Ziskame sessionu pokud existuje
        session* actual_session = get_session_by_client(actual_disconnected_client->client, actual_session_list);
        if(actual_session != NULL){
            //Odebereme ho ze sessiony
            remove_client_from_session(actual_disconnected_client->client, actual_session);
            //Pokud sessiona neni prazdna dame vedet oppentovi
            if(!is_session_empty(actual_session)){
                client* last_client_in_session = get_some_client_from_session(actual_session);
                // Pokud existuje napiseme mu zpravu ze opponent opustil hru
                if(last_client_in_session != NULL){
                    send_client_message(last_client_in_session->socket, OPPONENT_LEFT,"");
                }
            }
            else{
                log_info("Session: %d will be deleted", actual_session->id);
                //Smazeme z listu session
                remove_session_from_session_list(actual_session, actual_session_list);
                //Zrusime sessionu
                free_session(actual_session);

            }
        }
        log_info("Client was thrown away");
}
void execute_client_action(client* actual_client, int action, char* params, client_handle_container* container){
    printf("EXECUTE ACTION: %d \n",action);
    switch(action){
        case NEW_SESSION_REQUEST:
            new_session_request(actual_client, container->session_list);
            break;
        case NEW_GAME_REQUEST:
            new_game_request(actual_client, params, container->session_list);
            break;
        case PEXESO_REVEALED_REQUEST:
            pexeso_reveale_request(actual_client, params, container->session_list);
            break;
    }

}


/*###########################REQUESTS#########################*/

void new_session_request(client* actual_client, session_list* actual_session_list){
    log_trace("NEW SESSION REQUEST");
    // Pokud je uz v sessione, tak mu napiseme zpravu
    if(is_client_in_session_list(actual_client, actual_session_list)){
        log_info(" USER ALREADY IN SESSION");
        send_client_message(actual_client->socket, ALREADY_IN_SESSION,"");
        return;
    }
    //Ziskame otevrenou sessionu, pokud neni zadna vytvorime
    session* actual_session = get_open_session(actual_session_list);
    if(actual_session == NULL){
        log_info("Nenalezena otevrena sessiona, zalozime novou");
        // Vytvorime novou sessionu
        actual_session = create_session(actual_client,NULL,NULL,get_new_session_unique_id(actual_session_list));
        // Pridame ji do listu
        add_session_to_session_list(actual_session, actual_session_list);

        log_info("    Vytvorena nova sessiona\n");

        char* id_session_string = (char*) malloc(sizeof(char) * 10);
        sprintf(id_session_string,"%d",actual_session->id);
        send_client_message(actual_client->socket, SESSION_ID_RESPONSE, id_session_string);
        //Posleme ze musi pockat na dalsiho hrace co se pripoji
        send_client_message(actual_client->socket,WAIT_FOR_PLAYER_TO_JOIN,"");
    }
    else{
        log_info("Nalezena otevrena sessiona \n");

        add_client_in_session(actual_client, actual_session);

        char* id_session_string = (char*) malloc(sizeof(char) * 10);
        sprintf(id_session_string,"%d",actual_session->id);
        send_client_message(actual_client->socket, SESSION_ID_RESPONSE, id_session_string);

        printf("    Pridelana sessiona: %d\n",actual_session->id);

        //Dame vedet obema hracum ze jsou spojeni a jestli chteji hrat
        send_message_both_clients_in_session(WANT_TO_PLAY_GAME_RESPONSE,"",actual_session);
    }
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
void new_game_request(client* actual_client, char* params, session_list* actual_session_list){
    log_trace(" NEW GAME REQUEST\n");
    session* actual_session = get_session_by_client(actual_client, actual_session_list);
    if(actual_session == NULL){
        log_error("NEW GAME REQUEST - Client isn't in session");
        //Posleme clientovi ze jeste neni v sessione
        return;
    }
    if(!is_session_valid(actual_session)){
        log_error("NEW GAME REQUEST - Session is not valid");
        return;
    }
    int want_to_play = convert_string_to_long(params);
    if(want_to_play){
        log_info("Client wants to play:%d",want_to_play);
        set_client_wants_play(actual_client, actual_session);

        if(is_session_ready_to_play_game(actual_session)){
            //TODO poslat obema ze se hraje
            //TODO pokud maji uz zalozenou?
            printf("    Zakladame hru\n");
            char** sounds = get_sounds_for_pexeso("../../../sounds",8);
            actual_session->game = create_game(sounds,8);
            if(actual_session->game == NULL){
                log_error("GAME CREATING FAILED");
                return;
            }
            //Napiseme ze zacla hra
            send_message_both_clients_in_session(NEW_GAME_BEGIN_RESPONSE,"",actual_session);
            //Napiseme kdo hraje
            send_who_is_on_turn_both_clients_in_session(actual_session);
        }
        else{
            send_client_message(actual_client->socket, WAIT_FOR_OPPONENT_DECIDE_RESPONSE, "");
        }
    }
    else{
        //TODO napsat druhemu clientovi
    }
    /*
    if(is_session_open(actual_session)){
        printf("Client musi pockat na dalsiho hrace\n");
        //Posleme ze cekame na dalsiho clienta
        send_client_message(actual_client->socket,WAIT_FOR_OPPONENT_DECIDE_RESPONSE,"");
    }
    else{
        //TODO pokud maji uz zalozenou?
        printf("    Zakladame hru\n");
        char** sounds = get_sounds_for_pexeso("../../../sounds",8);
        actual_session->game = create_game(sounds,8);
        if(actual_session->game == NULL){
            log_error("GAME CREATING FAILED");
            return;
        }
        //Napiseme ze zacla hra
        send_message_both_clients_in_session(NEW_GAME_BEGIN_RESPONSE,"",actual_session);
        //Napiseme kdo hraje
        send_who_is_on_turn_both_clients_in_session(actual_session);
    } */


}

void pexeso_reveale_request(client* actual_client, char* params, session_list* actual_session_list){
    log_trace("PEXESO REVEAL REQUEST");
    session* actual_session = get_session_by_client(actual_client, actual_session_list);
    if(actual_session == NULL){
        printf("CLIENT NENI V ZADNE SESSIONE \n");
        return;
    }
    if(!is_session_valid(actual_session) || actual_session->game == NULL){
        log_error("PEXESO REVEAL REQUEST - Not valid session or no game");
        return;
    }
    int is_on_turn = is_client_on_turn(actual_client, actual_session);
    if(is_on_turn == -1){
        log_error("IS ON TURN - Returned -1");
        return;
    }
    if(is_on_turn){
        int pexeso_revealed = convert_string_to_long(params);
        if(pexeso_revealed == -1){
            //TODO POSLAT CHYBU
            printf("Nevalidni pexeso\n");
            return;
        }
        //Zkontrolujeme pexeso
        int is_pexeso_revealed_valid = isValid(pexeso_revealed, actual_session->game);
        //Je validni tah
        if(is_pexeso_revealed_valid){
            char* sound = reveal(pexeso_revealed, actual_session->game);
            if(sound == NULL){
                //Neco se stalo spatne
            }
            // Posleme obema ze byla revealnuta puzzle
            send_message_both_clients_in_session(PEXESO_REVEAL_RESPONSE,sound,actual_session);
            send_message_both_clients_in_session(PEXESO_REVEAL_ID_RESPONSE,params, actual_session);

            // Je konec hracova kola?
            if(is_end_of_turn(actual_session->game)){
                //Pokud skoroval posleme obema skore a posleme obema puzzle ktere byli odhaleny
                if(scored(actual_session->game)){
                    //Posleme skore
                    char p1_score[256];
                    sprintf(p1_score,"%d;%d",actual_session->game->p1_score,actual_session->game->p2_score);
                    send_client_message(actual_session->first_client->socket,SCORE_RESPONSE,p1_score);

                    char p2_score[256];
                    sprintf(p2_score,"%d;%d",actual_session->game->p2_score,actual_session->game->p1_score);
                    send_client_message(actual_session->second_client->socket,SCORE_RESPONSE,p2_score);
                    //Posleme odhalene pexeso
                    char revealed_pexeso[256];
                    sprintf(revealed_pexeso,"%d;%d",actual_session->game->first_reveal,actual_session->game->second_reveal);
                    send_message_both_clients_in_session(SUCCESFULLY_REVEALED_PEXESO_RESPONSE, revealed_pexeso, actual_session);

                }
                else{
                    //TODO poslat vyresetovani
                }

                if(is_game_over(actual_session->game)){
                    //TODO Smazat hru
                    printf("Konec hry \n");
                }
                else{
                    nextTurn(actual_session->game);
                    send_who_is_on_turn_both_clients_in_session(actual_session);
                }
            }
            else{
                printf("Hrac hraje jeste jednou\n");
                send_client_message(actual_client->socket,PLAY_AGAIN_RESPONSE,"");
            }
        }
        //Neni validni tah
        else{
            printf("Nevalidni tah\n");
        }
        printf("Jo hrac je na rade bude hrat \n");
    }
    else{
        printf("Hrac neni na rade!\n");
    }
}
void send_who_is_on_turn_both_clients_in_session(session* actual_session){
    log_trace("SEND WHO IS ON TURN BOTH CLIENTS");
    char* yes = "1";
    char* no = "0";

    int p1_on_turn = is_client_on_turn(actual_session->first_client, actual_session);
    //Je prvni hrac na tahu
    if(p1_on_turn){
        log_trace("SEND WHO IS ON TURN BOTH CLIENTS - P1 is on turn");
        send_client_message(actual_session->first_client->socket,IS_PLAYER_TURN_RESPONSE, yes);
        send_client_message(actual_session->second_client->socket,IS_PLAYER_TURN_RESPONSE, no);
    }
    //Neni prvni hrac na tahu
    else{
        log_trace("SEND WHO IS ON TURN BOTH CLIENTS - P2 is on turn");
        send_client_message(actual_session->first_client->socket,IS_PLAYER_TURN_RESPONSE, no);
        send_client_message(actual_session->second_client->socket,IS_PLAYER_TURN_RESPONSE, yes);
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



