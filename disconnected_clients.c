#include "disconnected_clients.h"


disconnected_client* create_disconnected_client(client* actual_client){
    disconnected_client* new_disconnected_client = (disconnected_client*) malloc(sizeof(disconnected_client));
    if(new_disconnected_client == NULL){
        printf("DISCONNECTED CLIENT ERROR - Creating disconnected client failed!");
        return NULL;
    }
    new_disconnected_client->client = actual_client;
    new_disconnected_client->time_of_disconnected = time(0);
    new_disconnected_client->next = NULL;
    new_disconnected_client->previous = NULL;

    return new_disconnected_client;
}

disconnected_clients_list* create_disconnected_clients_list(){
    disconnected_clients_list* new_disconnected_clients_list = (disconnected_clients_list*) malloc(sizeof(disconnected_clients_list));
    if(new_disconnected_clients_list == NULL){
        printf("DISCONNECTED CLIENTS LIST ERROR - Creating disconnected clients list failed!");
        return NULL;
    }
    new_disconnected_clients_list->first = NULL;
    new_disconnected_clients_list->last = NULL;

    printf("DISCONNECTED CLIENTS LIST CREATED \n");
    return new_disconnected_clients_list;
}

void add_disconnected_client_to_disconnected_clients_list(disconnected_client* new_disconnected_client, disconnected_clients_list* actual_disconnected_clients_list){
	disconnected_client* first = actual_disconnected_clients_list->first;
	if(first == NULL){
		actual_disconnected_clients_list->first = new_disconnected_client;
		actual_disconnected_clients_list->last = new_disconnected_client;

		actual_disconnected_clients_list->first->next = NULL;
		actual_disconnected_clients_list->first->previous = NULL;
		printf("DISCONNECTED CLIENTS LIST - New disconnected client - %d - was added to start of list!\n", new_disconnected_client->client->id);
		return;
	}

	disconnected_client* last = actual_disconnected_clients_list->last;
	if(last == NULL){
		log_error("DISCONNECTED CLIENTS LIST - Adding in disconnected clients list failed, last is NULL!");
		return;
	}
	last->next = new_disconnected_client;
	new_disconnected_client->previous = last;
	actual_disconnected_clients_list->last = new_disconnected_client;
	printf("DISCONNECTED CLIENTS LIST- New disconnected client - %s - was added to end of list!\n", new_disconnected_client->client->name);
}
disconnected_client* find_disconnected_client_by_id(int client_id, disconnected_clients_list* actual_disconnected_clients_list){
    log_trace("FIND DISCONNECTED CLIENT BY ID");
    if(is_disconnected_clients_list_empty(actual_disconnected_clients_list)){
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
int is_disconnected_clients_list_empty(disconnected_clients_list* actual_disconnected_clients_list){
    if(actual_disconnected_clients_list->first == NULL && actual_disconnected_clients_list->last == NULL){
        return 1;
    }
    return 0;
}
void remove_disconnected_client_from_disconnected_clients_list(disconnected_client* disconnected_client_to_remove, disconnected_clients_list* actual_disconnected_clients_list){
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
	if( next != NULL && previous != NULL){
		// Preskocime ho
		previous->next = next;
		next->previous = previous;
	}
	disconnected_client_to_remove->next = NULL;
	disconnected_client_to_remove->previous = NULL;
}
void free_disconnected_client(disconnected_client* actual_disconnected_client){
    if(actual_disconnected_client == NULL){
        return;
    }
    //Uvolnime klienta
    free_client(actual_disconnected_client->client);
    //Resetujeme parametry
    actual_disconnected_client->next = NULL;
    actual_disconnected_client->previous = NULL;
    //Uvolnime odpojeneho klienta
    free(actual_disconnected_client);
}
void print_disconnected_clients(disconnected_clients_list* actual_disconnected_clients_list){
    log_info("DISCONNECTED CLIENTS");
    disconnected_client* pom = actual_disconnected_clients_list->first;
    while(pom != NULL){
        struct tm* tm_info = localtime(&pom->time_of_disconnected);
        char buffer[26];

        strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info );
        log_info("        Client: %s ID:%d socket:%d disconnected_time:%s\n", pom->client->name, pom->client->id, pom->client->socket, buffer);
        pom = pom->next;
    }
}
