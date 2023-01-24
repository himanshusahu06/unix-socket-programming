#include "common.h"

void err_sys(const char* s) {
    perror(s);
    exit(0);
}

void err_quit(const char* s) {
    printf("ERROR: %s\n", s);
    exit(0);
}