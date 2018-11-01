#include "session.h"

session* create_session(client* first_client, client* second_client, game* actual_game, int id){
	session* new_session = (session*) malloc(sizeof(session));
	if(new_session == NULL){
		return NULL;
	}
		
	new_session->id = id;
	new_session->first_client = first_client;
	new_session->second_client = second_client;
	new_session->game = actual_game;
	new_session->next = NULL;
	new_session->previous = NULL;
	
	printf("SESSION CREATED - For players: %s - %s \n", first_client->name, second_client->name);
	return new_session;
}
