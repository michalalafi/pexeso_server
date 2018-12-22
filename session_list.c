#include "session_list.h"


session_list* create_session_list(){
	session_list* new_session_list= (session_list*)malloc(sizeof(session_list));
	if(new_session_list == NULL){
		perror("SESSION LIST ERROR - Creating session list failed!");
		return NULL;
	}
	new_session_list->first = NULL;
	new_session_list->last = NULL;

	printf("SESSION LIST CREATED \n");
	return new_session_list;
}

void add_session_to_session_list(session* new_session, session_list* actual_session_list){
	session* first = actual_session_list->first;
	if(first == NULL){
		actual_session_list->first = new_session;
		actual_session_list->last = new_session;

		actual_session_list->first->next = NULL;
		actual_session_list->first->previous = NULL;
		printf("SESSION LIST - New session - %d - was added to start of list!\n", new_session->id);
		return;
	}

	session* last = actual_session_list->last;
	if(last == NULL){
		log_error("SESSION LIST ERROR - Adding in session list failed\n Last is NULL!");
		return;
	}
	last->next = new_session;
	new_session->previous = last;
	actual_session_list->last = new_session;
	printf("SESSION LIST - New session - %d - was added to end of list!\n", new_session->id);

}

void remove_session_from_session_list(session* session_to_remove, session_list* actual_session_list){
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
int is_session_list_empty(session_list* actual_session_list){
    log_trace("IS SESSION LIST EMPTY");
    if(actual_session_list == NULL){
        log_error("IS SESSION LIST EMPTY - Not valid params");
        return -1;
        printf("            SESSION LIST JE NULL!\n");
    }
    if(actual_session_list->first == NULL){
        return 1;
    }
    return 0;
}
int is_client_in_session_list(client* actual_client, session_list* actual_session_list){
    log_trace("IS CLIENT IN SESSION LIST");
    if(is_session_list_empty(actual_session_list)){
        printf("          VRACIME 0\n");
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
session* get_open_session(session_list* actual_session_list){
    log_trace("GET OPEN SESSION");
    if(is_session_list_empty(actual_session_list)){
        printf("Session list je prazdny \n");
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
session* get_session_by_client(client* actual_client, session_list* actual_session_list){
    log_trace("GET SESSION BY CLIENT");
    if(is_session_list_empty(actual_session_list)){
        printf("Get session - session list je prazdny \n");
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
session* get_session_by_id(int session_id, session_list* actual_session_list){
    //printf("FINDING SESSION BY ID: %d \n",session_id);
    if(is_session_list_empty(actual_session_list)){
        //printf("NO CLIENT FOUND DUE EMPTY LOBBY \n");
        return NULL;
    }

    session* pom = actual_session_list->first;
    while(pom != NULL){
        if(pom->id == session_id){
            printf("SESSION FOUND: ID = %d\n",pom->id);
            return pom;
        }
        pom = pom->next;
    }
    //printf("NO SESSION FOUND WITH ID: %d \n",session_id);
    return NULL;

}

int get_new_session_unique_id(session_list* actual_session_list){
    int unique_id = 0;
    session* existing_session = NULL;
    do{
        unique_id = rand();
        existing_session = get_session_by_id(unique_id, actual_session_list);
    }while(existing_session != NULL);

    return unique_id;
}

void print_session_list(session_list* actual_session_list){
    printf("    SESSION LIST\n");
    session* pom = actual_session_list->first;
    while(pom != NULL){
        printf("        Session id: %d first_client: %s second_client: %s \n", pom->id, pom->first_client==NULL?"NULL":pom->first_client->name,pom->second_client==NULL?"NULL":pom->second_client->name);

        pom = pom->next;
    }
}
