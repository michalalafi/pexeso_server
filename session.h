#ifndef SESSION_H
#define SESSION_H

#include <stdio.h>
#include <stdlib.h>
#include "client.h"
#include "game.h"

typedef struct SESSION{
	int id;
	struct CLIENT* first_client;
	struct CLIENT* second_client;
	struct GAME* game;
	struct SESSION* next;
	struct SESSION* previous;
}session;

session* create_session(client* first_client, client* second_client, game* actual_game, int id);

int is_session_open(session* actual_session);

int is_client_in_session(client* actual_client, session* actual_session);

int get_client_player_order(client* actual_client, session* actual_session);

int is_client_on_turn(client* actual_client, session* actual_session);

#endif
