#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "aether.h"

void parse_position(char *line) {
    char *current_ptr = line + 8;
    int idx;

    if (strncmp(current_ptr, " startpos", 9) == 0) {
        idx = set_position(START_POS);
        current_ptr += 10;
    } else if (strncmp(current_ptr, " fen ", 5) == 0) {
        current_ptr += 5;
        idx = set_position(current_ptr);
        current_ptr += (idx + 2);
    }

    if (idx == -1) { // an invalid fen string was passed
        return;
    }

    if (strncmp(current_ptr, "moves ", 6) == 0) {
        current_ptr += 6;
        char *next;
        move_t mv;

        while (true) {
            if (is_null_move(mv = string_to_move(current_ptr))) {
                break;
            }
            make_move(mv);

            if ((next = strstr(current_ptr, " ")) == NULL) {
                break;
            }

            current_ptr = next + 1;
        }
    }
}

void parse_go(char *line, SEARCH_INFO *s_info) {
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
    s_info->start_time = get_time();

    if (time != -1) {
        time /= moves_to_go;
        s_info->time_set = true;
        s_info->stop_time = s_info->start_time + time + inc;
    }

    search(s_info);
}

void uci_loop(void) {
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    char buf[INPUT_BUFFER_SIZE];

    printf("id name %s\n", NAME);
    printf("id author %s\n", AUTHOR);
    printf("uciok\n");

    set_position(START_POS);
    SEARCH_INFO *s_info = malloc(sizeof(SEARCH_INFO));

    while (true) {
        memset(buf, 0, sizeof(buf));
        fflush(stdout);

        if (
            fgets(buf, sizeof(buf), stdin) == NULL
            || buf[0] == '\n'
        ) {
            continue;
        }

        if (strncmp(buf, "isready", 7) == 0) {
            printf("readyok\n");
            continue;
        } else if (strncmp(buf, "position", 8) == 0) {
            parse_position(buf);
        } else if (strncmp(buf, "ucinewgame", 10) == 0) {
            set_position(START_POS);
        } else if (strncmp(buf, "go", 2) == 0) {
            parse_go(buf, s_info);
        } else if (strncmp(buf, "quit", 4) == 0) {
            s_info->quit = true;
        } else if (strncmp(buf, "uci", 3) == 0) {
            printf("id name %s\n", NAME);
            printf("id author %s\n", AUTHOR);
            printf("uciok\n");
        } else if (strncmp(buf, "d", 1) == 0) {
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
        }

        if (s_info->quit == true) {
            break;
        }
    }

    free(s_info);
}

int main(void) {
    init_engine();
    uci_loop();
    free_tables();
    return 0;
}