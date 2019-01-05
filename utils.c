#include "utils.h"

/*
* Function: swap
* ------------------------
* Prohodi stringy v poli na indexech i a j
*
* array: pole stringu
* i: prvni index
* j: druhy index
*
* returns: void
*/
void swap(char** array, int i, int j)
{
    if(array == NULL){
        log_error("SWAP - Not valid params");
        return;
    }
	int a_length = strlen(array[i]) + 1;

	char* temp = (char*) malloc(a_length * sizeof(char));

	strcpy(temp,array[i]);

	int b_length = strlen(array[j]) + 1;

	array[i] = (char*) malloc(b_length * sizeof(char));

	strcpy(array[i],array[j]);

	array[j] = (char*) malloc(a_length * sizeof(char));
	strcpy(array[j],temp);

	free(temp);

}
/*
* Function: shuffle
* ------------------------
* Prohodi prvky v poli, zamicha je
*
* array: pole stringu
* array_length: delka pole
*
* returns: void
*/
void shuffle(char** array, int array_length ){
    if(array == NULL){
        log_error("SHUFFLE - Not valid params");
        return;
    }
	// Use a different seed value so that we don't get same
    // result each time we run this program
    srand ( time(NULL) );
    // Start from the last element and swap one by one. We don't
    // need to run for the first element that's why i > 0
    int i = array_length - 1;
    for (; i > 0; i--)
    {
        // Pick a random index from 0 to i
        int j = rand() % (i + 1);
        // Swap array[i] with the element at random index j
        swap(array, i, j);
    }
}
/*
* Function: duplicate
* ------------------------
* Zduplikuje prvky v poli
*
* array: pole stringu
* array_length: delka pole
*
* returns: delka noveho pole
*/
int duplicate(char** array, int array_length){
    if(array == NULL){
        log_error("DUPLICATE - Not valid params");
        return 0;
    }

	int new_array_length = array_length * 2;
	array = (char**) realloc(array,new_array_length * sizeof(char*));

	int i = array_length;
	for(; i < new_array_length; i++){
		array[i] = (char *) malloc((strlen(array[i%array_length])+1)* sizeof(char));
		strcpy(array[i], array[i%array_length]);
	}
	return new_array_length;
}
/*
* Function: shrink_array
* ------------------------
* Zmeni velikost pole na zadanou delku
*
* array: pole stringu
* new_array_length: nova delka pole
*
* returns: void
*/
void shrink_array(char** array, int new_array_length){
    if(array == NULL){
        log_error("SHRINK ARRAY - Not valid params");
        return;
    }

	char** temp = realloc(array, sizeof(char*) * new_array_length);
	if(temp){
		array = temp;
	}
}
/*
* Function: replace_new_line_char
* ------------------------
* Nahradi konec radky koncem stringu
*
* string_to_replace: string ktery se upravi
*
* returns: void
*/
void replace_new_line_char(char* string_to_replace){
    if(string_to_replace == NULL){
        log_error("REPLACE NEW LINE CHAR - Not valid params");
        return;
    }
    int i = 0;
    for(i = strlen(string_to_replace); i > 0; i--){
        if(string_to_replace[i] == '\n'){
            string_to_replace[i] = '\0';
        }
    }
}
/*
* Function: convert_string_to_long
* ------------------------
* Prevede string na long
*
* string: string na prevedeni
*
* returns: long/error
*/
long convert_string_to_long(char* string){
    if(string == NULL){
        log_error("CONVERT STRING TO LONG - Not valid params");
        return -1;
    }
    char* end;
    long number = strtol(string,&end, 0);
    if(*end == '\0'){
        return number;
    }
    return -1;
}
