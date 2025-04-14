#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "aether.h"

char fen_str[92];
char best_move[6];

void quit_engine(void) {
    free_tables();
    exit(0);
}

void parse_position(char *line) {
    char *cmd = strtok(line, " ");

    if ((cmd = strtok(NULL, " ")) == NULL) {
        return;
    }

    if (strcmp(cmd, "startpos") == 0) {
        set_position(START_POS);
    } else if (strcmp(cmd, "fen") == 0) {
        int res = 0;
        fen_str[0] = '\0';

        for (int i = 2; i < 8; i++) {
            if ((cmd = strtok(NULL, " ")) == NULL) {
                res = -1;
                break;
            }
            strcat(fen_str, cmd);
            strcat(fen_str, " ");
        }

        int end = strcspn(fen_str, "\0");
        fen_str[end - 1] = '\0';

        if (res != 0 || !fen_match(fen_str)) {
            fen_str[0] = '\0';
            return;
        }

        if (set_position(fen_str) != 0) {
            exit(-1);
        }

        fen_str[0] = '\0';
    } else {
        return;
    }

    move_t mv;

    if ((cmd = strtok(NULL, " ")) != NULL && strcmp(cmd, "moves") == 0) {
        while ((cmd = strtok(NULL, " ")) != NULL) {
            if (is_null_move((mv = string_to_move(cmd)))) {
                break;
            }
            make_move(mv);
        }
    }
}

void parse_go(char *line) {
    char *cmd, *end;
    int n;

    if ((cmd = strstr(line, "perft")) != NULL) {
        char *tok = strtok(line, " ");
        tok = strtok(NULL, " ");

        if ((tok = strtok(NULL, " ")) != NULL) {
            if ((n = strtol(tok, &end, 10)) != 0) {
                divide(n);
            }
        }

        return;
    }

    SEARCH_INFO *s_info = malloc(sizeof(SEARCH_INFO));

    int depth = MAX_DEPTH, moves_to_go = 30, time = -1, inc = 0;
    s_info->time_set = false;

    if (side == WHITE && (cmd = strstr(line, "wtime")) != NULL) {
        if ((n = strtol(cmd + 6, &end, 10)) != 0) {
            time = n;
        }
    }

    if (side == BLACK && (cmd = strstr(line, "btime")) != NULL) {
        if ((n = strtol(cmd + 6, &end, 10)) != 0) {
            time = n;
        }
    }

    if (side == WHITE && (cmd = strstr(line, "winc")) != NULL) {
        if ((n = strtol(cmd + 5, &end, 10)) != 0) {
            inc = n;
        }
    }

    if (side == BLACK && (cmd = strstr(line, "binc")) != NULL) {
        if ((n = strtol(cmd + 5, &end, 10)) != 0) {
            inc = n;
        }
    }

    if ((cmd = strstr(line, "movestogo")) != NULL) {
        if ((n = strtol(cmd + 10, &end, 10)) != 0) {
            moves_to_go = n;
        }
    }

    if ((cmd = strstr(line, "depth")) != NULL) {
        if ((n = strtol(cmd + 6, &end, 10)) != 0) {
            depth = n;
        }
    }

    if ((cmd = strstr(line, "movetime")) != NULL) {
        if ((n = strtol(cmd + 9, &end, 10)) != 0) {
            time = n;
            moves_to_go = 1;
        }
    }

    s_info->depth = depth;

    if (time != -1) {
        time /= moves_to_go;
        time -= 50; // avoid overrunning time

        s_info->time_set = true;
        s_info->start_time = get_time();
        s_info->stop_time = s_info->start_time + time + inc;
    }

    search(s_info);
    bool quit_prog = s_info->quit;
    free(s_info);

    if (quit_prog == true) {
        quit_engine();
    }
}

int main(void) {
    set_position(START_POS);

    while (1) {
        char buf[256];
        
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            printf("Error reading input\n");
            return -1;
        }

        buf[strcspn(buf, "\n")] = 0;
        char *cmd;

        if ((cmd = strstr(buf, "position")) != NULL) {
            parse_position(cmd);
        } else if ((cmd = strstr(buf, "go")) != NULL) {
            parse_go(cmd);
        } else if (strcmp(buf, "d") == 0) {
            char fen_str[92];
            board_to_fen(fen_str);
            print_board();
            printf("\nFEN: %s\nKey: %lu\nCheckers: ", fen_str, board_hash);
            if (check_info) {
                printf("%s", coord_to_string((check_info >> 2) & 0xFF));
                if ((check_info & 3) == DOUBLE_CHECK) {
                    printf(", %s", coord_to_string(check_info >> 10));
                }
            }
            printf("\n");
        } else if (strcmp(buf, "uci") == 0) {
            printf("id name %s\n", NAME);
            printf("id author %s\n", AUTHOR);
            printf("uciok\n");
        } else if (strcmp(buf, "quit") == 0) {
            quit_engine();
        }
    }
}