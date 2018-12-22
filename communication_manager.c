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
    log_info("CLIENT MESSAGE: '%s' length:%d", container->message, strlen(container->message));
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
            //TODO pokud je akce recconect tak predame metode
            if(client_message->action == RECONNECT_REQUEST){
                log_info("Zadost o reconnect");
                handle_client_reconnect(container->client_socket, client_message, container->disconnected_clients_list, container->lobby, container->session_list);
            }
            else{
                log_info("Client nenalezen");
                // Je clientovi odeslana zprava ze neni prihlasen
                send_client_message(container->client_socket, NOT_LOGGED_IN_LOBBY,"");
            }
        }
        else{
            execute_client_action(actual_client,client_message->action,client_message->params,container, container->sounds_folder_path);
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

    send_client_message_with_int_param(client_socket, CLIENTS_ID_RESPONSE, new_client->id);
    send_client_message(client_socket, STATUS_RESPONSE, "Lobby");
}

void handle_client_disconnect(int client_socket, lobby* actual_lobby, disconnected_clients_list* actual_disconnected_clients_list, session_list* actual_session_list){
    log_trace("HANDLE CLIENT DISCONNECT - Start");
    //Najdeme clienta v lobby
    client* actual_client = find_client_by_socket(client_socket, actual_lobby);
    if(actual_client == NULL){
        log_error("HANDLE CLIENT DISCONNECT - Client is not in lobby");
        return;
    }
    //Odstranimeho z lobby
    remove_client_from_lobby(actual_client, actual_lobby);

    session* actual_session = get_session_by_client( actual_client, actual_session_list);
    if(actual_session != NULL){
        log_info("HANDLE CLIENT DISCONNET - Client was in session");
        if(is_session_valid(actual_session)){
            //Posleme opponentovi ze se client odpojil
            if(actual_session->first_client->id == actual_client->id){
                send_client_message(actual_session->second_client->socket, OPPONENT_LEFT_RESPONSE,"");
                send_client_message(actual_session->second_client->socket, STATUS_RESPONSE, "Opponent left - Wait...");
            }
            else{
                send_client_message(actual_session->first_client->socket, OPPONENT_LEFT_RESPONSE,"");
                send_client_message(actual_session->first_client->socket, STATUS_RESPONSE, "Opponent left - Wait...");
            }
        }
        else{
            //Musime ji smazat protoze by se k ni mohl nekdo pripojit
            log_info("Session: %d will be deleted", actual_session->id);
            //Smazeme z listu session
            remove_session_from_session_list(actual_session, actual_session_list);
            //Zrusime sessionu
            free_session(actual_session);
        }
    }

    //Vytvorime odpojeneho clienta
    disconnected_client* disconnected_client = create_disconnected_client(actual_client);
    if(disconnected_client == NULL){
        log_error("HANDLE CLIENT DISCONNECT - Creating disconnected client failed");
        return;
    }
    //Vlozime do listu odpojenych
    add_disconnected_client_to_disconnected_clients_list(disconnected_client, actual_disconnected_clients_list);
    log_trace("HANDLE CLIENT DISCONNECT - End");
}

