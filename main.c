#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <pthread.h>
// kvuli iotctl
#include <sys/ioctl.h>
#include <memory.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include "session.h"
#include "session_list.h"
#include "communication_manager.h"
#include "lobby.h"
#include "test.h"
#include "client_handle_container.h"
#include "disconnected_clients.h"
#include "log.h"

#include "server_stats.h"

#define MAX_PORT 65535

int server_socket;
struct sockaddr_in my_addr;
/*
* Function: server_stats_handle
* ------------------------
* Funkce pro vlakno pro obsluhu requestu admina
*
* vargp: server_stats;
*
* returns: void*
*/
void* server_stats_handle(void* vargp){
    log_info("SERVER STATS HANDLE - Started");
    server_stats* actual_server_stats = (server_stats*) vargp;
    if(actual_server_stats == NULL){
        log_error("SERVER STATS - Not valid params");
        return NULL;
    }
    char input[256];
    while(actual_server_stats->run){
        fgets(input, 256, stdin);
        char* pos;
        if((pos = strchr(input, '\n')) != NULL){
            *pos = '\0';
            handle_admin_request(input, actual_server_stats);
        }
        else{
            log_error("SERVER STATS HANDLE - Too long input");
        }
    }
    return NULL;
}
/*
* Function: server_setup
* ------------------------
* Priprava serveru, vraci 1 jestli se povedlo
*
* sounds_folder_path: cesta ke slozce se zvuky
*
* returns: ANO/NE
*/
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
/*
* Function: server_start
* ------------------------
* Start serveru, vraci 1 jestli se povedlo
*
* port: port serveru
* adress: adressa serveru
*
* returns: ANOE/NE
*/
int server_start(int port, in_addr_t adress){
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));

    memset(&my_addr, 0, sizeof(struct sockaddr_in));

    my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port);
	my_addr.sin_addr.s_addr = adress;

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
/*
* Function: stop_server
* ------------------------
* Stop serveru
*
* actual_server_stats: container s listy
*
* returns: void
*/
void stop_server(server_stats* actual_server_stats){
    log_trace("STOPING SERVER");
    free_lobby(actual_server_stats->lobby);
    free_session_list(actual_server_stats->session_list);
    free_disconnected_clients_list(actual_server_stats->disconnected_clients_list);
}
/*
* Function: server_listen
* ------------------------
* Spusteni listen, vraci 1 jestli se povede
*
* sounds_folder_path: cesta ke slozce se zvuky
*
* returns: ANO/NE
*/
int server_listen(char* sounds_folder_path, int pexeso_count){
    int return_value = listen(server_socket, 5);
    if (return_value == 0){
        log_info("LISTEN SUCCESSFUL");
	} else {
		log_error("LISTEN FAILED");
	}
    lobby* actual_lobby = create_lobby();
    session_list* actual_session_list = create_session_list();
    disconnected_clients_list* actual_disconnected_clients = create_disconnected_clients_list();

    server_stats* actual_server_stats = create_server_stats(actual_lobby, actual_session_list, actual_disconnected_clients);

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, server_stats_handle, actual_server_stats);

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

	while(actual_server_stats->run){
        client_set = client_socks;
		// sada deskriptoru je po kazdem volani select prepsana sadou deskriptoru kde se neco delo
		return_value = select(FD_SETSIZE, &client_set, (fd_set *)0,(fd_set *)0, /*( struct timeval *)1 */ &timeout);
		if(return_value == 0){
            log_info("SELECT TIMEOUT");
            timeout.tv_sec = 60;
            handle_disconnected_clients_list(actual_session_list, actual_disconnected_clients, pexeso_count);
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
                        client_handle_container* h_container = create_client_handle_container(actual_lobby, actual_session_list, fd, message, actual_disconnected_clients, sounds_folder_path, pexeso_count);
                        int handle_value = handle_client(h_container);

                        if(handle_value == NOT_VALID_MESSAGE_SENDED){
                            log_info("HANDLE CLIENT - Closing socket, not valid message");
                            close(fd);
                            FD_CLR( fd, &client_socks);
                        }
					}
					// na socketu se stalo neco spatneho
					else {
						close(fd);
						FD_CLR( fd, &client_socks);

						handle_client_disconnect(fd, actual_lobby, actual_disconnected_clients, actual_session_list, pexeso_count);
						log_info("CLIENT HAS DISCONNECTED AND WAS REMOVED FROM SOCKET'S SET");
					}
				}
			}
		}
		handle_disconnected_clients_list(actual_session_list, actual_disconnected_clients, pexeso_count);

	}
	stop_server(actual_server_stats);

	return 0;
}
void help(){
    log_info("HELP");
    log_info("To start server use this:");
    log_info("  ./main -port <port> -address <adress> -folder <path to folder> -count <pexeso count>");
    log_info("  If no params, defaul will be used:");
    log_info("  port - 10000");
    log_info("  adress - INADDR_ANY");
    log_info("  folder - sounds");
    log_info("  count - 32");
}
/*
* Function: start
* ------------------------
* Spusteni podle parametru, 1 pokud se povede
*
* argc: pocet argumentu
* argv: argumenty
*
* returns: ANO/NE
*/
int start(int argc, char* argv[]){
    int i;
    int port = 10000;
    int adress_result = 0;
    int adress_index = 0;
    in_addr_t adress = INADDR_ANY;

    char* sounds_folder_path = "sounds";
    int pexeso_count = 32;

    for(i = 1; i < argc; i++){
        if(strcmp(argv[i], "-port") == 0 && (i + 1) < argc){
            int value = (int)convert_string_to_long(argv[i + 1]);
            if(value > MAX_PORT && value < 0){
                log_error("NOT VALID PORT - Port is out of limit, default will be used");
            }
            else{
                i = i + 1; //Musime pridat aby jsme preskocili zpracovany
                port = value;
            }
        }
        else if(strcmp(argv[i], "-address") == 0 && (i + 1) < argc){
            adress_result = inet_pton(AF_INET, argv[i + 1], &adress);
            adress_index = i + 1;

            i = i + 1; //Musime pridat aby jsme preskocili zpracovany
        }
        else if(strcmp(argv[i], "-folder" ) == 0 && (i + 1) < argc){
            sounds_folder_path = (char* )malloc(sizeof(char) * (strlen( argv[i + 1]) + 1));
            strcpy(sounds_folder_path, argv[i + 1]);

            i = i + 1; //Musime pridat aby jsme preskocili zpracovany
        }
        else if(strcmp(argv[i], "-count" ) == 0 && (i + 1) < argc){
            int value = (int)convert_string_to_long(argv[i + 1]);
            if(value < 0){
                log_error("NOT VALID PEXESO COUNT - Default pexeso count will be used");
            }
            else{
                i = i + 1; //Musime pridat aby jsme preskocili zpracovany
                pexeso_count = value;
            }

            i = i + 1; //Musime pridat aby jsme preskocili zpracovany
        }
        else if(strcmp(argv[i], "-help") == 0){
            help();
            return -1;
        }
        else{
            log_error("NOT VALID PARAMS");
            help();
            return -1;
        }

    }
    if(adress_result == 1){
        log_info("SERVER IS STARTING ON ADRESS: %s/%d", argv[adress_index], port);
    }
    else{
        log_info("SERVER IS STARTING ON ADRESS: %s/%d", "INADDR_ANY", port);
    }
    log_info("SERVER IS STARTING WITH PARAM- Folder of sounds: %s", sounds_folder_path);
    log_info("SERVER IS STARTING WITH PARAM- Pexeso count: %d", pexeso_count);
    if(server_setup(sounds_folder_path) == -1)
    {
        help();
        return -1;
    }
    if(server_start(port, adress))
        return -1;
    if(server_listen(sounds_folder_path, pexeso_count))
        return -1;

    return 0;
}
/**
 * Hlavni funkce, ktera zprostredkovava spusteni
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[]) {
    start(argc, argv);

    return 0;
}
