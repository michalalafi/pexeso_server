#include "game.h"

/* Function: create_game
* ------------------------
* Vytvori novou hru a priradi ji parametry
* nastavi hre defaultni hodnoty
*
* sounds: zvuky ziskane pro hru
* sound_count: pocet zvuku
*
* returns: pole zvuku
*/
game* create_game(char** sounds, int sound_count){
	if(sounds == NULL){
        printf("CREATE GAME ERROR - Creating game failed, no sounds passed! \n");
		return NULL;
	}
	game* new_game = (game*) malloc(sizeof(game));
    if(new_game == NULL){
        printf("CREATE GAME ERROR - Creating game failed! \n");
		return NULL;
	}
	new_game->pexesos = sounds;
	new_game->pexeso_count = sound_count;

    int i;
    for(i = 0; i < PEXESO_COUNT; i++){
        new_game->revead_pexesos_indexes[i] = -1;
    }
	new_game->remaining_pexeso_count = sound_count;
	new_game->p1_score = 0;
	new_game->p2_score = 0;
	new_game->first_reveal = -1;
	new_game->second_reveal = -1;
	new_game->actual_player = 0;

	return new_game;
}
game* create_game_with_path(char* sounds_folder_path){
    if(sounds_folder_path == NULL){
        log_error("CREATE GAME WITH PATH - Not valid params");
        return NULL;
    }

    char** sounds = get_sounds_for_pexeso(sounds_folder_path, PEXESO_COUNT);
    if(sounds == NULL){
        log_error("CREATE GAME WITH PATH - Cannot load sounds");
        return NULL;
    }
    return create_game(sounds, PEXESO_COUNT);
}
/* Function: reveal
* ------------------------
* Nastavi hodnoty zahranych pexeses na indexy odhalenych
*
* pexeso_revealed: odhalene pexeso
* actual_game: aktualni hra
*
* returns: MESSAGE
*/
char* reveal(int pexeso_revealed, game* actual_game){
    if(actual_game == NULL){
        printf("REVEAL ERROR - Reveal failed, no game passed! \n");
        return NULL;
    }
	// Zahral prvni tah
	if(actual_game->first_reveal < 0)
	{
		printf("Prvni tah!\n");
		printf("	index: %d\n",pexeso_revealed);
		printf("		 : %s\n",actual_game->pexesos[pexeso_revealed]);
		actual_game->first_reveal = pexeso_revealed;
	}
	// Zahral druhy tah
	else{
		printf("Druhy tah!\n");
		printf("	index: %d\n",pexeso_revealed);
		printf("		 : %s\n",actual_game->pexesos[pexeso_revealed]);
		actual_game->second_reveal = pexeso_revealed;
	}
    return actual_game->pexesos[pexeso_revealed];
}

