#include "message_proccessing.h"


/*
* Function: create_message
* ------------------------
* Vytvori novou zpravu
*
* returns: nova zprava
*/
message* create_message(){
    message* new_message = (message*) malloc(sizeof(message));
    if(new_message == NULL){
        log_error("CREATE MESSAGE - Creating message failed");
        return NULL;
    }
    new_message->action = -50;
    new_message->client_id = -50;
    new_message->params = NULL;
    return new_message;
}
/*
* Function: extract_message
* ------------------------
* Vytvori novou zpravu podle zpravy od clienta
*
* raw_message: surova zprava od clienta
*
* returns: nova zprava
*/
message* extract_message(char* raw_message){
    if(raw_message == NULL){
        log_error("EXTRACT MESSAGE - Not valid params");
        return NULL;
    }
    message* extracted_message = create_message();
    if(extracted_message == NULL){
        return NULL;
    }
    int delimiter_count = count_of_delimiter(raw_message, PARTS_DELIMITER_C);
    if(delimiter_count != MAX_DELIMITERS){
        return NULL;
    }
    char** parts = split_parts(raw_message);
    if(parts == NULL){
        return NULL;
    }

    if(parts[0] != NULL){
        long id = convert_string_to_long(parts[0]);
        if(id == -1){
            return NULL;
        }
        extracted_message->client_id = id;
    }
    if(parts[1] != NULL){
        long action = convert_string_to_long(parts[1]);
        if(action == -1){
            return NULL;
        }
        extracted_message->action = action;
    }
    if(parts[2] != NULL){
        extracted_message->params = malloc((strlen(parts[2]) +1) * sizeof(char));
        strcpy(extracted_message->params, parts[2]);
    }
    return extracted_message;
}
/*
* Function: create_raw_message_for_client
* ------------------------
* Vytvori novou zpravu pro clienta podle akce a parametru
*
* action: akce
* params: parametry
*
* returns: pripravena zprava pro clienta
*/
char* create_raw_message_for_client(int action, char* params){
    char* raw_message = (char*) malloc(1024 * sizeof(char));

    sprintf(raw_message,"%d%c%s%c",action,PARTS_DELIMITER_C,params,'\0');

    return raw_message;
}
/*
* Function: split_parts
* ------------------------
* Rozdeli string podle znaku na casti
*
* raw_message: string na rozdeleni
*
* returns: casti zpravy
*/
char** split_parts(char* raw_message){;
    if(raw_message == NULL){
        log_error("SPLIT PARTS - Not valid params");
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
        //Nakopirovani vysledku
        strcpy(parts[i], split);
        // Dalsi rozdeleni
        split = strtok(NULL,PARTS_DELIMITER_S);
        i++;
    }
    return parts;
}
/*
* Function: count_of_delimiter
* ------------------------
* Spocita kolikrat je rozdelovaci znak ve stringu
*
* string: string na prohledani
* delimiter: znak ktery hledame ve stringu
*
* returns: pocet vyskytu znaku
*/
int count_of_delimiter(char* string, char delimiter){
    if(string == NULL){
        log_error("COUNT OF DELIMITER - Not valid params");
        return 0;
    }

    int i = 0, count = 0;
    for(; i < strlen(string); i++){
        if(string[i] == delimiter)
            count++;
    }
    return count;
}
/*
* Function: free_message
* ------------------------
* Uvolni zpravu od clienta
*
* actual_message: message ktera ma byt uvolnena
*
* returns: void
*/
void free_message(message* actual_message){
    if(actual_message == NULL){
        log_error("FREE MESSAGE - Not valid params");
        return;
    }
    free(actual_message->params);
    free(actual_message);
}




