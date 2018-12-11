#include "utils.h"


void swap (char ** array, int i, int j)
{
	//printf("Vstupuje: '%s' a '%s' \n", array[i],array[j]);
	int a_length = strlen(array[i]) + 1;
	//printf("Ziskano a: %d\n", a_length);
	char* temp = (char*) malloc(a_length * sizeof(char));
	//	printf("Vytvoren temp\n");
	strcpy(temp,array[i]);
	//	printf("Temp: '%s' \n", temp);
	int b_length = strlen(array[j]) + 1;
	//	printf("Ziskano b: %d\n", b_length);
	array[i] = (char*) malloc(b_length * sizeof(char));

	strcpy(array[i],array[j]);
	//	printf("Array[i]: '%s' \n", array[i]);
	array[j] = (char*) malloc(a_length * sizeof(char));
	strcpy(array[j],temp);
	//	printf("Array[j]: '%s' \n", array[j]);
	free(temp);

}
void shuffle(char** array, int array_length ){
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
  		//	printf("------------I: %d a J: %d \n",i,j);
        // Swap array[i] with the element at random index j
        swap(array, i, j);
    }
}
int duplicate(char** array, int array_length){
	int new_array_length = array_length * 2;
	array = (char**) realloc(array,new_array_length * sizeof(char*));

	int i = array_length;
	for(; i < new_array_length; i++){
		array[i] = (char *) malloc((strlen(array[i%array_length])+1)* sizeof(char));
		strcpy(array[i], array[i%array_length]);
	}
	return new_array_length;
}

void shrink_array(char** array, int new_array_length){
	char** temp = realloc(array, sizeof(char*) * new_array_length);
	if(temp){
		array = temp;
	}
}

void replace_new_line_char(char* string_to_replace){
    int i = 0;
    for(i = strlen(string_to_replace); i > 0; i--){
        if(string_to_replace[i] == '\n'){
            string_to_replace[i] = '\0';
        }
    }
}

long convert_string_to_long(char* string){
    char* end;
    long number = strtol(string,&end, 0);
    if(*end == '\0'){
        return number;
    }
    return -1;
}
