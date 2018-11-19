#ifndef FILE_PROCESSOR_H
#define FILE_PROCESSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include "utils.h"

char** get_sounds_from_folder(char* folder_path, int * sounds_length);

char** get_sounds_for_pexeso(char* folder_path, int pexeso_count);

char *get_filename_ext(char *filename);

void print_all_sounds(char** sounds, int sound_length);

#endif
