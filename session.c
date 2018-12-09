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

	//printf("SESSION CREATED - For players: %s - %s \n", first_client->name, second_client->name);
	return new_session;
}

int is_session_open(session* actual_session){
    printf("Je sessiona prazdna \n");
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
int is_client_in_session(client* actual_client, session* actual_session){
    printf("        IS CLIENT IN SESSION\n");
    if(actual_session == NULL || actual_client == NULL){
        printf("        NENI PROTOZE SESSION NEBO CLIENT JE NULL\n");
        return 0;
    }
    if(actual_session->first_client != NULL){
        if(actual_session->first_client->id == actual_client->id){
            printf("       JE PROTOZE JE PRVNI CLIENT\n");
            return 1;
        }
    }
    if(actual_session->second_client != NULL){
        if(actual_session->second_client->id == actual_client->id){
            printf("       JE PROTOZE JE DRUHY CLIENT\n");
            return 1;
        }
    }
    return 0;
}
void remove_client_from_session(client* actual_client, session* actual_session){
    if(actual_session->first_client != NULL){
        if(actual_session->first_client->id == actual_client->id){
            actual_session->first_client = NULL;
        }
    }
    else if(actual_session->second_client != NULL){
        if(actual_session->second_client->id == actual_client->id){
            actual_session->second_client = NULL;
        }
    }
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
