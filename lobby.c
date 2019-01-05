#include "lobby.h"


/*
* Function: create_lobby
* ------------------------
* Vytvori novy list pro pripojene clienty
*
* returns: novy list clientu
*/
lobby* create_lobby(){
	lobby* new_lobby = (lobby*)malloc(sizeof(lobby));
	if(new_lobby == NULL){
        log_error("CREATE LOBBY - Creating lobby failed");
		return NULL;
	}
	new_lobby->first = NULL;
	new_lobby->last = NULL;
	new_lobby->number_of_connected_clients = 0;

    log_info("LOBBY CREATED");
	return new_lobby;
}
/*
* Function: add_client_to_lobby
* ------------------------
* Prida clienta do listu
*
* new_client: pridavany client
* actual_lobby: seznam clientu
*
* returns: void
*/
void add_client_to_lobby(client* new_client, lobby* actual_lobby){
    if(new_client == NULL || actual_lobby == NULL){
        log_error("ADD CLIENT TO LOBBY - Not valid params");
        return;
    }
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
        log_error("ADD CLIENT TO LOBBY - Adding in lobby failed, last is NULL");
		return;
	}
	last->next = new_client;
	new_client->previous = last;
	actual_lobby->last = new_client;
    actual_lobby->number_of_connected_clients += 1;
}
/*
* Function: remove_client_from_lobby
* ------------------------
* Odstrani clienta z listu
*
* client_to_remove: client na odstraneni
* actual_lobby: seznam clientu
*
* returns: void
*/
void remove_client_from_lobby(client* client_to_remove, lobby* actual_lobby){
    if(client_to_remove == NULL || actual_lobby == NULL){
        log_error("REMOVE CLIENT FROM LOBBY - Not valid params");
        return;
    }
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
	if(next != NULL && previous != NULL){
		// Preskocime ho
		previous->next = next;
		next->previous = previous;
	}
	client_to_remove->next = NULL;
	client_to_remove->previous = NULL;

	actual_lobby->number_of_connected_clients -= 1;
}
/*
* Function: is_lobby_empty
* ------------------------
* Vraci jestli je lobby prazdne
*
* actual_lobby: seznam clientu
*
* returns: Ano/Ne
*/
int is_lobby_empty(lobby* actual_lobby){
    if(actual_lobby == NULL){
        log_error("IS LOBBY EMPTY - Not valid params");
        return -1;
    }
    if(actual_lobby->first == NULL && actual_lobby->last == NULL){
        return 1;
    }
    return 0;
}
/*
* Function: find_client_by_id
* ------------------------
* Najde clienta podle id
*
* client_id: id clienta
* actual_lobby: seznam clientu
*
* returns: nalezeny client
*/
client* find_client_by_id(int client_id, lobby* actual_lobby){
    log_trace("FIND CLIENT BY ID");
    if(actual_lobby == NULL){
        log_error("FIND CLIENT BY ID - Not valid params");
        return NULL;
    }
    if(is_lobby_empty(actual_lobby)){
        log_error("FIND CLIENT BY ID - Lobby is empty");
        return NULL;
    }
    client* pom = actual_lobby->first;
    while(pom != NULL){
        if(pom->id == client_id){
            return pom;
        }
        pom = pom->next;
    }
    return NULL;
}
/*
* Function: find_client_by_socket
* ------------------------
* Najde clienta podle socketu
*
* client_socket: socket clienta
* actual_lobby: seznam clientu
*
* returns: nalezeny client
*/
client* find_client_by_socket(int client_socket, lobby* actual_lobby){
    log_trace("FIND CLIENT BY SOCKET");
    if(actual_lobby == NULL){
        log_error("FIND CLIENT BY SOCKET - Not valid params");
        return NULL;
    }
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
/*
* Function: is_client_in_lobby_by_socket
* ------------------------
* Je client podle socketu v lobby
*
* socket: socket clienta
* actual_lobby: seznam clientu
*
* returns: Ano/Ne
*/
int is_client_in_lobby_by_socket(int socket, lobby* actual_lobby){
    if(actual_lobby == NULL){
        log_error("IS CLIENT IN LOBBY BY SOCKET - Not valid params");
        return -1;
    }
    if(is_lobby_empty(actual_lobby)){
        log_error("IS CLIENT IN LOBBY BY SOCKET - Lobby is empty");
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
/*
* Function: get_number_of_clients_online
* ------------------------
* Vraci pocet clientu v listu
*
* actual_lobby: seznam clientu
*
* returns: pocet clientu v listu
*/
int get_number_of_clients_online(lobby* actual_lobby){
    if(actual_lobby == NULL){
        log_error("GET NUMBER OF CLIENTS ONLINE - Not valid params");
        return -1;
    }
    return actual_lobby->number_of_connected_clients;
}
/*
* Function: get_new_client_unique_id
* ------------------------
* Vraci nove unikatni id pro clienta
*
* actual_lobby: seznam clientu
*
* returns: unikatni cislo clienta
*/
int get_new_client_unique_id(lobby* actual_lobby){
    if(actual_lobby == NULL){
        log_error("GET NEW CLIENT UNIQUE ID - Not valid params");
        return -1;
    }

    int unique_id = 0;
    client* existing_client = NULL;
    do{
        unique_id = rand() % 255;
        existing_client = find_client_by_id(unique_id, actual_lobby);
    }while(existing_client != NULL);

    return unique_id;
}
/*
* Function: free_lobby
* ------------------------
* Uvolni lobby
*
* actual_lobby: seznam clientu
*
* returns: void
*/
void free_lobby(lobby* actual_lobby){
    if(actual_lobby == NULL){
        log_error("FREE LOBBY- Not valid params");
        return;
    }

	client* pom = actual_lobby->first;
	while(pom != NULL){
		client* to_free = pom;
		pom = pom->next;

		free_client(to_free);
	}
	free(actual_lobby);
}
/*
* Function: print_clients
* ------------------------
* Vypise obsah listu
*
* actual_lobby: seznam clientu
*
* returns: void
*/
void print_clients(lobby* actual_lobby){
    if(actual_lobby == NULL){
        log_error("PRINT CLIENTS - Not valid params");
        return;
    }
    log_info("LOBBY");
	client* pom = actual_lobby->first;
	while(pom != NULL){
		log_info("        Client: %s ID:%d socket:%d", pom->name, pom->id, pom->socket);
		pom = pom->next;
	}
}
