#include <stdio.h>
#include "config.h"
#include "engine.h"
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/un.h>
#include <bits/poll.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/stat.h>



void server_exit(){

    if(errno != 0) {
        fprintf(stderr, "Error %s\n", strerror(errno));
    }
    if (access(SOCKET_PATH, F_OK) != -1) {
        remove(SOCKET_PATH);

    }

    exit(1);
}

int create_socket_fd() {
    umask(0);
    int sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        server_exit();
    }

    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int)) < 0) {
        server_exit();
    }

    if (fcntl(sock_fd, F_SETFL) == -1) {
        perror("fcntl");
        server_exit();
    }

    return sock_fd;
}

void bind_socket(int sock_fd) {
    struct sockaddr_un serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strncpy(serv_addr.sun_path, SOCKET_PATH, sizeof(serv_addr.sun_path) - 1);
    chmod(SOCKET_PATH, 0777);
    if (bind(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
        server_exit();
    }

    if (listen(sock_fd, MAX_CLIENTS) == -1) {
        server_exit();
    }
}

int handle_new_connection(struct Player *players, struct Game *game, struct pollfd *fds, int nfds, int client_fd) {
    if (nfds == MAX_CLIENTS + 1) {
        fprintf(stderr, "Too many clients\n");
        close(client_fd);
    } else {
        fds[nfds].fd = client_fd;
        fds[nfds].events = POLLIN;
        fds[nfds].revents = 0;
        printf("New client connected \n");
        game->count_occupied_cells++;
        struct Player player;

        init_player(game, &player, nfds);
        players[nfds - 1] = player;

        nfds++;
        printf("NFDS %d\n", nfds);
        send_updated_game_to_clients(game, fds, nfds, players);
    }
    return nfds;
}

int main(){
    srand(time(NULL));
    if(atexit(server_exit) != 0){
        fprintf(stderr, "at exit register error\n");
        exit(1);
    }
    //signal(SIGKILL, &server_exit);
    signal(SIGINT, &server_exit);


    struct Player players[MAX_CLIENTS];
    struct Game game = init_board();
//    prepare_frame(&game);
//    drawCurrentState(game);
//    server_exit();

    int sock_fd = create_socket_fd();
    bind_socket(sock_fd);

    struct pollfd fds[MAX_CLIENTS + 1];

    fds[0].fd = sock_fd;
    fds[0].events = POLLIN;

    int nfds = 1;
    while(1){
        int ret = poll(fds, nfds, -1);
        if (ret == -1) {
            server_exit();
        }

        if (fds[0].revents & POLLIN) {
            struct sockaddr_in client_addr;

            memset(&client_addr, 0, sizeof(client_addr));
            socklen_t addr_len = sizeof(client_addr);
            int client_fd = accept(sock_fd, (struct sockaddr *) &client_addr, &addr_len);
            if(client_fd == -1){
                server_exit();
            }

            nfds = handle_new_connection(players, &game, fds, nfds, client_fd);

        }

        for (size_t i = 1; i < nfds; i++) {
            if (fds[i].revents & POLLIN) {
                char input_key;
                ret = (int) recv(fds[i].fd, &input_key, 1 * sizeof(char), 0);


                if (ret == -1) {
                    perror("recv");
                    continue;
                } else if (ret == 0) {
                    printf("Client disconnected\n");
                    close(fds[i].fd);

                    // Remove client_fd from pollfd array
                    fds[i] = fds[nfds - 1];
                    nfds--;
                    i--;
                    continue;
                }


                if(input_key == 'r'){
                    input_key = players[i-1].last_move;
                }

                //printf("HANDLE %zu |||||| Last %c Now %c\n", i, inputs[0], inputs[1]);

                handleButton(&game, &players[i - 1], input_key);

                for(size_t j = 0; j < nfds - 1; j++){
                    if(j == i - 1) continue;
                    for(size_t k = 0; k < players[j].length; k++){
                        if(players[i-1].body[0].x == players[j].body[k].x && players[i-1].body[0].y == players[j].body[k].y){
                            game.is_running = false;
                            memset(game.info_message, '\0', EXIT_MESSAGE_BUFFER_SIZE);
                            sprintf(game.info_message, "Player %zu wins the game!!!", players[j].player_number);
                        }
                    }
                }
            }
        }

        send_updated_game_to_clients(&game, fds, nfds, players);

        if(!game.is_running){
            exit(0);
        }
    }
}