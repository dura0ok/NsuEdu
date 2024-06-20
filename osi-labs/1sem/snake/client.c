#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <sys/poll.h>
#include "config.h"
#include "client_engine.h"
#include <unistd.h>
#include <errno.h>


void client_exit() {
    tc_exit_alt_screen();
    reset_keypress();
    if(errno != 0) {
        fprintf(stderr, "Error %s\n", strerror(errno));
    }
    exit(0);
}

int connect_to_server() {
    int sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        client_exit();
    }

    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1) {
        perror("setsockopt");
        client_exit();
    }

    struct sockaddr_un serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strncpy(serv_addr.sun_path, SOCKET_PATH, sizeof(serv_addr.sun_path) - 1);

    if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("connect");
        client_exit();
    }
    return sock_fd;
}




void init_receive_game_info(int sock_fd, struct Game *game) {
    printf("receive init\n");
    int res = (int)recv(sock_fd, game, sizeof(struct Game), 0);
    if (res == -1 || res == 0) {
        errno = EBUSY;
        perror("Server close connection with you when trying go init field.\n Maybe Server is full\n");
        client_exit();
    }
}

void handle_server_input(int sock_fd, struct Game *game) {
    int res = (int)recv(sock_fd, game, sizeof(*game), 0);
    if(!(*game).is_running){
        drawCurrentState(*game);
        sleep(5);
        client_exit();
    }
    if (res == -1) {
        printf("kek1\n");
        perror("recv");
        exit(EXIT_FAILURE);
    }
}

int main(){
    srand(time(NULL));
    signal(SIGINT, SIG_IGN);

    if(atexit(client_exit) != 0){
        fprintf(stderr, "at exit register error\n");
        exit(1);
    }

    set_keypress();
    tc_enter_alt_screen();


    int sock_fd = connect_to_server();
    struct pollfd fds[2];
    memset(&fds, 0, sizeof(fds));
    fds[0].fd = 0;
    fds[0].events = POLLIN; // without blocking
    fds[1].fd = sock_fd;
    fds[1].events = POLLIN;

    struct Game game;
    init_receive_game_info(sock_fd, &game);


    char lastInput = 'x';

    while (1) {
        //clearScreen();
        drawCurrentState(game);

        int ret = poll(fds, 2, TIMEOUT);
        if (ret == -1) {
            continue;
        }

        if (ret > 0) {
            if(fds[1].revents & POLLIN){
                handle_server_input(sock_fd, &game);
                continue;
            }

            if(fds[0].revents & POLLIN){
                char c = (char) getc(stdin);
                if (c == 'q') {
                    client_exit();
                    break;
                }


                if (send(fds[1].fd, &c, 1 * sizeof(char), 0) == -1) {
                    perror("send");
                    client_exit();
                }
            }

        } else {
            char input_key = 'r';
            if (send(fds[1].fd, &input_key, 1 * sizeof(char), 0) == -1) {
                perror("send");
                client_exit();
            }
        }
    }
}