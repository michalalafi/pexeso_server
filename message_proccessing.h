#ifndef MESSAGE_PROCESSING_H
#define MESSAGE_PROCESSING_H

#include <stdio.h>
#include <stdlib.h>
#include "client.h"
#include <string.h>

#define MAX_DELIMITERS 2
#define MAX_PARTS 3
#define PARTS_DELIMITER_C '|'
#define PARTS_DELIMITER_S "|"

typedef struct MESSAGE{
	int client_id;
	int action;
    char* params;
}message;

message* extract_message(char* raw_message);

char** split_parts(char* raw_message);

int count_of_delimiter(char* string, char delimiter);

#endif
