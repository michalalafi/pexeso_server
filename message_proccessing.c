#include "message_proccessing.h"

message* create_message(){
    message* new_message = (message*) malloc(sizeof(message));
    if(new_message == NULL){
        printf("MESSAGE ERROR - Creating message failed! \n");
    }
    new_message->action = -50;
    new_message->client_id = -50;
    new_message->params = NULL;
    return new_message;
}

message* extract_message(char* raw_message){
    printf("%s -> ",raw_message);
    message* extracted_message = create_message();

    int delimiter_count = count_of_delimiter(raw_message, PARTS_DELIMITER_C);
    if(delimiter_count != MAX_DELIMITERS){
        return NULL;
    }
    char** parts = split_parts(raw_message);
    if(parts == NULL){
        return NULL;
    }

    if(parts[0] != NULL){
        char* end;
        long number = strtol(parts[0],&end, 0);
        if(*end == '\0'){
            extracted_message->client_id = number;
        }
        else{
            return NULL;
        }
    }
    if(parts[1] != NULL){
        char* end;
        long number = strtol(parts[1],&end, 0);
        if(*end == '\0'){
            extracted_message->action = number;
        }
        else{
            return NULL;
        }
    }
    if(parts[2] != NULL){
        extracted_message->params = malloc((strlen(parts[2]) +1) * sizeof(char));
        strcpy(extracted_message->params, parts[2]);
    }

    //printf("Struct message: \n message->client_id = %d \n message->action = %d \n message->params = %s \n", extracted_message->client_id,extracted_message->action,extracted_message->params);
    return extracted_message;
}

char* create_raw_message_for_client(int action, char* params){
    char* raw_message = (char*) malloc(1024 * sizeof(char));

    sprintf(raw_message,"%d|%s",action,params);

    printf("CREATED MESSAGE %s \n",raw_message);

    return raw_message;
}

char** split_parts(char* raw_message){
    //printf("SPLIT PARTS \n");
    if(raw_message == NULL){
        return NULL;
    }
    char** parts = (char**) malloc(sizeof(char*) * MAX_PARTS);
    int i = 0;
    for(i = 0; i < MAX_PARTS; i++){
        parts[i] = NULL;
    }
    // Prekopirujeme raw_message aby jsme ho neupravili
    char* split_pom = malloc((strlen(raw_message) + 1) * sizeof(char));
    strcpy(split_pom,raw_message);
    // Prvni rozdeleni
    char* split = strtok(split_pom, PARTS_DELIMITER_S);
    i = 0;
    while(split != NULL){
        if(i >= MAX_PARTS){
            return NULL;
        }
        parts[i] = (char*)malloc( (strlen(split) + 1) * sizeof(char));
        //printf("Split %s \n",split);
        //Nakopirovani vysledku
        strcpy(parts[i], split);
        // Dalsi rozdeleni
        split = strtok(NULL,"|");
        i++;
    }
    for(i = 0; i < MAX_PARTS; i++){
        printf("part[%d] = %s \n",i,parts[i]);
    }

    return parts;

}

int count_of_delimiter(char* string, char delimiter){
    int i = 0, count = 0;
    for(; i < strlen(string); i++){
        if(string[i] == delimiter)
            count++;
    }
    return count;
}




