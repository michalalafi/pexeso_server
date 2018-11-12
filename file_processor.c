#include "file_processor.h"

/* Function: get_filename_ext
* ------------------------
* Z vlozeneho stringu zjisti koncovku souboru
*
* filename: nazev souboru
*
* returns: koncovka souboru
*/
char* get_filename_ext(char* filename) {
    if(filename == NULL){
        printf("FILENAME EXTENSTION ERROR - No filename passed! \n");
        return NULL;
    }
    char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}
/* Function: print_all_sounds
* ------------------------
* Vytiskne pole zvuku
*
* sounds: pole zvuku
* sound_length: velikost pole
*
* returns: void
*/
void print_all_sounds(char** sounds, int sound_length)
{
    if(sounds == NULL){
        printf("PRINT SOUNDS ERROR - No sounds passed! \n");
        return;
    }
	int i = 0;
	for(i; i < sound_length; i++)
	{
		printf("%d - ",i);
		puts(sounds[i]);
	}
}
/* Function: get_sounds_from_folder
* ------------------------
* Ziska zvuky ze zadane slozky a nastavi delku pole
*
* folder_name: nazev slozky
* sound_length: velikost pole
*
* returns: pole zvuku
*/
char** get_sounds_from_folder(char* folder_name, int* sounds_length){
    if(folder_name == NULL){
        printf("GET SOUNDS FROM FOLDER ERROR - No foldername passed! \n");
        return NULL;
    }
	DIR *dir;
	struct dirent *ep;
	int sounds_size = 1000; // Defaultni delka pole
    int i = 0,
        offset = 10; // Delka zvetseni pole

	char** sounds = (char **) malloc( sounds_size * sizeof(char*)); // Vyhrazeni mista pro zvuky

	dir = opendir(folder_name);
	if(dir != NULL){
		while(ep = readdir(dir))
		{
            // Je koncovka mp3?
			if(strcmp(get_filename_ext(ep->d_name),"mp3") == 0){
                // + 1 for \0 character //
				sounds[i] = (char*)malloc((strlen(ep->d_name) + 1) * sizeof(char));
				strcpy(sounds[i],ep->d_name);
				i++;
			}
            // Pokud dojde kapacita pole, zvetsime ji
			if(i >= sounds_size)
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
	*sounds_length = i;
	return sounds;
}

