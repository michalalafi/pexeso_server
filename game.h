#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NEXT_TURN 20
// Hrac skoroval
#define PLAYER_SCORED 40
// Hrac neskoroval
#define PLAYER_NOT_SCORED 41
// Hrac odhalil pexeso
#define PLAYER_REVEALED_PEXESO 42
// Index pexesa je spatny
#define INVALID_PEXESO_REVEAL 91
// Odhaleni pexesa, ktera uz je odhalena
#define REVEALED_PEXESO_REVEAL 92
// Konec hry
#define GAME_OVER 100

typedef struct GAME{
	char** pexesos; // pexesa
	int pexeso_count; // pocet pexesa
	int remaining_pexeso_count; // kolik zbyva pexesa
	//hraè 1
	//hráè 2
	int p1_score; // p1 score
	int p2_score; // p2 score
	int actual_player;//aktualni hrac;
	int first_reveal; // aktualni prvni odhaleni puzzle
	int second_reveal; //aktualni druhe odhaleni puzzle
}game;

game* create_game(char** sounds, int sound_count);

char* reveal(int pexeso_revealed, game* actual_game);

int isValid(int puzzle_revealed, game* actual_game);

int scored(game* actual_game);

int nextTurn(game* actual_game);

int is_end_of_turn(game* actual_game);

int is_game_over(game* actual_game);

#endif
