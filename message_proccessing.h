#ifndef MESSAGE_PROCESSING_H
#define MESSAGE_PROCESSING_H

#include <stdio.h>
#include <stdlib.h>
#include "client.h"
#include <string.h>

typedef struct MESSAGE{
	int client_id;
	int action;
    char* params;
}message;

message* extract_message(char* raw_message);

#endif
