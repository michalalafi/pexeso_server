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
// Hrac odhalil puzzli
#define PLAYER_REVEALED_PUZZLE 42
// Index puzzle je spatny 
#define INVALID_PUZZLE_REVEAL 91 
// Odhaleni puzzle, ktera uz je odhalena
#define REVEALED_PUZZLE_REVEAL 92 
// Konec hry
#define GAME_OVER 100 

typedef struct GAME{
	char** puzzles; // puzzle
	int puzzle_count; // pocet puzzli
	int remaining_puzzle_count; // kolik zbyva puzzli
	//hraè 1 
	//hráè 2
	int p1_score; // p1 score
	int p2_score; // p2 score
	int actual_player;//aktualni hrac;
	int first_reveal; // aktualni prvni odhaleni puzzle
	int second_reveal; //aktualni druhe odhaleni puzzle
}game;

game* create_game(char** sounds, int sound_count);

int reveal(int puzzle_revealed, game* actual_game);

int isValid(int puzzle_revealed, game* actual_game);

int scored(game* actual_game);

int nextTurn(game* actual_game);

#endif
