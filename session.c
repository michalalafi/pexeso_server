#include "session.h"

session* create_session(client* first_client, client* second_client, game* actual_game){
	session* new_session = (session*) malloc(sizeof(session));
	if(new_session == NULL){
		return NULL;
	}
	new_session->first_client = first_client;
	new_session->second_client = second_client;
	new_session->game = actual_game;
	
	return new_session;
}
