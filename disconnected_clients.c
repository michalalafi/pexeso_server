#include "disconnected_clients.h"

/*
* Function: create_disconnected_client
* ------------------------
* Vytvori odpojeneho clienta
*
* actual_client: aktualni client
*
* returns: noveho odpojeneho clienta
*/
disconnected_client* create_disconnected_client(client* actual_client){
    if(actual_client == NULL){
        log_error("CREATE DISCONNECTED CLIENT - Not valid params");
        return NULL;
    }

    disconnected_client* new_disconnected_client = (disconnected_client*) malloc(sizeof(disconnected_client));
    if(new_disconnected_client == NULL){
        log_error("CREATE DISCONNECTED CLIENT - Creating disconnected client failed");
        return NULL;
    }
    new_disconnected_client->client = actual_client;
    new_disconnected_client->time_of_disconnected = time(0);
    new_disconnected_client->next = NULL;
    new_disconnected_client->previous = NULL;

    return new_disconnected_client;
}
/*
* Function: create_disconnected_clients_list
* ------------------------
* Vytvori list odpojenych clientu
*
* returns: novy list odpojenych clientu
*/
disconnected_clients_list* create_disconnected_clients_list(){
    disconnected_clients_list* new_disconnected_clients_list = (disconnected_clients_list*) malloc(sizeof(disconnected_clients_list));
    if(new_disconnected_clients_list == NULL){
        log_error("CREATE DISCONNECTED CLIENTS LIST -  Creating disconnected clients list failed");
        return NULL;
    }
    new_disconnected_clients_list->first = NULL;
    new_disconnected_clients_list->last = NULL;

    log_info("DISCONNECTED CLIENTS LIST CREATED");
    return new_disconnected_clients_list;
}
/*
* Function: add_disconnected_client_to_disconnected_clients_list
* ------------------------
* Prida odpojeneho clienta do listu
*
* new_disconnected_client: pridavany client
* actual_disconnected_clients_list: list odpojenych clientu
*
* returns: void
*/
void add_disconnected_client_to_disconnected_clients_list(disconnected_client* new_disconnected_client, disconnected_clients_list* actual_disconnected_clients_list){
    if(new_disconnected_client == NULL || actual_disconnected_clients_list == NULL){
        log_error("ADD DISCONNECTED CLIENT TO DISCONNECTED CLIENTS LIST - Not valid params");
        return;
    }

	disconnected_client* first = actual_disconnected_clients_list->first;
	if(first == NULL){
		actual_disconnected_clients_list->first = new_disconnected_client;
		actual_disconnected_clients_list->last = new_disconnected_client;

		actual_disconnected_clients_list->first->next = NULL;
		actual_disconnected_clients_list->first->previous = NULL;
		return;
	}

	disconnected_client* last = actual_disconnected_clients_list->last;
	if(last == NULL){
		log_error("ADD DISCONNECTED CLIENT TO DISCONNECTED CLIENTS LIST - Adding in disconnected clients list failed, last is NULL");
		return;
	}
	last->next = new_disconnected_client;
	new_disconnected_client->previous = last;
	actual_disconnected_clients_list->last = new_disconnected_client;
}
/*
* Function: find_disconnected_client_by_id
* ------------------------
* Najde clienta podle id v listu odpojenych
*
* client_id: id clienta
* actual_disconnected_clients_list: list odpojenych clientu
*
* returns: nalezeny odpojeny client
*/
disconnected_client* find_disconnected_client_by_id(int client_id, disconnected_clients_list* actual_disconnected_clients_list){
    if(actual_disconnected_clients_list == NULL){
        log_error("FIND DISCONNECTED CLIENT BY ID - Not valid params");
        return NULL;
    }
    if(is_disconnected_clients_list_empty(actual_disconnected_clients_list)){
        log_error("FIND DISCONNECTED CLIENT BY ID - List is empty");
        return NULL;
    }

    disconnected_client* pom = actual_disconnected_clients_list->first;
    while(pom != NULL){
        if(pom->client->id == client_id){
            return pom;
        }
        pom = pom->next;
    }
    return NULL;
}
/*
* Function: is_disconnected_clients_list_empty
* ------------------------
* Je list prazdny?
*
* actual_disconnected_clients_list: list odpojenych clientu
*
* returns: Ano/Ne/Chyba
*/
int is_disconnected_clients_list_empty(disconnected_clients_list* actual_disconnected_clients_list){
    if(actual_disconnected_clients_list == NULL){
        log_error("IS DISCONNECTED CLIENTS LIST EMPTY - Not valid params");
        return -1;
    }
    if(actual_disconnected_clients_list->first == NULL && actual_disconnected_clients_list->last == NULL){
        return 1;
    }
    return 0;
}
/*
* Function: remove_disconnected_client_from_disconnected_clients_list
* ------------------------
* Odstrani odpojeneho clienta ze seznamu
*
* disconnected_client_to_remove: client na odstraneni
* actual_disconnected_clients_list: list odpojenych clientu
*
* returns: void
*/
void remove_disconnected_client_from_disconnected_clients_list(disconnected_client* disconnected_client_to_remove, disconnected_clients_list* actual_disconnected_clients_list){
	if(disconnected_client_to_remove == NULL || actual_disconnected_clients_list == NULL){
        log_error("REMOVE DISCONNECTED CLIENT FROM DISCONNECTED CLIENTS LIST - Not valid params");
        return;
	}

	disconnected_client* previous = disconnected_client_to_remove->previous;
	disconnected_client* next = disconnected_client_to_remove->next;
	// Je prvek prvni?
	if(actual_disconnected_clients_list->first->client->id == disconnected_client_to_remove->client->id){
		// A ma nasledujici?
		if(next != NULL){
			//Prenastavime zacatek na jeho dalsiho
			actual_disconnected_clients_list->first = next;
			actual_disconnected_clients_list->first->previous = NULL;
		}
		else{
			//Jinak je seznam prazdny
			actual_disconnected_clients_list->first = NULL;
			actual_disconnected_clients_list->last = NULL;
		}
	}
	// Je prvek posledni?
	else if(actual_disconnected_clients_list->last->client->id == disconnected_client_to_remove->client->id){
		// A ma predchoziho?
		if(previous != NULL){
			//Prenastavime konec na jeho predchoziho
			actual_disconnected_clients_list->last = previous;
			actual_disconnected_clients_list->last->next = NULL;
		}
		else{
			//Jinak je seznam prazdny
			actual_disconnected_clients_list->first = NULL;
			actual_disconnected_clients_list->last = NULL;
		}
	}
	// Je nekde mezi?
	if(next != NULL && previous != NULL){
		// Preskocime ho
		previous->next = next;
		next->previous = previous;
	}
	disconnected_client_to_remove->next = NULL;
	disconnected_client_to_remove->previous = NULL;
}
/*
* Function: free_disconnected_client
* ------------------------
* Uvolni odpojeneho clienta
*
* actual_disconnected_client: odpojeny client
*
* returns: void
*/
void free_disconnected_client(disconnected_client* actual_disconnected_client){
    if(actual_disconnected_client == NULL){
        log_error("FREE DISCONNECTED CLIENT - Not valid params");
        return;
    }
    actual_disconnected_client->client = NULL;
    //Resetujeme parametry
    actual_disconnected_client->next = NULL;
    actual_disconnected_client->previous = NULL;
    //Uvolnime odpojeneho klienta
    free(actual_disconnected_client);
}
/*
* Function: free_disconnected_clients_list
* ------------------------
* Uvolni list odpojenych clientu
*
* actual_disconnected_clients_list: list odpojenych clientu
*
* returns: void
*/
void free_disconnected_clients_list(disconnected_clients_list* actual_disconnected_clients_list){
    if(actual_disconnected_clients_list == NULL){
        log_error("FREE DISCONNECTED CLIENTS LIST - Not valid params");
        return;
    }
    disconnected_client* pom = actual_disconnected_clients_list->first;
    while(pom != NULL){
        disconnected_client* to_free = pom;
        pom = pom->next;
        free_disconnected_client(to_free);
    }
    free(actual_disconnected_clients_list);
}
/*
* Function: print_disconnected_clients
* ------------------------
* Vypise list odpojenych clientu
*
* actual_disconnected_clients_list: list odpojenych clientu
*
* returns: void
*/
void print_disconnected_clients(disconnected_clients_list* actual_disconnected_clients_list){
    if(actual_disconnected_clients_list == NULL){
        log_error("PRINT DISCONNECTED CLIENTS - Not valid params");
        return;
    }
    log_info("DISCONNECTED CLIENTS");
    disconnected_client* pom = actual_disconnected_clients_list->first;
    while(pom != NULL){
        struct tm* tm_info = localtime(&pom->time_of_disconnected);
        char buffer[26];

        strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info );
        log_info("        Client: %s ID:%d socket:%d disconnected_time:%s", pom->client->name, pom->client->id, pom->client->socket, buffer);
        pom = pom->next;
    }
}
