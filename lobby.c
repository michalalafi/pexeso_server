#include "lobby.h"

lobby* create_lobby(){
	lobby* new_lobby = (lobby*)malloc(sizeof(lobby));
	if(new_lobby == NULL){
		return NULL;
	}
	new_lobby->first = NULL;
	new_lobby->last = NULL;
	
	return new_lobby;
}

void add_client_to_lobby(client* new_client, lobby* actual_lobby){
	client* first = actual_lobby->first;
	if(first == NULL){
		actual_lobby->first = new_client;
		actual_lobby->last = new_client;
		
		actual_lobby->first->next = NULL;
		actual_lobby->first->previous = NULL;
		return;
	}
	
	client* last = actual_lobby->last;
	if(last == NULL){
		return;
	}
	last->next = new_client;
	new_client->previous = last;
	actual_lobby->last = new_client;
	
}

void remove_client_from_lobby(client* client_to_remove, lobby* actual_lobby){
	client* previous = client_to_remove->previous;
	client* next = client_to_remove->next;
	// Je prvek prvni?	
	if(actual_lobby->first == client_to_remove){
		// A ma nasledujici?
		if(next != NULL){
			//Prenastavime zacatek na jeho dalsiho
			actual_lobby->first = next;
			actual_lobby->first->previous = NULL;
		}
		else{
			//Jinak je seznam prazdny
			actual_lobby->first = NULL;
			actual_lobby->last = NULL;
		}
	}
	// Je prvek posledni?
	else if(actual_lobby->last == client_to_remove){
		// A ma predchoziho?
		if(previous != NULL){
			//Prenastavime konec na jeho predchoziho
			actual_lobby->last = previous;
			actual_lobby->last->next = NULL;
		}
		else{
			//Jinak je seznam prazdny
			actual_lobby->first = NULL;
			actual_lobby->last = NULL;
		}
	}
	// Je nekde mezi?
	if( next != NULL && previous != NULL){
		// Preskocime ho		
		previous->next = next;
		next->previous = previous;
	}
	client_to_remove->next = NULL;
	client_to_remove->previous = NULL;
}

void print_clients(lobby* actual_lobby){
	client* pom = actual_lobby->first;
	while(pom != NULL){
		printf("Client: %s \n", pom->name);
		pom = pom->next;
	}
}
