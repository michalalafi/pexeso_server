#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"

#include "file_processor.h"

#define PEXESO_COUNT 8

typedef struct GAME{
	char** pexesos; // pexesa
	int revead_pexesos_indexes[PEXESO_COUNT]; // Jake byli jiz odhalene
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

game* create_game_with_path(char* sounds_folder_path);

char* reveal(int pexeso_revealed, game* actual_game);

int isValid(int puzzle_revealed, game* actual_game);

int scored(game* actual_game);

int nextTurn(game* actual_game);

int is_end_of_turn(game* actual_game);

int is_game_over(game* actual_game);

void print_not_revealed_sounds(game* actual_game);

void print_revealed_sounds_indexes(game* actual_game);

void free_game(game* actual_game);

#endif
