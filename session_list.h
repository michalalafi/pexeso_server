#ifndef SESSION_LIST_H
#define SESSION_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include "session.h"
#include "lobby.h"

typedef struct SESSION_LIST{
	struct SESSION* first;
	struct SESSION* last;
}session_list;

session_list* create_session_list();
void add_session_to_session_list(session* new_session, session_list* actual_session_list);
void remove_session_from_session_list(session* session_to_remove, lobby* actual_session_list);
session* get_open_session(session_list* actual_session_list);
int is_session_list_empty(session_list* actual_session_list);
session* get_session_by_client(client* actual_client, session_list* actual_session_list);

#endif
