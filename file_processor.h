#ifndef FILE_PROCESSOR
#define FILE_PROCESSOR

#include <stdio.h>

char** get_sounds_for_puzzle(char* folder_name, int * sounds_length);

char *get_filename_ext(const char *filename);

void print_all_sounds(char** sounds, int sound_length);

#endif
