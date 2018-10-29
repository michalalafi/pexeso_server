#ifndef FILE_PROCESSOR_H
#define FILE_PROCESSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

char** get_sounds_for_puzzle(char* folder_name, int * sounds_length);

char *get_filename_ext(char *filename);

void print_all_sounds(char** sounds, int sound_length);

#endif
