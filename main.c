#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
// kvuli iotctl
#include <sys/ioctl.h>
#include <pthread.h>
#include <memory.h>
#include <sys/select.h>

#include "session_list.h"
#include "communication_manager.h"
#include "lobby.h"
#include "test.h"
#include "client_handle_container.h"

/**
 * Hlavni funkce, ktera zprostredkovava pripojeni hracu
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[]) {

    int server_socket;
	int client_socket, fd;
	int return_value;
	char cbuf;
	int len_addr;
	int a2read;
	struct sockaddr_in my_addr, peer_addr;
	fd_set client_socks, tests;

	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	memset(&my_addr, 0, sizeof(struct sockaddr_in));

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(10000);
	my_addr.sin_addr.s_addr = INADDR_ANY;

	return_value = bind(server_socket, (struct sockaddr *) &my_addr, \
		sizeof(struct sockaddr_in));

	if (return_value == 0)
		printf("BIND SUCCESSFUL\n");
	else {
		printf("BIND ERROR - PORT IS PROPABLY OCCUPIED\n");
		return -1;
	}

	return_value = listen(server_socket, 5);
	if (return_value == 0){
		printf("LISTEN SUCCESSFUL\n");
	} else {
		printf("LISTEN ERROR\n");
	}

    lobby* actual_lobby = create_lobby();
    session_list* actual_session_list = create_session_list();

	// vyprazdnime sadu deskriptoru a vlozime server socket
	FD_ZERO( &client_socks );
	FD_SET( server_socket, &client_socks );

	pthread_t thread_id;;

	for (;;){

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
				if (fd == server_socket){
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
                        //client_handle_container* h_container = create_client_handle_container(actual_lobby,actual_session_list,fd);
                        pthread_create(&thread_id, NULL,(void *)&handle_client, (void *)NULL);
						/*recv(fd, &cbuf, 1, 0);
						printf("Prijato %c\n",cbuf);
						read(fd, &cbuf, 1);
						printf("Prijato %c\n",cbuf); */
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

	}



    test();
	/*printf("Reveal\n");
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
	} */

	return 0;
}
