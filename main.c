/*#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h> */


#include <stdio.h>
#include "file_processor.h"
#include "utils.h"
#include "game.h"
#include "client.h"
#include "lobby.h"
#include "session.h"
#include "session_list.h"

/**
 * Hlavni funkce, ktera zprostredkovava pripojeni hracu
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[]) {

	client* c = create_client(0,"Pepa");
	client* c2 = create_client(0,"Anca");
	client* c3 = create_client(0,"Karel");
	client* c4 = create_client(0,"Michal");
	client* c5 = create_client(0,"Ota");
	client* c6 = create_client(0,"Pavel");

	lobby* lobby = create_lobby();

	add_client_to_lobby(c,lobby);
	add_client_to_lobby(c2,lobby);
	add_client_to_lobby(c3,lobby);
	add_client_to_lobby(c4,lobby);
	add_client_to_lobby(c5,lobby);

	print_clients(lobby);


	int sounds_length = 0;
	// Ziskame vsechny zvuky v zadane slozce
	char** sounds = get_sounds_for_puzzle("D:/Projekty/sounds", &sounds_length);
	if (sounds == NULL)
	{
		return 0;
	}
	print_all_sounds(sounds,sounds_length);
	// Promichame je
	printf("SHUFFLE\n");
	shuffle(sounds, sounds_length);

	print_all_sounds(sounds,sounds_length);
	// Vezmeme jich 32
	printf("SHRINK\n");
	shrink_array(sounds,&sounds_length,4);
	print_all_sounds(sounds,sounds_length);
	// Zduplikujeme je
	printf("DUPLICATE\n");
	sounds_length = duplicate(sounds,sounds_length);
	print_all_sounds(sounds,sounds_length);
	// Promichame je
	printf("SHUFFLE\n");
	shuffle(sounds, sounds_length);
	print_all_sounds(sounds,sounds_length);
	printf("NEW GAME\n");
	game* new_game = create_game(sounds, sounds_length);
	print_all_sounds(new_game->puzzles,sounds_length);

	session_list* new_session_list = create_session_list();
	session* new_session = create_session(c,c2,new_game,0);

	add_session_to_session_list(new_session,new_session_list);

	return;
	printf("Reveal\n");
	char buff[1024];
	while(1){
		printf("Hrac: %d>",new_game->actual_player);
		fgets(buff, sizeof(buff),stdin);
		int puzzle = atoi(buff);
		int valid = isValid(puzzle,new_game);
		if(valid == 0){
			reveal(puzzle,new_game);

			// Pokud nejsou nastaveny vsechny potrebne atributy - tzn ze hrac jeste nehral po druhe, pokracujeme
			if(scored(new_game) < 0)
			{
				continue;
			}

			int next = nextTurn(new_game);
			print_all_sounds(new_game->puzzles,sounds_length);
			if(next == GAME_OVER){
				printf("Konec hry\n");
				break;
			}
		}
		else{
			printf("Nevalidni :%d\n",valid);
		}
	}

	/*int server_sock;
	int client_sock;
	int return_value;
	char cbuf;
	int *th_socket;
	struct sockaddr_in local_addr;
	struct sockaddr_in remote_addr;
	socklen_t	remote_addr_len;
	pthread_t thread_id;;
	pthread_t thread_id2;;

	server_sock = socket(AF_INET, SOCK_STREAM, 0);

	if (server_sock<=0) return -1;

	memset(&local_addr, 0, sizeof(struct sockaddr_in));

	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(10000);
	local_addr.sin_addr.s_addr = INADDR_ANY;

	return_value = bind(server_sock, (struct sockaddr *)&local_addr,\
                sizeof(struct sockaddr_in));

	if (return_value == 0)
		printf("Bind OK\n");
	else{
		printf("Bind ER\n");
		return -1;
	}

	return_value = listen(server_sock, 5);
	if (return_value == 0)
		printf("Listen OK\n");
	else{
		printf("Listen ER\n");
	}


	while(1){
		client_sock = accept(server_sock,\
			(struct sockaddr *)&remote_addr,\
			&remote_addr_len);
		if (client_sock > 0 ) {
			th_socket=malloc(sizeof(int));
			*th_socket=client_sock;
			pthread_create(&thread_id, NULL,\
                                  (void *)&write_client, (void *)th_socket);
			pthread_create(&thread_id2, NULL,\
                              	(void *)&read_client, (void *)th_socket);
		} else {
			printf("Trable\n");
			return -1;
		}
	} */
	/*int server_socket;
	int port = 10000;
	int server_is_runnning = 1;
	struct sockaddr_in local_sock_addr, peer_addr;


	int client_socket, fd;
	char cbuf;
	int len_addr;
	int a2read;
	fd_set client_socks, tests;

	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	memset(&local_sock_addr, 0, sizeof(struct sockaddr_in));
	local_sock_addr.sin_family = AF_INET;
	local_sock_addr.sin_port = htons(port);
	local_sock_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(server_socket, (struct sockaddr *) &local_sock_addr, sizeof(struct sockaddr_in)) < 0)
	{
		perror("BIND ERROR - Bind is already occupied!\n");
		return -1;
	}

	return_value = listen(server_socket, 5);
	if (listen(server_socket, 5) < 0){
		perror("LISTEN ERROR!\n");
		return -1;
	}

	fd_set file_descriptor_set;
	while(server_is_runnning){

	}


	// vyprazdnime sadu deskriptoru a vlozime server socket
	FD_ZERO( &client_socks );
	FD_SET( server_socket, &client_socks );

/*	for (;;){

		tests = client_socks;
		// sada deskriptoru je po kazdem volani select prepsana sadou deskriptoru kde se neco delo
		return_value = select( FD_SETSIZE, &tests, ( fd_set *)0, ( fd_set *)0, ( struct timeval *)0 );

		if (return_value < 0) {
			printf("Select - ERR\n");
			return -1;
		}
		// vynechavame stdin, stdout, stderr
		for( fd = 3; fd < FD_SETSIZE; fd++ ){
			// je dany socket v sade fd ze kterych lze cist ?
			if( FD_ISSET( fd, &tests ) ){
			// je to server socket ? prijmeme nove spojeni
				if (fd = server_socket){
					client_socket = accept(server_socket, (struct sockaddr *) &peer_addr, &len_addr);
					FD_SET( client_socket, &client_socks );
					printf("Pripojen novy klient a pridan do sady socketu\n");
				}
				// je to klientsky socket ? prijmem data
				else {
					// pocet bajtu co je pripraveno ke cteni
					ioctl( fd, FIONREAD, &a2read );
					// mame co cist
					if (a2read > 0){
						recv(fd, &cbuf, 1, 0);
						printf("Prijato %c\n",cbuf);
						read(fd, &cbuf, 1);
						printf("Prijato %c\n",cbuf);
					}
					// na socketu se stalo neco spatneho
					else {
						close(fd);
						FD_CLR( fd, &client_socks );
						printf("Klient se odpojil a byl odebran ze sady socketu\n");
					}
				}
			}
		}

	}	*/

	return 0;
}
