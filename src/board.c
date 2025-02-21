#include <stdio.h>
#include <string.h>
#include "constants.h"

void init_board(int *arr) {
    memset(arr + 0x22, GUARD, 12 * sizeof(int));
    memset(arr + 0x32, GUARD, 12 * sizeof(int));
    memset(arr + 0xC2, GUARD, 12 * sizeof(int));
    memset(arr + 0xD2, GUARD, 12 * sizeof(int));

    int row[12] = {GUARD, GUARD, 0, 0, 0, 0, 0, 0, 0, 0, GUARD, GUARD};

    for (int i = 0; i < 0x80; i += 0x10) {
        memcpy(arr + (0x42 + i), row, 12 * sizeof(int));
    }
}

void clear_board(int *arr) {
    for (int i = 0; i < 0x80; i += 0x10) {
        memset(arr + (0x44 + i), 0, 8 * sizeof(int));
    }
}

void board_to_string(int *arr, char *b_str) {
    int i = 0xB4;
    int j = 0;
    int square;

    while (i != 0x4C) {
        square = arr[i];

        if (square == GUARD) {
            b_str[j++] = '\n';
            i -= 0x18;
        } else {
            b_str[j++] = LETTERS[square];
            i++;
        }
    }
}

void print_board(int *arr) {
    char b_str[72];
    board_to_string(arr, b_str);
    printf("%s\n", b_str);
}