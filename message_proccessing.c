#include "message_proccessing.h"

message* extract_message(char* raw_message){
    printf("RAW MESSAGE: %s \n",raw_message);
    message* extracted_message = (message*) malloc(sizeof(message));


    // Rozdelime na casti podle |
    // Pak kazdou cast zpracujeme podle ID: ACTION: PARAMS: "
    // Pak budeme kontrolovat konzistenci
    // Obsahuje string 'ID:'
    char* ptr =strstr(raw_message,"ID:");
    if(ptr == NULL){
        return NULL;
    }
    // Pozice zacatku ID
    int pos = ptr - raw_message;

    char* each_delimiter[] = {"ID:","ACTION:","PARAMS:"};
    int i_deli = 0;
    char* split;
    printf("    Pozice ID je: %d \n",pos);
    printf("    Od pozice string je: %s\n",raw_message + pos);
    char* parts_delimiter = "|";
    char* to_split = malloc(sizeof(raw_message));
    strcpy(to_split,raw_message + pos);
    printf("    To split: -%s- \n",to_split);
    split = strtok(to_split, parts_delimiter);

    while(split != NULL){
        printf("    Split je: %s \n",split);
        char* pom = malloc(sizeof(split));
        strcpy(pom,split);
        printf("    POM je: %s \n",pom);
        char* pom_split = strtok(pom,each_delimiter[i_deli]);
        printf("    POM split je: %s \n",pom_split);

        char* end;
        long number = strtol(pom_split,&end, 0);
        if(*end == '\0'){
            printf("    ID je:%d \n",number);
        }
        else{
            printf("MESSAGE CORUPTED \n");
            return NULL;
        }
        split = strtok(NULL, parts_delimiter);
    }
    return NULL;
  /*  if(split != NULL){

    }
    split = strtok(NULL, delimiter);
    if(split != NULL)
        printf("    Split je: %s \n",split);
    split = strtok(NULL, delimiter);
    if(split != NULL)
        printf("    Split je: %s \n",split);
    printf("MESSAGE VALID \n"); */
}

//char*[]





