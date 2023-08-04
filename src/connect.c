#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <signal.h>

#include "defs.h"
#include "connect.h"
#include "parse.h"

i32 socket_connect(config_t *config)
{
    i32 sock_fd;
    struct sockaddr_in server_addr;
    struct hostent *host;
    struct in_addr **ip_host;
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }

    host = gethostbyname(config->host);
    ip_host = (struct in_addr**) host->h_addr_list;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(config->port);
    server_addr.sin_addr = **ip_host;

    if ((connect(sock_fd, (struct sockaddr*) &server_addr, sizeof(server_addr))) != 0) {
        perror("connect()");
        return -2;
    }

    return sock_fd;
}

err_t get_server_line(i32 sock_fd, char *buf) 
{
    memset(buf, 0, BUF_SIZE);
    ssize_t b_recvd, b_total = 0;

    while (buf[b_total - 1] != '\n') {
        b_recvd = recv(sock_fd, buf + b_total, 1, 0);
        if (b_recvd == -1) {
            perror("recv()");
            return RECV_ERROR;
        }
        ++b_total;
    }

    printf("S: %s", buf);
    fflush(stdout);
    if (*buf == '+') {
        return NO_ERROR;
    } else if (*buf == '-') {
        return SERVER_ERROR;
    } else {
        return COMMUNICATION_ERROR;
    }
}

err_t put_client_line(i32 sock_fd, const char *format, ...) 
{
    char buf[BUF_SIZE] = {0};
    va_list args;
    va_start(args, format);

    vsprintf(buf, format, args);
    if ((send(sock_fd, buf, strlen(buf), 0)) == -1) {
        perror("send()");
        return SEND_ERROR;
    }

    printf("C: %s", buf);
    fflush(stdout);
    return NO_ERROR;
}

err_t prolog(i32 sock_fd, clientinfo_t *ci, i32 *pi_id, playerinfo_t **pi)
{
    char buf[BUF_SIZE];
    err_t ec;

    /* S: ACCEPTING CONNECTIONS */
    ec = get_server_line(sock_fd, buf);
    err_check_return(ec);   
    ec = get_server_line(sock_fd, buf);
    err_check_return(ec);
    
    /* C: CLIENT VERSION */
    ec = put_client_line(sock_fd, "VERSION %s\n", "3.1");
    err_check_return(ec);

    /* S: ACCEPT CLIENT VERSION */
    ec = get_server_line(sock_fd, buf);
    err_check_return(ec);

    /* C: GAME ID */
    ec = put_client_line(sock_fd, "ID %s\n", ci->game_id);
    err_check_return(ec);

    /* S: PLAYING GAMEKIND */
    ec = get_server_line(sock_fd, buf);
    err_check_return(ec);
    parse_gamekind(buf, ci);

    ec = get_server_line(sock_fd, buf);
    err_check_return(ec);
    parse_gamename(buf, ci);

    /* C: PLAYER NUMBER */
    if (ci->player_num_wish > 0) {
        ec = put_client_line(sock_fd, "PLAYER %d\n", ci->player_num_wish - 1);
    } else {
        ec = put_client_line(sock_fd, "PLAYER\n");
    }
    err_check_return(ec);

    /* S: PLAYERS */
    ec = get_server_line(sock_fd, buf);
    err_check_return(ec);
    parse_client_player(buf, &ci->client);
    ec = get_server_line(sock_fd, buf);
    err_check_return(ec);
    parse_num_players(buf, ci); 

    *pi = shm_create_pi(pi_id, ci->keys_shm[0], ci->num_players);
    for (i32 player = 0; player < ci->num_players - 1; ++player) {
        ec = get_server_line(sock_fd, buf);
        err_check_return(ec);
        parse_other_player(buf, pi[player]);
    }

    /* S: ENDPLAYERS */
    ec = get_server_line(sock_fd, buf);
    err_check_return(ec);

    return NO_ERROR;
}

err_t gameplay_wait(i32 sock_fd) 
{
    err_t ec;
    ec = put_client_line(sock_fd, "OKWAIT\n");
    return ec;
}

err_t receive_pieces(i32 sock_fd, clientinfo_t *ci, piece_t *pc)
{
    char buf[BUF_SIZE];
    err_t ec;

    /* S: PIECELIST */
    ec = get_server_line(sock_fd, buf);
    err_check_return(ec);
    for (piece_t *cur = pc; cur < pc + ci->num_pieces; ++cur) {     
        /* S: PIECE */
        ec = get_server_line(sock_fd, buf);
        err_check_return(ec);
        parse_piece(buf, cur);
    }

    /* S: ENDPIECELIST */
    ec = get_server_line(sock_fd, buf);
    err_check_return(ec);

    return NO_ERROR;
}

