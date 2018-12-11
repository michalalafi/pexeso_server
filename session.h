#ifndef SESSION_H
#define SESSION_H

#include <stdio.h>
#include <stdlib.h>
#include "client.h"
#include "game.h"

#include "log.h"

typedef struct SESSION{
	int id;
	struct CLIENT* first_client;
	struct CLIENT* second_client;
	struct GAME* game;
	int want_first_client_play;
	int want_second_client_play;
	struct SESSION* next;
	struct SESSION* previous;
}session;


session* create_session(client* first_client, client* second_client, game* actual_game, int id);

int is_session_open(session* actual_session);

int is_session_empty(session* actual_session);

client* get_some_client_from_session(session* actual_session);

int is_client_in_session(client* actual_client, session* actual_session);

void remove_client_from_session(client* actual_client, session* actual_session);

int is_session_ready_to_play_game(session* actual_session);

void set_client_wants_play(client* actual_client, session* actual_session);

void add_client_in_session(client* actual_client, session* actual_session);

int is_session_valid(session* actual_session);

int get_client_player_order(client* actual_client, session* actual_session);

int is_client_on_turn(client* actual_client, session* actual_session);

void free_session(session* actual_session);
#endif
