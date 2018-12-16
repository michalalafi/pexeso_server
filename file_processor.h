#ifndef FILE_PROCESSOR_H
#define FILE_PROCESSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include "utils.h"

#include "log.h"

char** get_sounds_from_folder(char* folder_path, int * sounds_length);

char** get_sounds_for_pexeso(char* folder_path, int pexeso_count);

char *get_filename_ext(char *filename);

int is_folder_with_sounds_avaible(char* folder_path);

void print_all_sounds(char** sounds, int sound_length);

#endif
