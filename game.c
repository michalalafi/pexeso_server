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
	new_game->remaining_pexeso_count = sound_count;
	new_game->p1_score = 0;
	new_game->p2_score = 0;
	new_game->first_reveal = -1;
	new_game->second_reveal = -1;
	new_game->actual_player = 0;

	return new_game;
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
int reveal(int pexeso_revealed, game* actual_game){
    if(actual_game == NULL){
        printf("REVEAL ERROR - Reveal failed, no game passed! \n");
        return -1;
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
	return PLAYER_REVEALED_PEXESO;
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
        printf("ISVALID ERROR - Is valid failed, no game passed! \n");
        return -1;
    }
	//Spatny index v poli zvuku
	if(pexeso_revealed < 0 || pexeso_revealed >= actual_game->pexeso_count){
		printf("Byl zadan spatny index: %d\n",pexeso_revealed);
		return REVEALED_PEXESO_REVEAL;
	}
	//Odhaleni jiz odhalene pexeso - Jiz obodovane
	if(strcmp(actual_game->pexesos[pexeso_revealed],"") == 0){
		printf("Chyba byl zadan index, kde uz je odhalene pexeso\n");
		return INVALID_PEXESO_REVEAL;
	}
	// Pokud odhalil stejne pexeso jako tu prvni
	if(actual_game->first_reveal == pexeso_revealed)
	{
		printf("	-Hahaha dobry pokus, stejne indexy nelze zadat!\n 	-Zkus to znovu!\n");
		return REVEALED_PEXESO_REVEAL;
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
    if(actual_game == NULL){
        printf("SCORED ERROR - Scored failed, no game passed! \n");
        return -1;
    }
	if(!is_end_of_turn(actual_game)){
        printf("Neni konec hracova kola, nelze urcit skorovani \n");
		return -1;
	}
    int first_reveal = actual_game->first_reveal;
	int second_reveal = actual_game->second_reveal;
	char* first_pexeso_revealed = actual_game->pexesos[actual_game->first_reveal];
	char* second_pexeso_revealed = actual_game->pexesos[actual_game->second_reveal];
    // Jsou stejne odhalene puzzle
	if(strcmp(first_pexeso_revealed, second_pexeso_revealed) == 0)
	{
		printf("	-Za skoroval jsi!\n");

			printf("		1.index: %d\n",first_reveal);
			printf("			: %s\n",first_pexeso_revealed);

			printf("		2.index: %d\n",second_reveal);
			printf("			: %s\n",second_pexeso_revealed);

		if(actual_game->actual_player == 0){
			actual_game->p1_score += 1;
		}
		else{
			actual_game->p2_score += 1;
		}
		//Odecteme pocet puzzli
		actual_game->remaining_pexeso_count -= 2;
        // Smazeni puzzli ktere odhalil
        strcpy(actual_game->pexesos[actual_game->first_reveal],"");
        strcpy(actual_game->pexesos[actual_game->second_reveal],"");

		return 1;
	}
	else{
		printf("	-Spatne puzzle!\n");
			printf("		1.index: %d\n",first_reveal);
			printf("			: %s\n",actual_game->pexesos[actual_game->first_reveal]);

			printf("		2.index: %d\n",second_reveal);
			printf("			: %s\n",second_pexeso_revealed);

		return 0;
	}
}
int is_end_of_turn(game* actual_game){
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
    if(actual_game == NULL){
        printf("NEXT TURN ERROR - Next turn failed, no game passed! \n");
        return -1;
    }
	// Zmena hrace
	actual_game->actual_player ^= 1;
	// Resetovani hodnot
	actual_game->first_reveal = -1;
	actual_game->second_reveal = -1;

	printf("SCORE %d:%d\n",actual_game->p1_score,actual_game->p2_score);
	printf("Zbyva: %d\n",actual_game->remaining_pexeso_count);

	return NEXT_TURN;
}

int is_game_over(game* actual_game){
    if(actual_game->remaining_pexeso_count <= 0){
        return 1;
    }
    return 0;
}
