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
        log_error("GET FILENAME EXTENSION - Not valid params");
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
        log_error("GET SOUNDS FROM FOLDER - Not valid params");
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
		while((ep = readdir(dir)))
		{
            // Je koncovka mp3?
			if(strcmp(get_filename_ext(ep->d_name),"wav") == 0){
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
		log_error("GET SOUNDS FROM FOLDER - Couldn't open the directory");
		return NULL;
	}
	log_info("GET SOUNDS FROM FOLDER - All sounds loaded");
	*sounds_length = i;
	return sounds;
}
/* Function: get_sounds_for_pexeso
* ------------------------
* Zpracuje zvuky a pripravi je pro novou hru
*
* folder_path: cesta ke slozce
* pexeso_count: pocet pexes
*
* returns: pole zvuku
*/
char** get_sounds_for_pexeso(char* folder_path, int pexeso_count){
    if(folder_path == NULL){
        log_error("GET SOUNDS FOR PEXESO - Not valid params");
        return NULL;
    }
    // Pro ulozeni kolik je zvuku ve slozce
    int sounds_length = 0;
	// Ziskame vsechny zvuky v zadane slozce
	char** sounds = get_sounds_from_folder(folder_path, &sounds_length);
	if(sounds == NULL || sounds_length == 0)
	{
        log_error("GET SOUNDS FOR PEXESO - No sounds loaded");
        return NULL;
	}
	if(sounds_length < pexeso_count/2){
        log_error("GET SOUNDS FOR PEXESO - Not enough sounds loaded");
        return NULL;
	}
	// Promichame je
	shuffle(sounds, sounds_length);
	print_all_sounds(sounds,sounds_length);
	// Vezmeme jich polovicku
    int half_count = pexeso_count / 2;
    sounds_length = half_count;
	shrink_array(sounds, half_count);
	// Zduplikujeme je
	sounds_length = duplicate(sounds,sounds_length);
	// Promichame je
	shuffle(sounds, sounds_length);

	if(sounds_length != pexeso_count){
        log_error("GET SOUNDS FOR PEXESO - Wanted count has not been achieved");
        return NULL;
	}
    log_info("GET SOUNDS FOR PEXESO - Sounds prepared for game");
    return sounds;
}
/* Function: is_folder_with_sounds_avaible
* ------------------------
* Zjisti jestli slozka se zvuky je dostupna
*
* folder_path: cesta ke slozce
*
* returns: Ano/Ne
*/
int is_folder_with_sounds_avaible(char* folder_path){
    if(folder_path == NULL){
        log_error("IS FOLDER WITH SOUNDS AVAIBLE - Not valid params");
        return 0;
    }
    DIR *dir = opendir(folder_path);
    if(dir != NULL){
        (void) closedir (dir);
        return 1;
    }
    (void) closedir (dir);
    return 0;
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
        log_error("PRINT ALL SOUNDS - Not valid params");
        return;
    }
	int i;
	for(i = 0; i < sound_length; i++)
	{
		log_info("%d - ",i);
		puts(sounds[i]);
	}
}

