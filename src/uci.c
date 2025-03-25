#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "constants.h"
#include "engine.h"
#include "position.h"
#include "move.h"
#include "perft.h"

char fen_str[92];
char best_move[6];

int main(void) {
    set_position(START_POS);

    while (1) {
        char buf[256];
        
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            printf("Error reading input");
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
            } else if (strcmp(cmd, "startpos") == 0) {
                set_position(START_POS);
                continue;
            } else if (strcmp(cmd, "fen") != 0) {
                continue;
            }

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

            if (res != 0 || fen_match(fen_str) != 0) {
                fen_str[0] = '\0';
                continue;
            }

            if (set_position(fen_str) != 0) {
                return -1;
            }
            
            fen_str[0] = '\0';

            if ((cmd = strtok(NULL, " ")) != NULL && strcmp(cmd, "moves") == 0) {
                while ((cmd = strtok(NULL, " ")) != NULL) {
                    make_move(of_string(cmd));
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
                        search(n, best_move);
                        printf("%s\n", best_move);
                    }
                }
            } 
        } else if (strcmp(cmd, "d") == 0) {
            char fen_str[92];
            to_fen(fen_str);
            printf("%s\n", fen_str);
        } else if (strcmp(cmd, "uci") == 0) {
            printf("id name %s\n", NAME);
            printf("id author %s\n", AUTHOR);
            printf("uciok");
        } else if (strcmp(cmd, "quit") == 0) {
            return 0;
        }
    }
}