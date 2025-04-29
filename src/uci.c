#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "aether.h"

void parse_position(POSITION *pstn, char *line) {
    char *current_ptr = line + 8;
    int idx;

    if (strncmp(current_ptr, " startpos", 9) == 0) {
        idx = update_position(pstn, START_POS);
        current_ptr += 10;
    } else if (strncmp(current_ptr, " fen ", 5) == 0) {
        current_ptr += 5;
        idx = update_position(pstn, current_ptr);
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
            if (is_null_move(mv = string_to_move(pstn, current_ptr))) {
                break;
            }
            make_move(pstn, mv);

            if ((next = strstr(current_ptr, " ")) == NULL) {
                break;
            }

            current_ptr = next + 1;
        }
    }
}

void parse_go(POSITION *pstn, char *line, SEARCH_INFO *s_info) {
    char *cmd, *end;
    int n;

    if ((cmd = strstr(line, "perft")) != NULL) {
        char *tok = strtok(line, " ");
        tok = strtok(NULL, " ");

        if ((tok = strtok(NULL, " ")) != NULL) {
            if ((n = strtol(tok, &end, 10)) != 0) {
                divide(pstn, n);
            }
        }

        return;
    }

    int depth = MAX_DEPTH, moves_to_go = 30, time = -1, inc = 0;
    s_info->time_set = false;

    if (pstn->side == WHITE && (cmd = strstr(line, "wtime")) != NULL) {
        if ((n = strtol(cmd + 6, &end, 10)) != 0) {
            time = n;
        }
    }

    if (pstn->side == BLACK && (cmd = strstr(line, "btime")) != NULL) {
        if ((n = strtol(cmd + 6, &end, 10)) != 0) {
            time = n;
        }
    }

    if (pstn->side == WHITE && (cmd = strstr(line, "winc")) != NULL) {
        if ((n = strtol(cmd + 5, &end, 10)) != 0) {
            inc = n;
        }
    }

    if (pstn->side == BLACK && (cmd = strstr(line, "binc")) != NULL) {
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

    search(pstn, s_info);
}

void display_info(POSITION *pstn) {
    char fen_str[92];
    board_to_fen(pstn, fen_str);
    print_board(pstn);
    printf("\nFEN: %s\nKey: %lu\nCheckers: ", fen_str, pstn->key);
    if (pstn->check) {
        printf("%s", coord_to_string(pstn->fst_checker));
        if (pstn->check == DOUBLE_CHECK) {
            printf(", %s", coord_to_string(pstn->snd_checker));
        }
    }
    printf("\n");
    printf("En passant: %s\nCastling Rights: ", coord_to_string(pstn->ep_sq));
    if (pstn->c_rights & WHITE_KINGSIDE) {
        printf("K");
    }
    if (pstn->c_rights & WHITE_QUEENSIDE) {
        printf("Q");
    }
    if (pstn->c_rights & BLACK_KINGSIDE) {
        printf("k");
    }
    if (pstn->c_rights & BLACK_QUEENSIDE) {
        printf("q");
    }
    printf("\nPly: %d\n", pstn->ply);
}

void uci_loop(void) {
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    char buf[INPUT_BUFFER_SIZE];

    printf("id name %s\n", NAME);
    printf("id author %s\n", AUTHOR);
    printf("uciok\n");

    POSITION *pstn = new_position();
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
            parse_position(pstn, buf);
        } else if (strncmp(buf, "ucinewgame", 10) == 0) {
            update_position(pstn, START_POS);
        } else if (strncmp(buf, "go", 2) == 0) {
            parse_go(pstn, buf, s_info);
        } else if (strncmp(buf, "quit", 4) == 0) {
            s_info->quit = true;
        } else if (strncmp(buf, "uci", 3) == 0) {
            printf("id name %s\n", NAME);
            printf("id author %s\n", AUTHOR);
            printf("uciok\n");
        } else if (strncmp(buf, "d", 1) == 0) {
            display_info(pstn);
        } else if (strncmp(buf, "speed", 5) == 0) {
            uint64_t start = get_time();
            divide(pstn, 6);
            double t = (get_time() - start) / 1000;
            printf("Time taken: %.2f\n", t);
        }

        if (s_info->quit == true) {
            break;
        }
    }

    free(s_info);
    free_position(pstn);
}

int main(void) {
    uci_loop();
    return 0;
}