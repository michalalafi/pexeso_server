

/*#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
// kvuli iotctl
#include <sys/ioctl.h> */


#include <stdio.h>
#include "file_processor.h"
#include "utils.h"


/**
 * Hlavni funkce, ktera zprostredkovava pripojeni hracu
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[]) {
	int sounds_length = 0;
	char** sounds = get_sounds_for_puzzle("./sounds", &sounds_length);
	print_all_sounds(sounds,sounds_length);
	printf("SHUFFLE\n");
	shuffle(sounds, sounds_length);
	print_all_sounds(sounds,sounds_length);
	printf("SHRINK\n");
	shrink_array(sounds,&sounds_length,32);
	print_all_sounds(sounds,sounds_length);
	printf("DUPLICATE\n");
	sounds_length = duplicate(sounds,sounds_length);
	print_all_sounds(sounds,sounds_length);
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
