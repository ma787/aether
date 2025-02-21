#ifndef UTILS_H
#define UTILS_H 1

int get_rank(int pos) { return (pos >> 4) - 4;}
int get_file(int pos) { return (pos & 0x0F) - 4;}

#endif