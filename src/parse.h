#ifndef PARSE_H
#define PARSE_H

void parse_gamekind(char *buf, clientinfo_t *ci);
void parse_gamename(char *buf, clientinfo_t *ci);
void parse_client_player(char *buf, playerinfo_t *pi);
void parse_num_players(char* buf, clientinfo_t *ci);
void parse_other_player(char *buf, playerinfo_t *pi);
void parse_server_phase(char *buf, clientinfo_t *ci);
void parse_move_time(char *buf, clientinfo_t *ci);
void parse_capture(char *buf, clientinfo_t *ci);
void parse_piece(char *buf, piece_t *pc);
void parse_win(char *buf, clientinfo_t *ci, playerinfo_t *pi);

void print_player(playerinfo_t *pi);
void print_winner(playerinfo_t *pi, clientinfo_t *ci);

#endif /* PARSE_H */