#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
// #include <sha1.h>

#include "defs.h"
#include "connect.h"
#include "shm.h"
#include "nmmorris-ai/think.h"

static inline
void print_help(void) 
{
    printf("How to use this program\n");
    printf("bin/sysprak-client -g <game-id> -p <player-number> -c <config-file>\n");
}

static inline
void set_args(int argc, char *argv[], clientinfo_t *ci) 
{
    ci->player_num_wish = -1;
    strcpy(ci->conf_name, "client.conf");
    
    int32_t option;
    while ((option = getopt(argc, argv, "g:p:c:")) != -1) {
        switch (option)
        {
        case 'p':
            ci->player_num_wish = (int16_t) atoi(optarg); break;
        case 'g':
            strncpy(ci->game_id, optarg, ID_LEN - 1); break;
        case 'c':
            strncpy(ci->conf_name, optarg, CONF_LEN - 1); break;
        default:
            print_help();
            exit(1);
        }
    }
}

static inline
void init_clientinfo(int argc, char *argv[], clientinfo_t *ci) 
{
    set_args(argc, argv, ci);
    char conf_path[CONF_LEN + 30];
    sprintf(conf_path, "config/%s", ci->conf_name);
    read_config(conf_path, &ci->config);
    ci->keys_shm[0] = ftok(__FILE__, getpid());
    ci->keys_shm[1] = ftok(__FILE__, getpid() + 1);
    ci->thinker_attach = false;
}

static inline
void init_clientinfo_pids(clientinfo_t *ci)
{
    ci->pid_connector = getpid();
    ci->pid_thinker = getppid();
}

// flag that is set by sighandler
volatile __sig_atomic_t g_think_flag = 0;

void signal_handler_func(int32_t signum)
{
    //printf("signal received\n");
    if (signum == SIGUSR1) {
        g_think_flag = 1;
    }
}

int main(int argc, char *argv[]) 
{
    if (argc <= 1) {
        print_help();
        return -1;
    }

    int32_t pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("pipe()");
        return -1;
    }

    int32_t ci_id;
    clientinfo_t *ci = shm_create_ci(&ci_id);
    if (ci == NULL) {
        perror("shm_create_ci()");
        return -1;
    }

   init_clientinfo(argc, argv, ci);

    pid_t pid;
    pid = fork();
    if (pid < 0) {
        perror("fork()");
        return -1;
    } else if (pid > 0) { 
        /* thinker process */
        close(pipe_fd[0]);

        struct sigaction sa;
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = &signal_handler_func;
        if (sigaction(SIGUSR1, &sa, NULL) == -1) {
            perror("sigaction()");
        }

        int32_t pi_id, pc_id;
        playerinfo_t *pi;
        piece_t *pc = NULL;     
        
        while (1) {
            if (ci->thinker_attach) {
                ci->thinker_attach = false;
                pi = shm_create_pi(&pi_id, ci->keys_shm[0], ci->num_players);
                pc = shm_create_pc(&pc_id, ci->keys_shm[1], ci->num_pieces);
            }

            if (g_think_flag) {
                g_think_flag = 0;
                char movestr[6];
                think(movestr, ci->time_to_move, pc, ci->num_caps, &ci->client);
                write(pipe_fd[1], movestr, 6);
            }

            int exit_code;
            if (waitpid(pid, &exit_code, WNOHANG) != 0) {
                close(pipe_fd[1]);
                shm_destroy_ci(ci_id, ci);
                return exit_code;
            }
        }    

    } else { 
        /* connector process */
        close(pipe_fd[1]);
        err_t ec;
        init_clientinfo_pids(ci);

        ec = perform_connection(ci, pipe_fd[0]);
        close(pipe_fd[0]);
        return ec;
    }
}