err_t gameplay_move(i32 sock_fd, clientinfo_t *ci, piece_t *pc) 
{
    char buf[BUF_SIZE];
    err_t ec;
    /* S: CAPTURE */
    ec = get_server_line(sock_fd, buf);
    err_check_return(ec);
    parse_capture(buf, ci);
    
    ec = receive_pieces(sock_fd, ci, pc);
    err_check_return(ec);

    ec = put_client_line(sock_fd, "THINKING\n");
    err_check_return(ec);

    /* S: OKTHINK */
    ec = get_server_line(sock_fd, buf);
    err_check_return(ec);

    kill(ci->pid_thinker, SIGUSR1);

    return NO_ERROR;
}

err_t gameplay_play(i32 sock_fd, i32 pipe_fd)
{
    char buf[BUF_SIZE];
    err_t ec;

    char move[6] = { 0 };
    read(pipe_fd, move, 6);
    ec = put_client_line(sock_fd, "PLAY %s\n", move);
    err_check_return(ec);

    /* S: MOVEOK */
    ec = get_server_line(sock_fd, buf);
    err_check_return(ec);

    return NO_ERROR;
}

err_t gameplay_gameover(i32 sock_fd, clientinfo_t *ci, playerinfo_t *pi, piece_t *pc)
{
    char buf[BUF_SIZE];
    err_t ec;

    ec = receive_pieces(sock_fd, ci, pc);
    err_check_return(ec);

    for (i32 i = 0; i < ci->num_players; ++i) {
        /* S: PLAYERXWON */
        ec = get_server_line(sock_fd, buf);
        err_check_return(ec);
        parse_win(buf, ci, pi);
    }

    print_winner(pi, ci);

    /* S: QUIT */
    ec = get_server_line(sock_fd, buf);
    err_check_return(ec);

    return NO_ERROR;
}

err_t gameplay(i32 sock_fd, clientinfo_t *ci, playerinfo_t *pi, piece_t *pc) 
{
    char buf[BUF_SIZE];
    err_t ec;
    /* S: WAIT | MOVE | GAMEOVER */
    ec = get_server_line(sock_fd, buf);
    err_check_return(ec);
    parse_server_phase(buf, ci);
    switch(ci->serverphase) 
    {
    case WAIT:
        ec = gameplay_wait(sock_fd);
        err_check_return(ec);
        break;
    
    case MOVE:
        parse_move_time(buf, ci);
        ec = gameplay_move(sock_fd, ci, pc);
        err_check_return(ec);
        break;
    
    case GAMEOVER:
        ec = gameplay_gameover(sock_fd, ci, pi, pc);
        err_check_return(ec);
        break;
    
    case INVALID_PHASE:
        return INVALID_ARGUMENT;
    
    default:
        return INVALID_ARGUMENT;
    }
    return NO_ERROR;
}

err_t perform_connection(clientinfo_t *ci, i32 pipe_fd) 
{
    err_t ec = NO_ERROR;
    i32 sock_fd;
    sock_fd = socket_connect(&ci->config);
    if (sock_fd == -1) {
        return SOCK_ERROR;
    }

    i32 pi_id;
    playerinfo_t *pi = NULL;
    ec = prolog(sock_fd, ci, &pi_id, &pi);
    err_check_return(ec);

    print_player(&ci->client);
    for (i32 i = 0; i < ci->num_players - 1; ++i) {
        print_player(&pi[i]);
    }

    i32 pc_id;
    piece_t *pc = NULL;
    switch (ci->gamekind) {
        case NMMORRIS:
            pc = shm_create_pc(&pc_id, ci->keys_shm[1], ci->num_pieces);
            break;
        default:
            fprintf(stderr, "game type not supported by this client");
            return INVALID_ARGUMENT;          
    }

    ci->thinker_attach = true;

    nfds_t nfds = 2;
    struct pollfd poll_fds[2];
    poll_fds[0].fd = sock_fd;
    poll_fds[0].events = POLLIN;
    poll_fds[1].fd = pipe_fd;
    poll_fds[1].events = POLLIN;
    
    i32 poll_ret;
    while (true) {
        poll_ret = poll(&poll_fds[0], nfds, TIMEOUT_MS);
        if (poll_ret == -1) {
            perror("poll()");
            return POLL_ERROR;
        }

        if (poll_fds[0].revents & POLLIN) {
            /* socket is readable */
            ec = gameplay(sock_fd, ci, pi, pc);
            if (ec != NO_ERROR) {
                break;
            }
        }

        if (poll_fds[1].revents & POLLIN) {
            /* pipe is readable */
            gameplay_play(sock_fd, pipe_fd);
        }
    }

    shm_destroy_pi(pi_id, pi);
    shm_destroy_pc(pc_id, pc);
    close(sock_fd);
    return ec;
}