void handle_client_reconnect(int client_socket, message* client_message, disconnected_clients_list* actual_disconnected_clients_list, lobby* actual_lobby, session_list* actual_session_list){
    log_trace("HANDLE CLIENT RECONNECT");
    disconnected_client* actual_disconnected_client = find_disconnected_client_by_id(client_message->client_id, actual_disconnected_clients_list);
    if(actual_disconnected_client == NULL){
        handle_client_connect(client_socket, actual_lobby);
    }
    else{
        client* actual_client = actual_disconnected_client->client;
        //Nasli jsme ho
        //TODO jestli mel zalozenou hru pripojime
        //Rozhodne ho vratime do lobby
        add_client_to_lobby(actual_disconnected_client->client, actual_lobby);
        log_info("Pridali jsme disconnetleho clienta do lobby");

        session* actual_session = get_session_by_client(actual_disconnected_client->client, actual_session_list);
        if(actual_session == NULL){
            //Nenalezena sessiona
            //Posleme mu id
            send_client_message_with_int_param(client_socket, CLIENTS_ID_RESPONSE, actual_disconnected_client->client->id);
            send_client_message(client_socket, STATUS_RESPONSE, "Lobby");
            log_info("Client was not in session");

        }
        else{
            if(actual_session->game != NULL){
                send_client_message_with_int_param(actual_client->socket, NUMBER_OF_PEXESOS_RESPONSE, PEXESO_COUNT);
                //Napiseme ze zacla hra
                send_client_message(actual_client->socket, NEW_GAME_BEGIN_RESPONSE, "");
                //Napiseme jmeno clienta
                send_client_message(actual_session->first_client->socket, OPPONENTS_NAME_RESPONSE,actual_session->second_client->name);
                send_client_message(actual_session->second_client->socket, OPPONENTS_NAME_RESPONSE,actual_session->first_client->name);

                send_client_message_with_ints_params(actual_client->socket, SCORE_RESPONSE, actual_session->game->p1_score, actual_session->game->p1_score);

                int i;
                int first_pexeso = -1;
                for(i = 0; i < PEXESO_COUNT; i++){
                    if(actual_session->game->revead_pexesos_indexes[i] != -1 && first_pexeso == -1)
                        first_pexeso = i;
                    else if(first_pexeso != -1 && actual_session->game->revead_pexesos_indexes[i] != -1){
                        send_client_message_with_ints_params(actual_client->socket, SUCCESFULLY_REVEALED_PEXESO_RESPONSE, first_pexeso, i);
                        first_pexeso = -1;
                    }
                }

                //TODO posleme druhemu ze se uzivatel vratil
                //TODO posleme tomu prvnime odhalene pexeso

            }
            else{
                //TODO zalozit hru? a nebo je odpojit
            }

        }


    }

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
                    //TODO poslat ze na dobro odesel
                    send_client_message(last_client_in_session->socket, OPPONENT_LEFT_RESPONSE,"");
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
void execute_client_action(client* actual_client, int action, char* params, client_handle_container* container, char* sounds_folder_path){
    printf("EXECUTE ACTION: %d \n",action);
    switch(action){
        case NEW_SESSION_REQUEST:
            new_session_request(actual_client, container->session_list);
            break;
        case NEW_GAME_REQUEST:
            new_game_request(actual_client, params, container->session_list, sounds_folder_path);
            break;
        case PEXESO_REVEALED_REQUEST:
            pexeso_reveale_request(actual_client, params, container->session_list);
            break;

        case SESSION_ID_REQUEST:
            session_id_request(actual_client, container->session_list);
            break;
        case CLIENTS_NAME_REQUEST:
            client_name_request(actual_client);
            break;

        case NUMBER_OF_CLIENTS_ONLINE_REQUEST:
            number_of_clients_online_request(actual_client, container->lobby);
            break;

        case IS_ALIVE_REQUEST:
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

        send_client_message(actual_client->socket, NEW_SESSION_RESPONSE,"");
        //Posleme ze musi pockat na dalsiho hrace co se pripoji
        send_client_message(actual_client->socket,STATUS_RESPONSE,"Wait for player to join...");
    }
    else{
        log_info("Nalezena otevrena sessiona \n");

        add_client_in_session(actual_client, actual_session);

        send_client_message(actual_client->socket, NEW_SESSION_RESPONSE,"");

        printf("    Pridelana sessiona: %d\n",actual_session->id);

        //Dame vedet obema hracum ze jsou spojeni a jestli chteji hrat
        send_message_both_clients_in_session(WANT_TO_PLAY_GAME_RESPONSE,"",actual_session);

        send_message_both_clients_in_session(STATUS_RESPONSE,"New game found...", actual_session);
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
void new_game_request(client* actual_client, char* params, session_list* actual_session_list, char* sounds_folder_path){
    log_trace(" NEW GAME REQUEST\n");
    session* actual_session = get_session_by_client(actual_client, actual_session_list);
    if(actual_session == NULL){
        log_error("NEW GAME REQUEST - Client isn't in session");
        //TODO Posleme clientovi ze jeste neni v sessione
        return;
    }
    int want_to_play = convert_string_to_long(params);
    if(want_to_play){
        log_trace("Client chce hrat");
        if(!is_session_valid(actual_session)){
            log_error("NEW GAME REQUEST - Session is not valid");
            reset_session_for_new_game(actual_session);
            send_client_message(actual_client->socket, STATUS_RESPONSE, "Wait for player to join...");
        }
        else{
            set_client_wants_play(actual_client, actual_session, 1);
            //Oba chteji hrat
            if(is_session_ready_to_play_game(actual_session)){
                //TODO poslat obema ze se hraje
                //TODO pokud maji uz zalozenou?
                log_trace("Zakladame hru");
                actual_session->game = create_game_with_path(sounds_folder_path);
                if(actual_session->game == NULL){
                    log_error("GAME CREATING FAILED");
                    return;
                }
                send_message_both_clients_in_session_with_int_param(actual_session, NUMBER_OF_PEXESOS_RESPONSE, PEXESO_COUNT);
                //Napiseme ze zacla hra
                send_message_both_clients_in_session(NEW_GAME_BEGIN_RESPONSE,"",actual_session);
                //Napiseme jmeno clienta
                send_client_message(actual_session->first_client->socket, OPPONENTS_NAME_RESPONSE,actual_session->second_client->name);
                send_client_message(actual_session->second_client->socket, OPPONENTS_NAME_RESPONSE, actual_session->first_client->name);
                //Napiseme kdo hraje
                send_who_is_on_turn_both_clients_in_session(actual_session);
            }
            // Souper jeste neodpovedel
            else{
                log_trace("Souper jeste neodpovedel");
                send_client_message(actual_client->socket, STATUS_RESPONSE, "Wait for opponent to decide...");
            }
        }
    }
    //Nechtel hrat
    else{
        //Odstranime ho ze sessiony a vratime do lobby
        remove_client_from_session(actual_client, actual_session);
        log_info("Client odstranen ze sessiony");
        send_client_message(actual_client->socket, RETURN_TO_LOBBY_RESPONSE, "");
        send_client_message(actual_client->socket, STATUS_RESPONSE, "Lobby");

        if(is_session_empty(actual_session)){
            log_info("Sessiona je prazdna je odstranena");
            remove_session_from_session_list(actual_session, actual_session_list);
            free(actual_session);
        }
        else{
            log_info("Posilame druhemu hraci zpravu at ceka na jineho hrace");
            //Napiseme druhemu clientovi
            send_message_both_clients_in_session(STATUS_RESPONSE,"Wait for player to join...", actual_session);

            reset_session_for_new_game(actual_session);
        }
    }
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
                    // Posleme skore prvnimu hraci
                    send_client_message_with_ints_params(actual_session->first_client->socket, SCORE_RESPONSE,actual_session->game->p1_score,actual_session->game->p2_score);
                    //Posleme skore druhemu hraci
                    send_client_message_with_ints_params(actual_session->second_client->socket,SCORE_RESPONSE,actual_session->game->p2_score,actual_session->game->p1_score);
                    //Posleme odhalene pexeso
                    send_message_both_clients_in_session_with_ints_params(actual_session, SUCCESFULLY_REVEALED_PEXESO_RESPONSE, actual_session->game->first_reveal,actual_session->game->second_reveal);

                }
                else{
                    //TODO poslat vyresetovani
                }

                if(is_game_over(actual_session->game)){
                    send_message_both_clients_in_session(END_OF_GAME,"",actual_session);
                    log_info("Resetujeme hru a sessionu");
                    reset_session_for_new_game(actual_session);
                    printf("Konec hry \n");
                }
                else{
                    nextTurn(actual_session->game);
                    print_not_revealed_sounds(actual_session->game);
                    print_revealed_sounds_indexes(actual_session->game);
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
void session_id_request(client* actual_client, session_list* actual_session_list){
    log_trace("SESSION ID REQUEST");
    session* actual_session = get_session_by_client(actual_client, actual_session_list);
    if(actual_session == NULL){
        log_error("CLIENT NENI V ZADNE SESSIONE \n");
        //TODO NAPSAT
        return;
    }
    send_client_message_with_int_param(actual_client->socket, SESSION_ID_RESPONSE, actual_session->id);
}
void client_name_request(client* actual_client){
    send_client_message(actual_client->socket, CLIENTS_NAME_RESPONSE, actual_client->name);
}
void number_of_clients_online_request(client* actual_client, lobby* actual_lobby){
    log_trace("NUMBER OF CLIENTS ONLINE REQUEST");
    int number_of_clients_online = get_number_of_clients_online(actual_lobby);

    send_client_message_with_int_param(actual_client->socket, NUMBER_OF_CLIENTS_ONLINE_RESPONSE, number_of_clients_online);
}
void is_alive_request(client* actual_client){
    send_client_message(actual_client->socket, IS_ALIVE_RESPONSE, "");
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
    if(actual_session->first_client != NULL){
        send_client_message(actual_session->first_client->socket, action, params);
    }
    if(actual_session->second_client != NULL){
        send_client_message(actual_session->second_client->socket, action, params);
    }
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
    log_trace("SEND CLIENT MESSAGE");
    const char message_terminator = '\n';
    char* buff = create_raw_message_for_client(action,params);
    send(client_socket, buff, (size_t) strlen(buff),0);
    send(client_socket, &message_terminator, 1,0);

    log_info("SEND CLIENT MESSAGE - Sent message: %s", buff);
    free(buff);
}

void send_client_message_with_int_param(int client_socket, int action, int param){
    log_trace("SEND CLIENT MESSAGE WITH INT PARAM");
    char* int_string = (char*) malloc(sizeof(char) * 255);
    sprintf(int_string, "%d", param);
    send_client_message(client_socket, action, int_string);

    free(int_string);
}
void send_client_message_with_ints_params(int client_socket, int action, int param1, int param2){
    log_trace("SEND CLIENT MESSAGE WITH INTS PARAMS");
    char* int_string = (char*) malloc(sizeof(char) * 255 * 2);
    sprintf(int_string, "%d;%d", param1,param2);
    send_client_message(client_socket, action, int_string);

    free(int_string);
}
void send_message_both_clients_in_session_with_ints_params(session* actual_session, int action, int param1, int param2){
    log_trace("SEND MESSAGE BOTH CLIENTS IN SESSION WITH INTS PARAMS");
    if(actual_session->first_client != NULL){
        send_client_message_with_ints_params(actual_session->first_client->socket, action, param1, param2);
    }
    if(actual_session->second_client != NULL){
        send_client_message_with_ints_params(actual_session->second_client->socket, action, param1, param2);
    }
}
void send_message_both_clients_in_session_with_int_param(session* actual_session, int action, int param){
    log_trace("SEND MESSAGE BOTH CLIENTS IN SESSION WITH INT PARAM");
    if(actual_session->first_client != NULL){
        send_client_message_with_int_param(actual_session->first_client->socket, action, param);
    }
    if(actual_session->second_client != NULL){
        send_client_message_with_int_param(actual_session->second_client->socket, action, param);
    }

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



