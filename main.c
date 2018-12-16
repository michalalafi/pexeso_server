#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
// kvuli iotctl
#include <sys/ioctl.h>
#include <memory.h>
#include <sys/select.h>
#include "session.h"
#include "session_list.h"
#include "communication_manager.h"
#include "lobby.h"
#include "test.h"
#include "client_handle_container.h"
#include "disconnected_clients.h"
#include "log.h"

int server_socket;
struct sockaddr_in my_addr;

int server_setup(char* sounds_folder_path){

    srand(time(NULL));
    FILE* log_file;

    log_file = fopen("Server_Log.log","a+");
    if(log_file == NULL){
        log_error("CANNOT OPEN LOG FILE!");
        return -1;
    }
    else{
        log_set_fp(log_file);
    }

    if(is_folder_with_sounds_avaible(sounds_folder_path)){
        log_info("SOUNDS FOLDER IS AVAIBLE");
    }
    else{
        log_error("SOUNDS FOLDER ISN'T AVAIBLE");
        return -1;
    }


    log_info("SERVER SETUPED!");

    return 0;
}
int server_start(int port){
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    //SETSOCKOPT?

    memset(&my_addr, 0, sizeof(struct sockaddr_in));

    my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port);
	my_addr.sin_addr.s_addr = INADDR_ANY;

	int return_value = bind(server_socket, (struct sockaddr *) &my_addr, \
		sizeof(struct sockaddr_in));

    if (return_value == 0)
        log_info("BIND SUCCESSFUL");
	else {
        log_error("BIND - PORT IS PROPABLY OCCUPIED");
		return -1;
	}
	return 0;
}
int server_listen(char* sounds_folder_path){
    //Co znamena 5?
    int return_value = listen(server_socket, 5);
    if (return_value == 0){
        log_info("LISTEN SUCCESSFUL");
	} else {
		log_error("LISTEN FAILED");
	}
    lobby* actual_lobby = create_lobby();
    session_list* actual_session_list = create_session_list();
    disconnected_clients_list* actual_disconnected_clients = create_disconnected_clients_list();

	fd_set client_socks, client_set;
	struct sockaddr_in peer_addr;
	socklen_t len_addr;
    int a2read;
	int client_socket, fd;

	// Vyprazdnime sadu deskriptoru a vlozime server socket
	FD_ZERO( &client_socks );
	FD_SET( server_socket, &client_socks );

	struct timeval timeout;

    timeout.tv_sec = 60;
    timeout.tv_usec = 0;

	for (;;){
        client_set = client_socks;
		// sada deskriptoru je po kazdem volani select prepsana sadou deskriptoru kde se neco delo
		return_value = select( FD_SETSIZE +1, &client_set, ( fd_set *)0, ( fd_set *)0, /*( struct timeval *)1 */ &timeout);
		if(return_value == 0){
            log_info("SELECT TIMEOUT");
            timeout.tv_sec = 60;
            handle_disconnected_clients_list(actual_session_list, actual_disconnected_clients);
            continue;
		}

		if (return_value < 0) {
            log_error("SELECT - ERROR VALUE: %d", return_value);
			return -1;
		}
		// vynechavame stdin, stdout, stderr
		for( fd = 3; fd < FD_SETSIZE; fd++ ){
			// je dany socket v sade fd ze kterych lze cist ?
			if( FD_ISSET( fd, &client_set ) ){
			// je to server socket ? prijmeme nove spojeni
				if (fd == server_socket){
					client_socket = accept(server_socket, (struct sockaddr *) &peer_addr, &len_addr);
					FD_SET( client_socket, &client_socks );
					log_info("NEW CLIENT CONNECTED AND ADD TO SOCKET'S SET");
				}
				// je to klientsky socket ? prijmem data
				else {
					// pocet bajtu co je pripraveno ke cteni
					ioctl( fd, FIONREAD, &a2read );
					// mame co cist
					if (a2read > 0){
                        char message[1024];
                        recv(fd, &message, 1024, 0);
                        client_handle_container* h_container = create_client_handle_container(actual_lobby, actual_session_list, fd, message, actual_disconnected_clients, sounds_folder_path);
                        //TODO po pridani zpatky do lobby zmenit socket
                        //TODO free containter
                        handle_client(h_container);
					}
					// na socketu se stalo neco spatneho
					else {
						close(fd);
						FD_CLR( fd, &client_socks );

						handle_client_disconnect(fd, actual_lobby, actual_disconnected_clients);
						log_info("CLIENT HAS DISCONNECTED AND WAS REMOVED FROM SOCKET'S SET");
					}
				}
			}
		}
		//TODO zkontrolujeme kdy se odpojil a pokud uz mu ubehla doba vymazeme ho uplne
		handle_disconnected_clients_list(actual_session_list, actual_disconnected_clients);

	}
}

/**
 * Hlavni funkce, ktera zprostredkovava pripojeni hracu
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[]) {
    char* sounds_folder_path = "../../../sounds";
    if(server_setup(sounds_folder_path) == -1)
        return -1;
    if(server_start(10000))
        return -1;
    if(server_listen(sounds_folder_path))
        return -1;

    return 0;
}
