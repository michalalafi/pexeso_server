#ifndef SESSION_LIST_H
#define SESSION_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include "session.h"

typedef struct SESSION_LIST{
	struct SESSION* first;
	struct SESSION* last;
}session_list;

session_list* create_session_list();
void add_session_to_session_list(session* new_session, session_list* actual_session_list);

#endif
