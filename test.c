#include "test.h"

int number_of_clients = 6;

client** test_client_creation(lobby* lobby){

    char* names[] = {"Pepa","Anca","Karel","Michal","Ota","Pavel"};
    client** clients = (client**) malloc(sizeof(client*) * number_of_clients);

    int i = 0;
    for(i;i < number_of_clients ;i++){
        client* c = create_client(0,names[i], get_new_client_unique_id(lobby));
        if(c == NULL) return NULL;
        else {
            add_client_to_lobby(c,lobby);
            clients[i] = c;
        }
    }

	return clients;
}

lobby* test_lobby_adding(client** clients, lobby* lobby){
    int i = 0;
    for(i;i < number_of_clients; i++){
        add_client_to_lobby(clients[i],lobby);
    }
	print_clients(lobby);

	return lobby;
}

char** test_sounds_extration(){
    int sounds_length = 0;
	char* path = "../../sounds";
	// Ziskame vsechny zvuky v zadane slozce
	char** sounds = get_sounds_from_folder(path, &sounds_length);
	if(sounds == NULL)
	{
        return NULL;
	}
	print_all_sounds(sounds,sounds_length);
	// Promichame je
	printf("SHUFFLE\n");
	shuffle(sounds, sounds_length);

	print_all_sounds(sounds,sounds_length);
	// Vezmeme jich 32
	printf("SHRINK\n");
	shrink_array(sounds,&sounds_length,4);
	print_all_sounds(sounds,sounds_length);
	// Zduplikujeme je
	printf("DUPLICATE\n");
	sounds_length = duplicate(sounds,sounds_length);
	print_all_sounds(sounds,sounds_length);
	// Promichame je
	printf("SHUFFLE\n");
	shuffle(sounds, sounds_length);
	print_all_sounds(sounds,sounds_length);
    return sounds;
}
/*
void test_session(){
    printf("NEW GAME\n");
	game* new_game = create_game(sounds, sounds_length);
	print_all_sounds(new_game->puzzles,sounds_length);

	session_list* new_session_list = create_session_list();
	session* new_session = create_session(c,c2,new_game,0);

	add_session_to_session_list(new_session,new_session_list);

	return;
} */

void message_test(){

    char* raw_messages[] = {
                            "vasdas:7sasdads|asd|Pa",
                            "aaaID:aaa5148|2|avasdasfdasd",
                            "asdaaaaaaa",
                            "894513",
                            "ID:aaaaaaa|ACTION",
                            "ID:aaaaa",
                            "4554|ACTION:po",
                            "4555|1|",
                            "4555|1",
                            "4555|1|4555|1",
                            "|||||||||"

                        };

    int i = 0;
    for(i; i < 11; i++){
        message* m = extract_message(raw_messages[i]);
        if(m == NULL) printf("Nevalidni zprava \n");
        else printf("Validni zprava \n");
    }
}
void test(){
   /* lobby* lobby = create_lobby();

    client** clients = test_client_creation(lobby);
    if(clients == NULL){
        perror("Vytvareni clientu se nezdarilo! \n");
        return;
    }
    else
        printf("Clienti vytvoreni!\n");

    //lobby = test_lobby_adding(clients, lobby);
    if(lobby == NULL){
        perror("Vytvareni lobby se nezdarilo! \n");
        return;
    }
    else
        printf("Lobby vytvoreno!\n");

    char** sounds = test_sounds_extration(); */

    message_test();

}
