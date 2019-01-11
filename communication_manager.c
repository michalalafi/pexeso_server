#include "communication_manager.h"

/*
* Function: handle_client
* ------------------------
* Zpracuje zpravu od klienta
* Pokud je validni zprava tak ji predame dale
* Jinak rozhodneme jestli jde o zadost o lobby
*
* arg: argument, ktery obsahuje client_handle_container
*
* returns: void
*/
int handle_client(client_handle_container* container){
    log_trace("CLIENT HANDLE - Start");
    log_info("CLIENT HANDLE - Client request: '%s' length:%d", container->message, strlen(container->message));
    clock_t t = clock();

    if(container == NULL)
    {
        log_error("CLIENT HANDLE ERROR - Not valid params!");
        return 0;
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
                log_error("HANDLE CLIENT - Client is already in lobby");
                send_client_message(container->client_socket,ALREADY_LOGGED_IN,"");
            }
            else{
                // Pridame do lobby
                handle_client_connect(container->client_socket, container->lobby);
            }
        }
        else{
            log_info("HANDLE CLIENT - Not valid message");
            send_client_message(container->client_socket, NOT_VALID_MESSAGE,"Not valid message");
            return NOT_VALID_MESSAGE_SENDED;
        }
    }
    // Zprava je ve spravnem formatu, tudiz zpracujeme pozadavek
    else{
        //Zkusime najit clienta podle id ve zprave
        client* actual_client  = find_client_by_id(client_message->client_id, container->lobby);
        if(actual_client == NULL){
            //Pokud je client null a je zadost o reconnect
            if(client_message->action == RECONNECT_REQUEST){
                log_info("HANDLE CLIENT - Reconnect request");
                handle_client_reconnect(container->client_socket, client_message, container->disconnected_clients_list, container->lobby, container->session_list, container->pexeso_count);
            }
            //Pokud je client null tak posleme ze nepozadal o lobby
            else{
                log_info("HANDLE CLIENT - Client not found, sending NOT LOGGED IN LOBBY");
                // Je clientovi odeslana zprava ze neni prihlasen
                send_client_message(container->client_socket, NOT_LOGGED_IN_LOBBY,"");
            }
        }
        else{
            execute_client_action(actual_client,client_message->action,client_message->params,container, container->sounds_folder_path, client_message);
        }
    }

    t = clock() - t;

    double time_taken = ((double)t)/CLOCKS_PER_SEC;
    free_client_handle_container(container);
    free_message(client_message);
    log_trace("CLIENT HANDLE - End with time: %fs\n", time_taken);
	return 0;
}
/*
* Function: handle_client_connect
* ------------------------
* Vytvori noveho clienta a vloziho do lobby
*
* client_socket: clientuv socket
* actual_lobby: list pripojenych clientu
*
* returns: void
*/
void handle_client_connect(int client_socket, lobby* actual_lobby){
    log_trace("HANDLE CONNECT");

    client* new_client = create_client(client_socket, get_new_client_unique_id(actual_lobby));
    if(new_client == NULL){
        log_error("HANDLE CLIENT CONNECT - Create client failed");
        return;
    }
    add_client_to_lobby(new_client,actual_lobby);

    send_client_message_with_int_param(client_socket, CLIENTS_ID_RESPONSE, new_client->id);
    send_client_message(client_socket, STATUS_RESPONSE, "Lobby");

    log_info("HANDLE CONNECT - Client added to lobby");
}
/*
* Function: handle_client_disconnect
* ------------------------
* Vlozi clienta do listu odpojenych clientu
* Uvedomi druheho hrace pokud byl v sessione
*
* client_socket: clientuv socket
* actual_lobby: list pripojenych clientu
* actual_disconnected_clients_list: list odpojenych clientu
* actual_session_list: list aktivnich session
*
* returns: void
*/
void handle_client_disconnect(int client_socket, lobby* actual_lobby, disconnected_clients_list* actual_disconnected_clients_list, session_list* actual_session_list, int pexeso_count){
    log_trace("HANDLE CLIENT DISCONNECT - Start");
    //Najdeme clienta v lobby
    client* actual_client = find_client_by_socket(client_socket, actual_lobby);
    if(actual_client == NULL){
        log_error("HANDLE CLIENT DISCONNECT - Client is not in lobby");
        return;
    }
    //Odstranimeho z lobby
    remove_client_from_lobby(actual_client, actual_lobby);
    //Ziskame session, pokud v nektere byl, dame vedet druhemu hraci
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
            reset_actual_revealed_pexesos(actual_session->game);
        }
        else{
            //Musime ji smazat protoze by se k ni mohl nekdo pripojit
            log_info("HANDLE CLIENT DISCONNET - Session: %d will be deleted", actual_session->id);
            //Smazeme z listu session
            remove_session_from_session_list(actual_session, actual_session_list);
            //Uvolnime hru
            free_game(actual_session->game, pexeso_count);
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
/*
* Function: handle_client_correct_disconnect
* ------------------------
* Odpoji clienta z lobby a ze sessiony
*
* client_socket: clientuv socket
* actual_lobby: list pripojenych clientu
* actual_session_list: list aktivnich session
*
* returns: void
*/
void handle_client_correct_disconnect(int client_socket, lobby* actual_lobby, session_list* actual_session_list, int pexeso_count){
    log_trace("HANDLE CLIENT CORRECT DISCONNECT - Start");
    //Najdeme clienta v lobby
    client* actual_client = find_client_by_socket(client_socket, actual_lobby);
    if(actual_client == NULL){
        log_error("HANDLE CLIENT CORRECT DISCONNECT - Client is not in lobby");
        return;
    }
    //Odstranimeho z lobby
    remove_client_from_lobby(actual_client, actual_lobby);
    //Ziskame sessionu pokud existuje
    session* actual_session = get_session_by_client(actual_client, actual_session_list);
    if(actual_session != NULL){
        handle_client_remove_from_session(actual_client, actual_session, actual_session_list, pexeso_count);
    }
    log_trace("HANDLE CLIENT CORRECT DISCONNECT - End");
}
/*
* Function: handle_client_reconnect
* ------------------------
* Vlozi clienta do lobby pokud tam jeste neni a smaze ho z odpojenych clientu
* Pokud byl v sessione tak obnovi hru
*
* client_socket: clientuv socket
* client_message: message od clienta
* actual_disconnected_clients_list: list odpojenych clientu
* actual_lobby: list pripojenych clientu
* actual_session_list: list aktivnich session
*
* returns: void
*/
void handle_client_reconnect(int client_socket, message* client_message, disconnected_clients_list* actual_disconnected_clients_list, lobby* actual_lobby, session_list* actual_session_list, int pexeso_count){
    log_trace("HANDLE CLIENT RECONNECT - Start");

    disconnected_client* actual_disconnected_client = find_disconnected_client_by_id(client_message->client_id, actual_disconnected_clients_list);
    client* actual_client = find_client_by_id(client_message->client_id, actual_lobby);

    //Nenasli jsme ho nikde ani v odpojenych ani v lobby
    if(actual_disconnected_client == NULL && actual_client == NULL){
        log_trace("HANDLE CLIENT RECONNECT - Client not found, we will create him");
        send_client_message(client_socket, RECONNECT_RESPONE, ""); // Posleme clientovi recconnect response pro kontrolu u clienta
        handle_client_connect(client_socket, actual_lobby); // Zalozime ho v lobby
    }
    else{
        log_trace("HANDLE CLIENT RECONNECT - Trying to reconnect");
        if(actual_client == NULL){
            //Vezmeme odpojeneho clienta a aktualizujeme mu socket
            actual_client = actual_disconnected_client->client;
            actual_client->socket = client_socket;

            //Rozhodne ho vratime do lobby
            add_client_to_lobby(actual_disconnected_client->client, actual_lobby);
            remove_disconnected_client_from_disconnected_clients_list(actual_disconnected_client, actual_disconnected_clients_list);
            free_disconnected_client(actual_disconnected_client);
            log_info("HANDLE CLIENT RECONNECT - Disconnected client was reconnected");
        }
        actual_client->socket = client_socket;

        session* actual_session = get_session_by_client(actual_client, actual_session_list);
        if(actual_session == NULL){
            //Nenalezena sessiona, posleme mu id
            send_client_message_with_int_param(client_socket, CLIENTS_ID_RESPONSE, actual_client->id);
            send_client_message(client_socket, STATUS_RESPONSE, "Lobby");
        }
        else{
            log_trace("HANDLE CLIENT RECONNECT - Trying to restore session");
            if(actual_session->game != NULL){
                send_client_message_with_int_param(actual_client->socket, NUMBER_OF_PEXESOS_RESPONSE, pexeso_count);
                //Napiseme ze zacla hra
                send_client_message(actual_client->socket, NEW_GAME_BEGIN_RESPONSE, "");
                //Napiseme jmeno clienta
                send_client_message(actual_session->first_client->socket, OPPONENTS_NAME_RESPONSE,actual_session->second_client->name);
                send_client_message(actual_session->second_client->socket, OPPONENTS_NAME_RESPONSE,actual_session->first_client->name);

                send_client_message_with_ints_params(actual_client->socket, SCORE_RESPONSE, actual_session->game->p1_score, actual_session->game->p1_score);
                // Posleme odhalene pexesa
                int i;
                int first_pexeso = -1;
                for(i = 0; i < pexeso_count; i++){
                    if(actual_session->game->revead_pexesos_indexes[i] != -1 && first_pexeso == -1)
                        first_pexeso = i;
                    else if(first_pexeso != -1 && actual_session->game->revead_pexesos_indexes[i] != -1){
                        send_client_message_with_ints_params(actual_client->socket, SUCCESFULLY_REVEALED_PEXESO_RESPONSE, first_pexeso, i);
                        first_pexeso = -1;
                    }
                }
                //Posleme kdo je na rade
                send_who_is_on_turn_both_clients_in_session(actual_session);
            }
            else{
                //Dame vedet obema hracum ze jsou spojeni a jestli chteji hrat
                send_message_both_clients_in_session(WANT_TO_PLAY_GAME_RESPONSE,"",actual_session);
            }
        }
        send_client_message(client_socket, RECONNECT_RESPONE, ""); // Posleme clientovi recconnect response pro kontrolu u clienta
    }
    log_trace("HANDLE CLIENT RECONNECT - End");
}
/*
* Function: handle_disconnected_clients_list
* ------------------------
* Zkontroluje jestli odpojeny nepresahl limit a pokud jo tak ho odebere ze sessiony a uvolni
*
* actual_disconnected_clients_list: list odpojenych clientu
* actual_session_list: list aktivnich session
*
* returns: void
*/
void handle_disconnected_clients_list(session_list* actual_session_list, disconnected_clients_list* actual_disconnected_clients_list, int pexeso_count){
    log_trace("HANDLE DISCONNECTED CLIENTS LIST");
    disconnected_client* pom = actual_disconnected_clients_list->first;
    time_t now_t = time(0);
    while(pom != NULL){
        double diff_t = difftime(now_t, pom->time_of_disconnected);
        disconnected_client* pom_to_throw_away = pom;
        pom = pom->next;
        if(diff_t > LOGGED_OUT_TIMEOUT){
            throw_away_connection_with_client(pom_to_throw_away,actual_session_list, actual_disconnected_clients_list, pexeso_count);
        }
    }
}
/*
* Function: throw_away_connection_with_client
* ------------------------
* Odebere clienta z odpojenych a pokud je v sessione postara se o odebrani ze sessiony
* Uvolni clienta
* actual_disconnected_client: aktualni odpojeny client
* actual_disconnected_clients_list: list odpojenych clientu
* actual_session_list: list aktivnich session
*
* returns: void
*/
void throw_away_connection_with_client(disconnected_client* actual_disconnected_client, session_list* actual_session_list, disconnected_clients_list* actual_disconnected_clients_list, int pexeso_count){
    log_trace("THROW AWAY CONNECTION WITH CLIENT - Client: %s will be thrown away",actual_disconnected_client->client->name);
    //Odebereme ho z listu
    remove_disconnected_client_from_disconnected_clients_list(actual_disconnected_client, actual_disconnected_clients_list);
    //Ziskame sessionu pokud existuje
    session* actual_session = get_session_by_client(actual_disconnected_client->client, actual_session_list);
    if(actual_session != NULL){
        handle_client_remove_from_session(actual_disconnected_client->client, actual_session, actual_session_list, pexeso_count);
    }

    free_client(actual_disconnected_client->client);
    free_disconnected_client(actual_disconnected_client);
}
/*
* Function: handle_client_remove_from_session
* ------------------------
* Odebere clienta z sessiony pokud je pote prazdna smaze ji
*
* actual_client: aktualni client
* actual_session: aktualni sessiona clienta
* actual_session_list: list aktivnich session
*
* returns: void
*/
void handle_client_remove_from_session(client* actual_client, session* actual_session, session_list* actual_session_list, int pexeso_count){
    log_trace("HANDLE CLIENT REMOVE FROM SESSION");
    //Odebereme ho ze sessiony
    remove_client_from_session(actual_client, actual_session);
    //Pokud sessiona neni prazdna dame vedet oppentovi
    if(!is_session_empty(actual_session)){
        client* last_client_in_session = get_some_client_from_session(actual_session);
        // Pokud existuje napiseme mu zpravu ze opponent opustil hru
        if(last_client_in_session != NULL){
            send_client_message(last_client_in_session->socket, OPPONENT_LEFT_COMPLETELY_RESPONSE,"");
            send_client_message(last_client_in_session->socket, STATUS_RESPONSE, "Wait for player to join...");
            //Vyresetujeme sessionu
            reset_session_for_new_game(actual_session, pexeso_count);
        }
    }
    else{
        log_info("HANDLE CLIENT REMOVE FROM SESSION - Session: %d will be deleted", actual_session->id);
        //Smazeme z listu session
        remove_session_from_session_list(actual_session, actual_session_list);
        //Uvolnime hru
        free_game(actual_session->game, pexeso_count);
        //Zrusime sessionu
        free_session(actual_session);
    }
}
/*
* Function: execute_client_action
* ------------------------
* Zajisti volani zpravneho zpracovani requestu
*
* actual_client: aktualni client
* action: akce kterou pozaduje client
* params: parametry ktere posila
* container: container pro struktury serveru
* sounds_folder_path: cesta ke slozce zvuku
* client_message: clientova zprava
*
* returns: void
*/
void execute_client_action(client* actual_client, int action, char* params, client_handle_container* container, char* sounds_folder_path, message* client_message){
    log_trace("EXECUTE ACTION: %d",action);
    switch(action){
        case NEW_SESSION_REQUEST:
            new_session_request(actual_client, container->session_list);
            break;
        case NEW_GAME_REQUEST:
            new_game_request(actual_client, params, container->session_list, sounds_folder_path, container->pexeso_count);
            break;
        case PEXESO_REVEALED_REQUEST:
            pexeso_reveale_request(actual_client, params, container->session_list, container->pexeso_count);
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
            is_alive_request(actual_client);
            break;
        case DISCONNECT_REQUEST:
            handle_client_correct_disconnect(container->client_socket, container->lobby, container->session_list, container->pexeso_count);
            break;
        case RECONNECT_REQUEST:
            handle_client_reconnect(container->client_socket, client_message, container->disconnected_clients_list, container->lobby, container->session_list, container->pexeso_count);
            break;

    }

}


/*###########################REQUESTS#########################*/
/*
* Function: new_session_request
* ------------------------
* Obslouzi request o novou sessionu
*
* actual_client: aktualni client
* actual_session_list: aktualni session list
*
* returns: void
*/
void new_session_request(client* actual_client, session_list* actual_session_list){
    log_trace("NEW SESSION REQUEST - Start");
    // Pokud je uz v sessione, tak mu napiseme zpravu
    if(is_client_in_session_list(actual_client, actual_session_list)){
        log_info("USER ALREADY IN SESSION");
        //Posleme mu at ze new session -> toho prevede na waiting
        send_client_message(actual_client->socket, NEW_SESSION_RESPONSE,"");
        send_client_message(actual_client->socket, STATUS_RESPONSE, "Already in session...");
        return;
    }
    //Ziskame otevrenou sessionu, pokud neni zadna vytvorime
    session* actual_session = get_open_session(actual_session_list);
    if(actual_session == NULL){
        log_trace("CREATING NEW SESSION");
        // Vytvorime novou sessionu
        actual_session = create_session(actual_client,NULL,NULL,get_new_session_unique_id(actual_session_list));

        if(actual_session == NULL){
            log_error("NEW SESSION REQUEST - Creating session failed");
            return;
        }
        // Pridame ji do listu
        add_session_to_session_list(actual_session, actual_session_list);

        send_client_message(actual_client->socket, NEW_SESSION_RESPONSE,"");
        //Posleme ze musi pockat na dalsiho hrace co se pripoji
        send_client_message(actual_client->socket,STATUS_RESPONSE,"Wait for player to join...");

        log_info("CREATING NEW SESSION SUCCESFULL");
    }
    else{
        log_trace("FOUND OPEN SESSION");

        add_client_in_session(actual_client, actual_session);

        send_client_message(actual_client->socket, NEW_SESSION_RESPONSE,"");
        //Dame vedet obema hracum ze jsou spojeni a jestli chteji hrat
        send_message_both_clients_in_session(WANT_TO_PLAY_GAME_RESPONSE,"",actual_session);

        send_message_both_clients_in_session(STATUS_RESPONSE,"New game found...", actual_session);
    }
    log_trace("NEW SESSION REQUEST - End");
}
/*
* Function: new_game_request
* ------------------------
* Obslouzi request o novou hru
*
* actual_client: aktualni client
* params: parametry zpravy co nam poslal
* actual_session_list: aktualni session list
* sounds_folder_path: cesta ke slozce se zvuky
*
* returns: void
*/
void new_game_request(client* actual_client, char* params, session_list* actual_session_list, char* sounds_folder_path, int pexeso_count){
    log_trace("NEW GAME REQUEST - Start");
    session* actual_session = get_session_by_client(actual_client, actual_session_list);
    if(actual_session == NULL){
        log_error("NEW GAME REQUEST - Client isn't in session");
        send_client_message(actual_client->socket, NEW_GAME_RESPONSE, ""); //Posleme new game response pro kontrolu u clienta
        send_client_message(actual_client->socket, STATUS_RESPONSE, "Error. No session found...");
        return;
    }
    if(actual_session->game != NULL){
        send_client_message(actual_client->socket, NEW_GAME_RESPONSE, ""); //Posleme new game response pro kontrolu u clienta
        return;
    }

    int want_to_play = convert_string_to_long(params);
    if(want_to_play){
        log_info("NEW GAME REQUEST - Client wants to play");
        if(!is_session_valid(actual_session)){
            log_error("NEW GAME REQUEST - Session is not valid");
            reset_session_for_new_game(actual_session, pexeso_count);
            send_client_message(actual_client->socket, NEW_GAME_RESPONSE, ""); //Posleme new game response pro kontrolu u clienta
            send_client_message(actual_client->socket, STATUS_RESPONSE, "Wait for player to join...");
            send_client_message(actual_client->socket, OPPONENT_LEFT_COMPLETELY_RESPONSE, "");
        }
        else{
            set_client_wants_play(actual_client, actual_session, 1);
            //Oba chteji hrat
            if(is_session_ready_to_play_game(actual_session)){
                log_trace("NEW GAME REQUEST - Creating game");
                actual_session->game = create_game_with_path(sounds_folder_path, pexeso_count);
                if(actual_session->game == NULL){
                    log_error("GAME CREATING FAILED");
                    return;
                }
                send_message_both_clients_in_session(NEW_GAME_RESPONSE, "", actual_session); // Posleme new game response pro controlu u clientu

                send_message_both_clients_in_session_with_int_param(actual_session, NUMBER_OF_PEXESOS_RESPONSE, pexeso_count);
                //Napiseme ze zacla hra
                send_message_both_clients_in_session(NEW_GAME_BEGIN_RESPONSE,"",actual_session);
                //Napiseme jmeno clienta
                send_client_message(actual_session->first_client->socket, OPPONENTS_NAME_RESPONSE,actual_session->second_client->name);
                send_client_message(actual_session->second_client->socket, OPPONENTS_NAME_RESPONSE, actual_session->first_client->name);
                //Napiseme kdo hraje
                send_who_is_on_turn_both_clients_in_session(actual_session);

                send_message_both_clients_in_session(STATUS_RESPONSE, "Game is in progress...", actual_session);
            }
            // Souper jeste neodpovedel
            else{
                send_client_message(actual_client->socket, NEW_GAME_RESPONSE, ""); //Posleme new game response pro kontrolu u clienta
                send_client_message(actual_client->socket, STATUS_RESPONSE, "Wait for opponent to decide...");
            }
        }
    }
    //Nechtel hrat
    else{
        //Odstranime ho ze sessiony
        handle_client_remove_from_session(actual_client, actual_session, actual_session_list, pexeso_count);

        send_client_message(actual_client->socket, NEW_GAME_RESPONSE, ""); //Posleme new game response pro kontrolu u clienta
        send_client_message(actual_client->socket, RETURN_TO_LOBBY_RESPONSE, "");
        send_client_message(actual_client->socket, STATUS_RESPONSE, "Lobby");
    }
    log_trace("NEW GAME REQUEST - End");
}
/*
* Function: pexeso_reveale_request
* ------------------------
* Obslouzi request o odhaleni pexesa
*
* actual_client: aktualni client
* params: parametry zpravy co nam poslal
* actual_session_list: aktualni session list
*
* returns: void
*/
void pexeso_reveale_request(client* actual_client, char* params, session_list* actual_session_list, int pexeso_count){
    log_trace("PEXESO REVEAL REQUEST - Start");
    session* actual_session = get_session_by_client(actual_client, actual_session_list);
    if(actual_session == NULL){
        log_error("PEXESO REVEAL REQUEST - Client is not in session");
        send_client_message(actual_client->socket, RETURN_TO_LOBBY_RESPONSE, "");
        return;
    }
    //Pokud je nevalidni sessiona posleme at se vrati do lobby
    if(!is_session_valid(actual_session) || actual_session->game == NULL){
        log_error("PEXESO REVEAL REQUEST - Not valid session or no game");
        send_client_message(actual_client->socket, RETURN_TO_LOBBY_RESPONSE, "");
        return;
    }
    int is_on_turn = is_client_on_turn(actual_client, actual_session);
    // Pokud neni na rade
    if(is_on_turn == -1){
        log_error("PEXESO REVEAL REQUEST - Is on turn returned -1");
        return;
    }
    // Pokud je na rade
    if(is_on_turn){
        int pexeso_revealed = convert_string_to_long(params);
        if(pexeso_revealed == -1){
            log_error("PEXESO REVEAL REQUEST - Not valid pexeso");
            // Vyresetujeme pexesa
            send_who_is_on_turn_both_clients_in_session(actual_session);
            // Posleme ze nekdo zahral nevalidni pexeso
            send_message_both_clients_in_session(STATUS_RESPONSE, "Not valid pexeso was played...", actual_session);
            return;
        }
        int is_pexeso_revealed_valid = isValid(pexeso_revealed, actual_session->game); // Je pexeso validni
        //Je validni tah
        if(is_pexeso_revealed_valid){
            char* sound = reveal(pexeso_revealed, actual_session->game);
            if(sound == NULL){
                log_error("PEXESO REVEAL REQUEST - Reveal error");
                return;
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
                if(is_game_over(actual_session->game)){
                    send_message_both_clients_in_session(END_OF_GAME,"",actual_session);
                    log_trace("PEXESO REVEAL REQUEST - Game over, reseting game and session");
                    reset_session_for_new_game(actual_session, pexeso_count);
                }
                else{
                    nextTurn(actual_session->game);
                    print_not_revealed_sounds(actual_session->game, pexeso_count);
                    print_revealed_sounds_indexes(actual_session->game, pexeso_count);
                    send_who_is_on_turn_both_clients_in_session(actual_session);
                }
            }
            else{
                send_client_message(actual_client->socket,PLAY_AGAIN_RESPONSE,"");
            }
        }
        //Neni validni tah
        else{
            log_error("PEXESO REVEAL REQUEST - Not valid pexeso");
            send_who_is_on_turn_both_clients_in_session(actual_session); // Vyresetujeme pexesa
            send_message_both_clients_in_session(STATUS_RESPONSE, "Not valid pexeso was played...", actual_session);
        }
    }
    else{
        log_error("PEXESO REVEAL REQUEST - Client is NOT on turn");
    }
    log_trace("PEXESO REVEAL REQUEST - End");
}
/*
* Function: session_id_request
* ------------------------
* Obslouzi request o id sessiony
*
* actual_client: aktualni client
* actual_session_list: aktualni session list
*
* returns: void
*/
void session_id_request(client* actual_client, session_list* actual_session_list){
    log_trace("SESSION ID REQUEST");
    session* actual_session = get_session_by_client(actual_client, actual_session_list);
    if(actual_session == NULL){
        log_error("SESSION ID REQUEST - Client is not in session");
        return;
    }
    send_client_message_with_int_param(actual_client->socket, SESSION_ID_RESPONSE, actual_session->id);
}
/*
* Function: client_name_request
* ------------------------
* Obslouzi request o clientovo jmeno
*
* actual_client: aktualni client
*
* returns: void
*/
void client_name_request(client* actual_client){
    log_trace("CLIENT NAME REQUEST");
    send_client_message(actual_client->socket, CLIENTS_NAME_RESPONSE, actual_client->name);
}
/*
* Function: number_of_clients_online_request
* ------------------------
* Obslouzi request o pocet clientu v lobby
*
* actual_client: aktualni client
* actual_lobby: aktualni lobby
*
* returns: void
*/
void number_of_clients_online_request(client* actual_client, lobby* actual_lobby){
    log_trace("NUMBER OF CLIENTS ONLINE REQUEST");
    int number_of_clients_online = get_number_of_clients_online(actual_lobby);
    send_client_message_with_int_param(actual_client->socket, NUMBER_OF_CLIENTS_ONLINE_RESPONSE, number_of_clients_online);
}
/*
* Function: is_alive_request
* ------------------------
* Obslouzi request o to jestli je server dostupny
*
* actual_client: aktualni client
*
* returns: void
*/
void is_alive_request(client* actual_client){
    log_trace("IS ALIVE REQUEST");
    send_client_message(actual_client->socket, IS_ALIVE_RESPONSE, "");
}
/*
* Function: send_who_is_on_turn_both_clients_in_session
* ------------------------
* Posle clientum v sessione jestli jsou na rade
*
* actual_session: aktualni sessiona
*
* returns: void
*/
void send_who_is_on_turn_both_clients_in_session(session* actual_session){
    log_trace("SEND WHO IS ON TURN BOTH CLIENTS");
    char* yes = "1";
    char* no = "0";

    if(!is_session_valid(actual_session)){
        log_error("SEND WHO IS ON TURN BOTH CLIENTS - Session is not valid");
        return;
    }
    int p1_on_turn = is_client_on_turn(actual_session->first_client, actual_session);
    //Je prvni hrac na tahu
    if(p1_on_turn){
        log_info("SEND WHO IS ON TURN BOTH CLIENTS - P1 is on turn");
        send_client_message(actual_session->first_client->socket,IS_PLAYER_TURN_RESPONSE, yes);
        send_client_message(actual_session->second_client->socket,IS_PLAYER_TURN_RESPONSE, no);
    }
    //Neni prvni hrac na tahu
    else{
        log_info("SEND WHO IS ON TURN BOTH CLIENTS - P2 is on turn");
        send_client_message(actual_session->first_client->socket,IS_PLAYER_TURN_RESPONSE, no);
        send_client_message(actual_session->second_client->socket,IS_PLAYER_TURN_RESPONSE, yes);
    }
}
/*
* Function: send_message_both_clients_in_session
* ------------------------
* Posle clientum v sessione zpravu s akci a parametrama
*
* action: akce kterou posilame
* params: parametry ktere posilame
* actual_session: aktualni sessiona
*
* returns: void
*/
void send_message_both_clients_in_session(int action, char* params, session* actual_session){
    log_trace("SEND MESSAGE BOTH CLIENTS IN SESSION");
    if(!is_session_valid(actual_session)){
        log_error("SEND MESSAGE BOTH CLIENTS IN SESSION - Session is not valid");
        return;
    }
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
    if(buff == NULL){
        log_error("SEND CLIENT MESSAGE - Buffer is NULL");
        return;
    }
    send(client_socket, buff, (size_t) strlen(buff),0);
    send(client_socket, &message_terminator, 1,0);

    log_info("SEND CLIENT MESSAGE - Sent message: %s", buff);
    free(buff);
}
/*
* Function: send_client_message_with_int_param
* ------------------------
* Posle clientovi zpravu s akci a parametrem int
*
* client_socket: clientovo socket
* action: cislo akce
* params: parametr int
*
* returns: void
*/
void send_client_message_with_int_param(int client_socket, int action, int param){
    log_trace("SEND CLIENT MESSAGE WITH INT PARAM");
    //Vytvorime paramatry
    char* int_string = (char*) malloc(sizeof(char) * 255);
    sprintf(int_string, "%d", param);

    send_client_message(client_socket, action, int_string);

    free(int_string);
}
/*
* Function: send_client_message_with_ints_params
* ------------------------
* Posle clientovi zpravu s akci a parametrema int
*
* client_socket: clientovo socket
* action: cislo akce
* param1: parametr int
* param2: parametr2 int
*
* returns: void
*/
void send_client_message_with_ints_params(int client_socket, int action, int param1, int param2){
    log_trace("SEND CLIENT MESSAGE WITH INTS PARAMS");
    //Vytvorime paramatry
    char* int_string = (char*) malloc(sizeof(char) * 255 * 2);
    sprintf(int_string, "%d;%d", param1,param2);

    send_client_message(client_socket, action, int_string);

    free(int_string);
}
/*
* Function: send_message_both_clients_in_session_with_ints_params
* ------------------------
* Posle zpravu obema clientum v sessione s int parametrama
*
* actual_session: aktualni sessiona
* param1: parametr int
* param2: parametr2 int
*
* returns: void
*/
void send_message_both_clients_in_session_with_ints_params(session* actual_session, int action, int param1, int param2){
    log_trace("SEND MESSAGE BOTH CLIENTS IN SESSION WITH INTS PARAMS");
    if(!is_session_valid(actual_session)){
        log_error("SEND MESSAGE BOTH CLIENTS IN SESSION WITH INT PARAMS - Session is not valid");
        return;
    }
    send_client_message_with_ints_params(actual_session->first_client->socket, action, param1, param2);
    send_client_message_with_ints_params(actual_session->second_client->socket, action, param1, param2);
}
/*
* Function: send_message_both_clients_in_session_with_int_param
* ------------------------
* Posle zpravu obema clientum v sessione s int parametrem
*
* actual_session: aktualni sessiona
* param: parametr int
*
* returns: void
*/
void send_message_both_clients_in_session_with_int_param(session* actual_session, int action, int param){
    log_trace("SEND MESSAGE BOTH CLIENTS IN SESSION WITH INT PARAM");
    if(!is_session_valid(actual_session)){
        log_error("SEND MESSAGE BOTH CLIENTS IN SESSION WITH INT PARAM- Session is not valid");
        return;
    }
    send_client_message_with_int_param(actual_session->first_client->socket, action, param);
    send_client_message_with_int_param(actual_session->second_client->socket, action, param);
}
/*
* Function: is_raw_message_login_to_lobby_request
* ------------------------
* Zjisti jestli je string zprava se zadosti o lobby
*
* raw_message: puvodni zprava od clienta
*
* returns: Je/Neni
*/
int is_raw_message_login_to_lobby_request(char* raw_message){
    log_trace("IS RAW MESSAGE LOGIN TO LOBBY REQUEST");
    long action = convert_string_to_long(raw_message);
    if(action == -1){
        return 0;
    }

    if(action == LOGIN_TO_LOBBY_REQUEST){
        return 1;
    }

    return 0;
}



