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
	new_game->puzzles = sounds;
	new_game->puzzle_count = sound_count;
	new_game->remaining_puzzle_count = sound_count;
	new_game->p1_score = 0;
	new_game->p2_score = 0;
	new_game->first_reveal = -1;
	new_game->second_reveal = -1;
	new_game->actual_player = 0;

	return new_game;
}
/* Function: reveal
* ------------------------
* Nastavi hodnoty zahranych puzzli na indexy odhalenych
*
* puzzle_revealed: odhalena puzzle
* actual_game: aktualni hra
*
* returns: MESSAGE
*/
int reveal(int puzzle_revealed, game* actual_game){
    if(actual_game == NULL){
        printf("REVEAL ERROR - Reveal failed, no game passed! \n");
        return -1;
    }
	// Zahral prvni tah
	if(actual_game->first_reveal < 0)
	{
		printf("Prvni tah!\n");
		printf("	index: %d\n",puzzle_revealed);
		printf("		 : %s\n",actual_game->puzzles[puzzle_revealed]);
		actual_game->first_reveal = puzzle_revealed;
	}
	// Zahral druhy tah
	else{
		printf("Druhy tah!\n");
		printf("	index: %d\n",puzzle_revealed);
		printf("		 : %s\n",actual_game->puzzles[puzzle_revealed]);
		actual_game->second_reveal = puzzle_revealed;
	}
	return PLAYER_REVEALED_PUZZLE;
}

/* Function: isValid
* ------------------------
* Zkontroluje jestli odhalena puzzle je validni
*
* puzzle_revealed: odhalena puzzle
* actual_game: aktualni hra
*
* returns: MESSAGE
*/
int isValid(int puzzle_revealed, game* actual_game){
    if(actual_game == NULL){
        printf("ISVALID ERROR - Is valid failed, no game passed! \n");
        return -1;
    }
	//Spatny index v poli zvuku
	if(puzzle_revealed < 0 || puzzle_revealed >= actual_game->puzzle_count){
		printf("Byl zadan spatny index: %d\n",puzzle_revealed);
		return REVEALED_PUZZLE_REVEAL;
	}
	//Odhaleni jiz odhalene puzzle - Jiz obodovane
	if(strcmp(actual_game->puzzles[puzzle_revealed],"") == 0){
		printf("Chyba byl zadan index, kde uz je odhalena puzzle\n");
		return INVALID_PUZZLE_REVEAL;
	}
	// Pokud odhalil stejnou puzzli jako tu prvni
	if(actual_game->first_reveal == puzzle_revealed)
	{
		printf("	-Hahaha dobry pokus, stejne indexy nelze zadat!\n 	-Zkus to znovu!\n");
		return REVEALED_PUZZLE_REVEAL;
	}

	return 0;
}
/* Function: scored
* ------------------------
* Zkontroluje jestli hrac scoroval
* Pokud jo, pricte skore a smaze puzzle
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
	int first_reveal = actual_game->first_reveal;
	int second_reveal = actual_game->second_reveal;
	//Nejsou zadany tahy hrace
	if(second_reveal < 0 || first_reveal < 0){
		return -1;
	}

	char* first_puzzle_revealed = actual_game->puzzles[first_reveal];
	char* second_puzzle_revealed = actual_game->puzzles[second_reveal];
    // Jsou stejne odhalene puzzle
	if(strcmp(first_puzzle_revealed, second_puzzle_revealed) == 0)
	{
		printf("	-Za skoroval jsi!\n");

			printf("		1.index: %d\n",actual_game->first_reveal);
			printf("			: %s\n",actual_game->puzzles[actual_game->first_reveal]);

			printf("		2.index: %d\n",second_reveal);
			printf("			: %s\n",second_puzzle_revealed);

		if(actual_game->actual_player == 0){
			actual_game->p1_score += 1;
		}
		else{
			actual_game->p2_score += 1;
		}
		//Odecteme pocet puzzli
		actual_game->remaining_puzzle_count -= 2;
        // Smazeni puzzli ktere odhalil
        strcpy(actual_game->puzzles[actual_game->first_reveal],"");
        strcpy(actual_game->puzzles[actual_game->second_reveal],"");

		return PLAYER_SCORED;
	}
	else{
		printf("	-Spatne puzzle!\n");
			printf("		1.index: %d\n",actual_game->first_reveal);
			printf("			: %s\n",actual_game->puzzles[actual_game->first_reveal]);

			printf("		2.index: %d\n",second_reveal);
			printf("			: %s\n",second_puzzle_revealed);

		return PLAYER_NOT_SCORED;
	}
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
	printf("Zbyva: %d\n",actual_game->remaining_puzzle_count);

	if(actual_game->remaining_puzzle_count <= 0){
		return GAME_OVER;
	}
	return NEXT_TURN;
}
