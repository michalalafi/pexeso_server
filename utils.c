#include "utils.h"


void swap (char ** array, int i, int j)
{ 
	char* temp = (char*) malloc(strlen(array[i]) * sizeof(char));
		
	strcpy(temp,array[i]);
	array[i] = (char*) realloc(array[i],strlen(array[j]) * sizeof(char));
	
	strcpy(array[i],array[j]);
	
	array[j] = (char*) realloc(array[j],strlen(temp) * sizeof(char));
	strcpy(array[j],temp);
	
	free(temp);
	
}
void shuffle(char** array, int array_length ){
	// Use a different seed value so that we don't get same 
    // result each time we run this program 
    srand ( time(NULL) );
    // Start from the last element and swap one by one. We don't 
    // need to run for the first element that's why i > 0
    int i = array_length-1;
    for (i; i > 0; i--) 
    { 
        // Pick a random index from 0 to i 
        int j = rand() % (i+1); 
  
        // Swap array[i] with the element at random index j
        swap(array, i, j); 
    } 
}
int duplicate(char** array, int array_length){
	int new_array_length = array_length * 2;
	array = (char**) realloc(array,new_array_length * sizeof(char*));
	
	int i = array_length;
	for(i; i < new_array_length; i++){
		array[i] = (char *) malloc(strlen(array[i%array_length])* sizeof(char));
		strcpy(array[i], array[i%array_length]);
	} 
	return new_array_length;
}

void shrink_array(char** array, int * array_length, int new_array_length){
	char** temp = realloc(array, sizeof(char*) * new_array_length);
	if(temp){
		array = temp;
		*array_length = new_array_length;
	}
}
