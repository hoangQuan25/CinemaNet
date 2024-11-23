// utils.c
#include <string.h>
#include "utils.h"

void trim_newline(char *str) {
    size_t len = strlen(str);
    if (len == 0)
        return;
    if (str[len - 1] == '\n')
        str[len - 1] = '\0';
    if (len > 1 && str[len - 2] == '\r')
        str[len - 2] = '\0';
}

void trim_whitespace(char *str) {
    // Trim leading whitespace
    while(isspace((unsigned char)*str)) str++;

    // Trim trailing whitespace
    char *end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator
    end[1] = '\0';
}
