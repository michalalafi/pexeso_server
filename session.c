#include "session.h"

session* create_session(client* first_client, client* second_client, game* actual_game, int id){
	session* new_session = (session*) malloc(sizeof(session));
	if(new_session == NULL){
        printf("SESSION ERROR - Creating session failed!\n");
		return NULL;
	}

	new_session->id = id;
	new_session->first_client = first_client;
	new_session->second_client = second_client;
	new_session->game = actual_game;
	new_session->next = NULL;
	new_session->previous = NULL;
	new_session->want_first_client_play = -1;
	new_session->want_second_client_play = -1;

	//printf("SESSION CREATED - For players: %s - %s \n", first_client->name, second_client->name);
	return new_session;
}

void reset_session_for_new_game(session* actual_session){
    if(actual_session == NULL){
        log_error("RESET SESSION FOR NEW GAME - Not valid params");
        return;
    }
    free_game(actual_session->game);

    actual_session->game = NULL;
    actual_session->want_first_client_play = -1;
    actual_session->want_second_client_play = -1;
}

int is_session_open(session* actual_session){
    log_trace("IS SESSION OPEN");
    if(actual_session == NULL){
        log_error("IS SESSION OPEN - Not valid params");
        return -1;
    }
    // Je prvni hrac, ale neni druhy hrac
    if(actual_session->first_client != NULL && actual_session->second_client == NULL){
        return 1;
    }
    // Je druhy hrac, ale neni prvni hrac
    else if(actual_session->second_client != NULL && actual_session->first_client == NULL)
    {
        return 1;
    }
    return 0;
}
int is_session_empty(session* actual_session){
    if(actual_session == NULL){
        log_error("IS SESSION EMPTY - Not valid params");
        return -1;
    }

    if(actual_session->first_client == NULL && actual_session->second_client == NULL){
        return 1;
    }
    return 0;
}
client* get_some_client_from_session(session* actual_session){
    if(actual_session == NULL){
        log_error("IS SESSION EMPTY - Not valid params");
        return NULL;
    }

    if(actual_session->first_client != NULL){
        return actual_session->first_client;
    }
    if(actual_session->second_client != NULL){
        return actual_session->second_client;
    }
    return NULL;
}
int is_client_in_session(client* actual_client, session* actual_session){
    log_trace("IS CLIENT IN SESSION");
    if(actual_session == NULL || actual_client == NULL){
        return 0;
    }
    if(actual_session->first_client != NULL){
        if(actual_session->first_client->id == actual_client->id){
            return 1;
        }
    }
    if(actual_session->second_client != NULL){
        if(actual_session->second_client->id == actual_client->id){
            return 1;
        }
    }
    return 0;
}
void remove_client_from_session(client* actual_client, session* actual_session){
    log_trace("REMOVE CLIENT FROM SESSION");
    if(actual_session == NULL || actual_client == NULL){
        log_error("REMOVE CLIENT FROM SESSION - Not valid params");
        return;
    }

    if(actual_session->first_client != NULL){
        if(actual_session->first_client->id == actual_client->id){
            log_info("REMOVE CLIENT FROM SESSION - First client was removed");
            actual_session->first_client = NULL;
            return;
        }
    }
    if(actual_session->second_client != NULL){
        if(actual_session->second_client->id == actual_client->id){
            log_info("REMOVE CLIENT FROM SESSION - Second client was removed");
            actual_session->second_client = NULL;
            return;
        }
    }

    log_error("REMOVE CLIENT FROM SESSION - No client was removed");

}
int is_session_ready_to_play_game(session* actual_session){
    log_trace("IS SESSION READY TO PLAY GAME");
    if(actual_session->want_first_client_play == 1 && actual_session->want_second_client_play == 1){
        return 1;
    }
    return 0;
}
void set_client_wants_play(client* actual_client, session* actual_session, int value){
    if(actual_client == NULL || actual_session == NULL){
        log_error("CLIENT WANT PLAY - Not valid params");
        return;
    }

    if(!is_session_valid(actual_session)){
        log_error("CLIENT WANT PLAY - Session not valid");
        return;
    }

    if(actual_session->first_client->id == actual_client->id){
        actual_session->want_first_client_play = value;
    }
    else if(actual_session->second_client->id == actual_client->id){
        actual_session->want_second_client_play = value;
    }
}
void add_client_in_session(client* actual_client, session* actual_session){
    if(actual_client == NULL || actual_session == NULL){
        log_error("INSERT CLIENT IN SESSION - Not valid params");
        return;
    }

    if(actual_session->first_client == NULL){
        actual_session->first_client = actual_client;
    }
    else if(actual_session->second_client == NULL){
        actual_session->second_client = actual_client;
    }
}
int is_session_valid(session* actual_session){
    if(actual_session->first_client != NULL && actual_session->second_client != NULL){
        return 1;
    }
    return 0;
}
int get_client_player_order(client* actual_client, session* actual_session){
    log_trace("GET CLIENT PLAYER ORDER");
    if(actual_client == NULL || actual_session == NULL){
        log_error("GET CLIENT PLAYER ORDER - Not valid params");
        return -1;
    }
    if(actual_session->first_client == NULL || actual_session->second_client == NULL){
        log_error("GET CLIENT PLAYER ORDER - Not valid session");
        return -1;
    }
    if(actual_client->id == actual_session->first_client->id){
        return 0;
    }
    else if(actual_client->id == actual_session->second_client->id){
        return 1;
    }
    return -1;
}
int is_client_on_turn(client* actual_client, session* actual_session){
    log_trace("IS CLIENT ON TURN");
    if(actual_client == NULL || actual_session == NULL){
        log_error("IS CLIENT ON TURN - Not valid params");
        return -1;
    }
    if(actual_session->game == NULL){
        log_error("IS CLIENT ON TURN - No game");
        return -1;
    }
    int client_player_order = get_client_player_order(actual_client, actual_session);
    if(client_player_order == -1){
        printf("Chyba pri is client on turn\n");
        return -1;
    }
    log_trace("Je player order: %d == actual player: %d",client_player_order,actual_session->game->actual_player);
    if(client_player_order == actual_session->game->actual_player){
        return 1;
    }
    return 0;
}

void free_session(session* actual_session){
    if(actual_session == NULL){
        log_error("FREE SESSION - Not valid params");
        return;
    }
    actual_session->first_client = NULL;
    actual_session->second_client = NULL;
    actual_session->next = NULL;
    actual_session->previous = NULL;
    actual_session->game = NULL;
    free(actual_session);
}
