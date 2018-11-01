#include "file_processor.h"

char *get_filename_ext(char *filename) {
    char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}
void print_all_sounds(char** sounds, int sound_length)
{
	int i = 0;
	for(i; i < sound_length; i++)
	{
		printf("%d - ",i);
		puts(sounds[i]);
	}
}
char** get_sounds_for_puzzle(char* folder_name, int* sounds_length){
	DIR *dir;
	struct dirent *ep;

	int sounds_size = 1000;
	char** sounds = (char **) malloc( sounds_size * sizeof(char*));

	int sounds_count = 0;
	int offset = 10;
	int i = 0;

	dir = opendir(folder_name);
	if(dir != NULL){
		while(ep = readdir(dir))
		{
			if(strcmp(get_filename_ext(ep->d_name),"mp3") == 0){
                /* + 1 for \0 character */
				sounds[i] = (char*)malloc((strlen(ep->d_name) + 1) * sizeof(char));
				strcpy(sounds[i],ep->d_name);
				sounds_count++;
				i++;
			}

			if(sounds_count >= sounds_size)
			{
				sounds = (char **) realloc(sounds, (sounds_size + offset) * sizeof(char*));
				sounds_size += offset;
			}
		}
		(void) closedir (dir);
	}
	else{
		perror("FOLDER ERROR - Couldn't open the directory");
		return NULL;
	}
	printf("---All sounds loaded!---\n");
	*sounds_length = sounds_count;
	return sounds;
}

