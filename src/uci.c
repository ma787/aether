#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "aether.h"

char fen_str[92];
char best_move[6];

int main(void) {
    set_position(START_POS);

    while (1) {
        char buf[256];
        
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            printf("Error reading input\n");
            return -1;
        }

        buf[strcspn(buf, "\n")] = 0;
        char *cmd = strtok(buf, " ");

        if (cmd == NULL) {
            continue;
        }

        if (strcmp(cmd, "position") == 0) {
            if ((cmd = strtok(NULL, " ")) == NULL) {
                continue;
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
                    continue;
                }

                if (set_position(fen_str) != 0) {
                    return -1;
                }

                fen_str[0] = '\0';
            } else {
                continue;
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

        } else if (strcmp(cmd, "go") == 0) {
            if ((cmd = strtok(NULL, " ")) != NULL) {
                if (strcmp(cmd, "perft") == 0) {
                    if ((cmd = strtok(NULL, " ")) == NULL) {
                        continue;
                    }
    
                    char *end;
                    int n;
                    
                    if ((n = strtol(cmd, &end, 10)) != 0) {
                        divide(n);
                    }
                } else if (strcmp(cmd, "depth") == 0) {
                    if ((cmd = strtok(NULL, " ")) == NULL) {
                        continue;
                    }
    
                    char *end;
                    int n;
                    
                    if ((n = strtol(cmd, &end, 10)) != 0) {
                        SEARCH_INFO *s_info = malloc(sizeof(SEARCH_INFO));
                        s_info->depth = n;
                        search(s_info);
                        free(s_info);
                    }
                }
            } 
        } else if (strcmp(cmd, "d") == 0) {
            char fen_str[92];
            board_to_fen(fen_str);
            print_board();
            printf("\nFEN: %s\nKey: %lu\n", fen_str, board_hash);
        } else if (strcmp(cmd, "uci") == 0) {
            printf("id name %s\n", NAME);
            printf("id author %s\n", AUTHOR);
            printf("uciok\n");
        } else if (strcmp(cmd, "quit") == 0) {
            free_tables();
            return 0;
        }
    }
}