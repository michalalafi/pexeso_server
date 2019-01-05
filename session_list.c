#include "session_list.h"

/*
* Function: create_session_list
* ------------------------
* Vytvori list session
*
* returns: novy list session
*/
session_list* create_session_list(){
	session_list* new_session_list= (session_list*)malloc(sizeof(session_list));
	if(new_session_list == NULL){
		log_error("CREATE SESSION LIST - Creating session list failed");
		return NULL;
	}
	new_session_list->first = NULL;
	new_session_list->last = NULL;

	log_info("SESSION LIST CREATED");
	return new_session_list;
}
/*
* Function: add_session_to_session_list
* ------------------------
* Prida sessionu do listu
*
* new_session: pridavana sessiona
* actual_session_list: list session
*
* returns: void
*/
void add_session_to_session_list(session* new_session, session_list* actual_session_list){
    if(new_session == NULL || actual_session_list == NULL){
        log_error("ADD SESSION TO SESSION LIST - Not valid params");
        return;
    }
	session* first = actual_session_list->first;
	if(first == NULL){
		actual_session_list->first = new_session;
		actual_session_list->last = new_session;

		actual_session_list->first->next = NULL;
		actual_session_list->first->previous = NULL;
		return;
	}

	session* last = actual_session_list->last;
	if(last == NULL){
		log_error("ADD SESSION TO SESSION LIST - Adding in session list failed, last is NULL!");
		return;
	}
	last->next = new_session;
	new_session->previous = last;
	actual_session_list->last = new_session;
}
/*
* Function: remove_session_from_session_list
* ------------------------
* Odstrani sessionu z listu
*
* session_to_remove: odebirana sessiona
* actual_session_list: list session
*
* returns: void
*/
void remove_session_from_session_list(session* session_to_remove, session_list* actual_session_list){
    if(session_to_remove == NULL || actual_session_list == NULL){
        log_error("REMOVE SESSION FROM SESSION LIST - Not valid params");
        return;
    }
	session* previous = session_to_remove->previous;
	session* next = session_to_remove->next;
	// Je prvek prvni?
	if(actual_session_list->first->id == session_to_remove->id){
		// A ma nasledujici?
		if(next != NULL){
			//Prenastavime zacatek na jeho dalsiho
			actual_session_list->first = next;
			actual_session_list->first->previous = NULL;
		}
		else{
			//Jinak je seznam prazdny
			actual_session_list->first = NULL;
			actual_session_list->last = NULL;
		}
	}
	// Je prvek posledni?
	else if(actual_session_list->last->id == session_to_remove->id){
		// A ma predchoziho?
		if(previous != NULL){
			//Prenastavime konec na jeho predchoziho
			actual_session_list->last = previous;
			actual_session_list->last->next = NULL;
		}
		else{
			//Jinak je seznam prazdny
			actual_session_list->first = NULL;
			actual_session_list->last = NULL;
		}
	}
	// Je nekde mezi?
	if( next != NULL && previous != NULL){
		// Preskocime ho
		previous->next = next;
		next->previous = previous;
	}
	session_to_remove->next = NULL;
	session_to_remove->previous = NULL;
}
/*
* Function: is_session_list_empty
* ------------------------
* Je list session prazdny
*
* actual_session_list: list session
*
* returns: Ano/Ne
*/
int is_session_list_empty(session_list* actual_session_list){
    log_trace("IS SESSION LIST EMPTY");
    if(actual_session_list == NULL){
        log_error("IS SESSION LIST EMPTY - Not valid params");
        return -1;
    }
    if(actual_session_list->first == NULL){
        return 1;
    }
    return 0;
}
/*
* Function: is_client_in_session_list
* ------------------------
* Je client v list session
*
* actual_client: client
* actual_session_list: list session
*
* returns: Ano/Ne
*/
int is_client_in_session_list(client* actual_client, session_list* actual_session_list){
    log_trace("IS CLIENT IN SESSION LIST");
    if(actual_client == NULL || actual_session_list == NULL){
        log_error("IS CLIENT IN SESSION LIST - Not valid params");
        return 0;
    }
    if(is_session_list_empty(actual_session_list)){
        log_error("IS CLIENT IN SESSION LIST - Session list is empty");
        return 0;
    }
    session* pom = actual_session_list->first;
    while(pom != NULL){
        if(is_client_in_session(actual_client, pom)){
            return 1;
        }
        pom = pom->next;
    }
    return 0;
}
/*
* Function: get_open_session
* ------------------------
* Vraci otevrenou sessionu
*
* actual_session_list: list session
*
* returns: otevrena sessiona
*/
session* get_open_session(session_list* actual_session_list){
    log_trace("GET OPEN SESSION");
    if(actual_session_list == NULL){
        log_error("GET OPEN SESSION - Not valid params");
        return NULL;
    }
    if(is_session_list_empty(actual_session_list)){
        log_error("GET OPEN SESSION - Session list is empty");
        return NULL;
    }
    session* pom =  actual_session_list->first;
    while(pom != NULL){
        if(is_session_open(pom)){
            return pom;
        }
        pom = pom->next;
    }
    return NULL;
}
/*
* Function: get_session_by_client
* ------------------------
* Vraci sessionu ve ktere je client
*
* actual_client: client
* actual_session_list: list session
*
* returns: sessiona ve ktere je client
*/
session* get_session_by_client(client* actual_client, session_list* actual_session_list){
    log_trace("GET SESSION BY CLIENT");
    if(actual_client == NULL || actual_session_list == NULL){
        log_error("GET SESSION BY CLIENT - Not valid params");
        return NULL;
    }
    if(is_session_list_empty(actual_session_list)){
        log_error("GET SESSION BY CLIENT - Session list is empty");
        return NULL;
    }
    session* pom = actual_session_list->first;
    while(pom != NULL){
        if(is_client_in_session(actual_client,pom)){
            return pom;
        }
        pom = pom->next;
    }
    return NULL;
}
/*
* Function: get_session_by_id
* ------------------------
* Vraci sessionu podle jeji id
*
* session_id: id sessiony
* actual_session_list: list session
*
* returns: sessiona podle id
*/
session* get_session_by_id(int session_id, session_list* actual_session_list){
    if(actual_session_list == NULL){
        log_error("GET SESSION BY ID - Not valid params");
        return NULL;
    }
    if(is_session_list_empty(actual_session_list)){
        log_error("GET SESSION BY ID -Session list is empty");
        return NULL;
    }
    session* pom = actual_session_list->first;
    while(pom != NULL){
        if(pom->id == session_id){
            return pom;
        }
        pom = pom->next;
    }
    return NULL;
}
/*
* Function: get_new_session_unique_id
* ------------------------
* Vraci unikatni id nove sessiony
*
* actual_session_list: list session
*
* returns: nove uniktani id
*/
int get_new_session_unique_id(session_list* actual_session_list){
    if(actual_session_list == NULL){
        log_error("GET NEW SESSION UNIQUE ID - Not valid params");
        return -1;
    }
    int unique_id = 0;
    session* existing_session = NULL;
    do{
        unique_id = rand() % 255;
        existing_session = get_session_by_id(unique_id, actual_session_list);
    }while(existing_session != NULL);

    return unique_id;
}
/*
* Function: free_session_list
* ------------------------
* Uvolni list session
*
* actual_session_list: list session
*
* returns: void
*/
void free_session_list(session_list* actual_session_list){
    if(actual_session_list == NULL){
        log_error("FREE SESSION LIST - Not valid params");
        return;
    }
    session* pom = actual_session_list->first;
    while(pom != NULL){
        session* to_free = pom;
        pom = pom->next;

        free_session(to_free);
    }
    free(actual_session_list);
}

/*
* Function: print_session_list
* ------------------------
* Vypise sessiony v listu
*
* actual_session_list: list session
*
* returns: void
*/
void print_session_list(session_list* actual_session_list){
    if(actual_session_list == NULL){
        log_error("PRINT SESSION LIST - Not valid params");
        return;
    }
    log_info("SESSION LIST");
    session* pom = actual_session_list->first;
    while(pom != NULL){
        log_info("        Session id: %d first_client: %s second_client: %s", pom->id, pom->first_client==NULL?"NULL":pom->first_client->name,pom->second_client==NULL?"NULL":pom->second_client->name);
        pom = pom->next;
    }
}
