#ifndef PROTOCOL_H
#define PROTOCOL_H

/**LOBBY HANDLE**/
#define LOGIN_TO_LOBBY_REQUEST 4
    #define CLIENTS_ID_RESPONSE 10
    #define CLIENTS_NAME_RESPONSE 11
/**SESSION HANDLE**/
#define NEW_SESSION_REQUEST 5
    #define SESSION_ID_RESPONSE 5
/**GAME HANDLE**/
#define NEW_GAME_REQUEST 6
    #define WAIT_FOR_PLAYER_TO_JOIN 12
    #define NEW_GAME_BEGIN_RESPONSE 13
    #define PEXESO_REVEALED_REQUEST 20
    #define PEXESO_REVEAL_RESPONSE 20

#define NOT_VALID_MESSAGE 90

#define SESSION_RESPONSE 70
#define LOGGED_IN_LOBBY 60


#define NOT_LOGGED_IN_LOBBY 91
#define ALREADY_LOGGED_IN 92
#define ALREADY_IN_SESSION 93
/*
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
*/


#endif

