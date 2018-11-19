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
/* Function: get_sounds_from_folder
* ------------------------
* Ziska zvuky ze zadane slozky a nastavi delku pole
*
* folder_name: nazev slozky
* sound_length: velikost pole
*
* returns: pole zvuku
*/
char** get_sounds_from_folder(char* folder_path, int* sounds_length){
    if(folder_path == NULL){
        printf("GET SOUNDS FROM FOLDER ERROR - No foldername passed! \n");
        return NULL;
    }
	DIR *dir;
	struct dirent *ep;
	int sounds_size = 1000; // Defaultni delka pole
    int i = 0,
        offset = 10; // Delka zvetseni pole

	char** sounds = (char **) malloc( sounds_size * sizeof(char*)); // Vyhrazeni mista pro zvuky

	dir = opendir(folder_path);
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
char** get_sounds_for_pexeso(char* folder_path, int pexeso_count){
    // Pro ulozeni kolik je zvuku ve slozce
    int sounds_length = 0;
	// Ziskame vsechny zvuky v zadane slozce
	char** sounds = get_sounds_from_folder(folder_path, &sounds_length);
	if(sounds == NULL || sounds_length == 0)
	{
        printf("SOUNDS LOADING ERROR - No sounds loaded!\n");
        return NULL;
	}
	print_all_sounds(sounds,sounds_length);
	// Promichame je
	printf("SHUFFLE\n");
	shuffle(sounds, sounds_length);
	print_all_sounds(sounds,sounds_length);

	// Vezmeme jich polovicku
	printf("SHRINK\n");
    int half_count = pexeso_count / 2;
    sounds_length = half_count;
	shrink_array(sounds, half_count);
	print_all_sounds(sounds,sounds_length);
	// Zduplikujeme je
	printf("DUPLICATE\n");
	sounds_length = duplicate(sounds,sounds_length);
	print_all_sounds(sounds,sounds_length);
	// Promichame je
	printf("SHUFFLE\n");
	shuffle(sounds, sounds_length);
	print_all_sounds(sounds,sounds_length);

	if(sounds_length != pexeso_count){
        printf("SOUNDS LOADING ERROR - Wanted count has not been achieved! \n");
	}
    printf("SOUNDS LOADED SUCCESSFULLY! \n");
    return sounds;

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
	for(; i < sound_length; i++)
	{
		printf("%d - ",i);
		puts(sounds[i]);
	}
}