/* Function: isValid
* ------------------------
* Zkontroluje jestli odhalene pexeso je validni
*
* pexeso_revealed: odhalene pexeso
* actual_game: aktualni hra
*
* returns: MESSAGE
*/
int isValid(int pexeso_revealed, game* actual_game){
    if(actual_game == NULL){
        log_error("IS VALID - Not valid params");
        return -1;
    }
	//Spatny index v poli zvuku
	if(pexeso_revealed < 0 || pexeso_revealed >= actual_game->pexeso_count){
		printf("Byl zadan spatny index: %d\n",pexeso_revealed);
		return -1;
	}
	//Odhaleni jiz odhalene pexeso - Jiz obodovane
	if(strcmp(actual_game->pexesos[pexeso_revealed],"") == 0){
		printf("Chyba byl zadan index, kde uz je odhalene pexeso\n");
		return -1;
	}
	// Pokud odhalil stejne pexeso jako tu prvni
	if(actual_game->first_reveal == pexeso_revealed)
	{
		printf("	-Hahaha dobry pokus, stejne indexy nelze zadat!\n 	-Zkus to znovu!\n");
		return -1;
	}

	return 1;
}
/* Function: scored
* ------------------------
* Zkontroluje jestli hrac scoroval
* Pokud jo, pricte skore a smaze pexeso
*
* actual_game: aktualni hra
*
* returns: MESSAGE
*/
int scored(game* actual_game){
    log_trace("SCORED");
    if(actual_game == NULL){
        log_error("SCORED - Not valid params");
        return -1;
    }
	if(!is_end_of_turn(actual_game)){
        log_error("SCORES - Is not end of players turn");
		return -1;
	}
    int first_reveal = actual_game->first_reveal;
	int second_reveal = actual_game->second_reveal;
	char* first_pexeso_revealed = actual_game->pexesos[first_reveal];
	char* second_pexeso_revealed = actual_game->pexesos[second_reveal];
    // Jsou stejne odhalene puzzle
	if(strcmp(first_pexeso_revealed, second_pexeso_revealed) == 0)
	{
		if(actual_game->actual_player == 0){
			actual_game->p1_score += 1;
		}
		else{
			actual_game->p2_score += 1;
		}
		//Odecteme pocet puzzli
		actual_game->remaining_pexeso_count -= 2;
        // Smazeni puzzli ktere odhalil
        strcpy(actual_game->pexesos[first_reveal],"");
        strcpy(actual_game->pexesos[second_reveal],"");

        actual_game->revead_pexesos_indexes[first_reveal] = 1;
        actual_game->revead_pexesos_indexes[second_reveal] = 1;

		return 1;
	}
	else{
		return 0;
	}
}
int is_end_of_turn(game* actual_game){
    log_trace("IS END OF TURN");
    int first_reveal = actual_game->first_reveal;
	int second_reveal = actual_game->second_reveal;
	//Nejsou zadany tahy hrace
	if(second_reveal < 0 || first_reveal < 0){
		return 0;
	}
	return 1;
}
/* Function: nextTurn
* ------------------------
* Prepne hrace, vyresetuje odhaleni a zkontroluje jestli neni konec hry
*
* actual_game: aktualni hra
*
* returns: MESSAGE
*/
int nextTurn(game* actual_game){
    log_trace("NEXT TURN");
    if(actual_game == NULL){
        log_error("NEXT TURN - Next turn failed, no game passed!");
        return -1;
    }
	// Zmena hrace
	actual_game->actual_player ^= 1;
	// Resetovani hodnot
	actual_game->first_reveal = -1;
	actual_game->second_reveal = -1;

	printf("SCORE P1:%d - P2:%d\n",actual_game->p1_score,actual_game->p2_score);
	printf("Zbyva: %d\n",actual_game->remaining_pexeso_count);

	return 1;
}

int is_game_over(game* actual_game){
    log_trace("IS GAME OVER");
    if(actual_game == NULL){
        log_error("IS GAME OVER - Not valid params");
        return -1;
    }

    if(actual_game->remaining_pexeso_count <= 0){
        return 1;
    }
    return 0;
}
void print_not_revealed_sounds(game* actual_game){
    log_info("NOT REVEALED SOUNDS");
    if(actual_game == NULL){
        log_error("PRINT NOT REVEALED SOUNDS - Not valid params");
        return;
    }
    if(actual_game->pexesos == NULL){
        log_error("PRINT NOT REVEALED SOUNDS - Game has not array of sounds");
        return;
    }

    int i;
    for(i = 0; i < PEXESO_COUNT; i++){
        log_info("  NotReveald[%d]:%s", i, actual_game->pexesos[i]);
    }
}
void print_revealed_sounds_indexes(game* actual_game){
    log_info("REVEALED SOUNDS");
    if(actual_game == NULL){
        log_error("PRINT REVEALED SOUNDS - Not valid params");
        return;
    }

    int i;
    for(i = 0; i < PEXESO_COUNT; i++){
        log_info("  Revealed[%d]:%d",i,actual_game->revead_pexesos_indexes[i]);
    }
}
void free_game(game* actual_game){
    log_info("FREE GAME");
    if(actual_game == NULL){
        log_error("FREE GAME - Not valid params");
        return;
    }

    actual_game->actual_player = 0;
    actual_game->first_reveal = 0;
    actual_game->second_reveal = 0;
    actual_game->p1_score = 0;
    actual_game->p2_score = 0;
    actual_game->remaining_pexeso_count = 0;

    int i;
    for(i = 0; i < PEXESO_COUNT; i++){
        free(actual_game->pexesos[i]);
    }

    free(actual_game->pexesos);
    free(actual_game);
    log_info("FREE GAME DONE");
}
