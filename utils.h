#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "log.h"

void swap (char ** array, int i, int j);

void shuffle(char** array, int array_length );

int duplicate(char** array, int array_length);

void shrink_array(char** array, int new_array_length);

void replace_new_line_char(char* string_to_replace);

long convert_string_to_long(char* string);
#endif
