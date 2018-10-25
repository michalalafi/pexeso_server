#ifndef SESSION_H
#define SESSION_H

#include <stdio.h>
#include <stdlib.h>
#include "client.h"
#include "game.h"

typedef struct SESSION{
	struct CLIENT* first_client;
	struct CLIENT* second_client;
	struct GAME* game;
}session;

session* create_session(client* first_client, client* second_client, game* actual_game);

#endif
