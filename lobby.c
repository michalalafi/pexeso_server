#include "lobby.h"

lobby* create_lobby(){
	lobby* new_lobby = (lobby*)malloc(sizeof(lobby));
	if(new_lobby == NULL){
		return NULL;
	}
	new_lobby->first = NULL;
	new_lobby->last = NULL;
	new_lobby->number_of_connected_clients = 0;

	return new_lobby;
}

void add_client_to_lobby(client* new_client, lobby* actual_lobby){
	client* first = actual_lobby->first;
	if(first == NULL){
		actual_lobby->first = new_client;
		actual_lobby->last = new_client;

		actual_lobby->first->next = NULL;
		actual_lobby->first->previous = NULL;
		actual_lobby->number_of_connected_clients += 1;
		return;
	}

	client* last = actual_lobby->last;
	if(last == NULL){
		return;
	}
	last->next = new_client;
	new_client->previous = last;
	actual_lobby->last = new_client;
    actual_lobby->number_of_connected_clients += 1;
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

	actual_lobby->number_of_connected_clients -= 1;
}
int is_lobby_empty(lobby* actual_lobby){
    if(actual_lobby->first == NULL && actual_lobby->last == NULL){
        return 1;
    }
    return 0;
}
client* find_client_by_id(int client_id, lobby* actual_lobby){
    //printf("FINDING CLIENT BY ID: %d \n",client_id);
    if(is_lobby_empty(actual_lobby)){
        //printf("NO CLIENT FOUND DUE EMPTY LOBBY \n");
        return NULL;
    }

    client* pom = actual_lobby->first;
    while(pom != NULL){
        if(pom->id == client_id){
            printf("CLIENT FOUND: ID = %d NAME = %s \n",pom->id,pom->name);
            return pom;
        }
        pom = pom->next;
    }
    //printf("NO CLIENT FOUND WITH ID: %d \n",client_id);
    return NULL;
}

client* find_client_by_socket(int client_socket, lobby* actual_lobby){
    log_trace("FIND CLIENT BY SOCKET");
    if(is_lobby_empty(actual_lobby)){
        log_error("FIND CLIENT BY SOCKET - lobby is empty");
        return NULL;
    }

    client* pom = actual_lobby->first;
    while(pom != NULL){
        if(pom->socket == client_socket){
            return pom;
        }
        pom = pom->next;
    }
    return NULL;
}
int is_client_in_lobby_by_socket(int socket,lobby* actual_lobby){
    if(is_lobby_empty(actual_lobby)){
        return 0;
    }
    client* pom = actual_lobby->first;
    while(pom != NULL){
        if(pom->socket == socket){
            return 1;
        }
        pom = pom->next;
    }
    return 0;
}

int get_number_of_clients_online(lobby* actual_lobby){
    if(actual_lobby == NULL){
        log_error("GET NUMBER OF CLIENTS ONLINE - Not valid params");
        return -1;
    }
    return actual_lobby->number_of_connected_clients;
}
int get_new_client_unique_id(lobby* actual_lobby){
    int unique_id = 0;
    client* existing_client = NULL;
    do{
        unique_id = rand();
        existing_client = find_client_by_id(unique_id, actual_lobby);
    }while(existing_client != NULL);

    return unique_id;
}

void print_clients(lobby* actual_lobby){
    printf("    LOBBY\n");
	client* pom = actual_lobby->first;
	while(pom != NULL){
		printf("        Client: %s ID:%d socket:%d \n", pom->name, pom->id, pom->socket);
		pom = pom->next;
	}
}
