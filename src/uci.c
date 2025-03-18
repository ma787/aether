#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "perft.h"


int main(void) {
    info *pstn = new_position(START_POS);

    if (pstn == NULL) {
        printf("Error: could not initialise position.");
        return -1;
    }

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
            cmd = strtok(NULL, " ");

            if (cmd == NULL) {
                continue;
            } else if (strcmp(cmd, "startpos") == 0) {
                clear_position(pstn);
                pstn = new_position(START_POS);
                continue;
            } else if (strcmp(cmd, "fen") != 0) {
                continue;
            }

            int res = 0;
            char fen_str[92];

            for (int i = 2; i < 8; i++) {
                cmd = strtok(NULL, " ");
                if (cmd == NULL) {
                    res = -1;
                    break;
                }
                strcat(fen_str, cmd);
                strcat(fen_str, " ");
            }

            int end = strcspn(fen_str, "\0");
            fen_str[end - 1] = '\0';

            if (res != 0 || fen_match(fen_str) != 0) {
                continue;
            }

            pstn = new_position(fen_str);

        } else if (strcmp(cmd, "go") == 0) {
            if ((cmd = strtok(NULL, " ")) != NULL && strcmp(cmd, "perft") == 0) {
                cmd = strtok(NULL, " ");
                if (cmd == NULL) {
                    continue;
                }

                char *end;
                int n;
                
                if ((n = strtol(cmd, &end, 10)) != 0) {
                    divide(pstn, n);
                }
            }
        } else if (strcmp(cmd, "quit") == 0) {
            return 0;
        } else if (strcmp(cmd, "uci") == 0) {
            printf("id name %s\n", NAME);
            printf("id author %s\n", AUTHOR);
            printf("uciok");
        }
    }
}