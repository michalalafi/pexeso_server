#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void swap (char ** array, int i, int j);
void shuffle(char** array, int array_length );
int duplicate(char** array, int array_length);
void shrink_array(char** array, int* array_length, int new_array_length);
#endif
