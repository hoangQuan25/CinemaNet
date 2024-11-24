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

int seat_label_to_id(const char *seat_label) {
    if (strlen(seat_label) < 2) {
        return -1;
    }
    char row_char = toupper(seat_label[0]);
    int row_number = row_char - 'A';
    int seat_number = atoi(&seat_label[1]) - 1;
    int seats_per_row = 10;

    if (row_number < 0 || seat_number < 0 || seat_number >= seats_per_row) {
        return -1;
    }

    int seat_id = row_number * seats_per_row + seat_number;
    return seat_id;
}

void seat_id_to_label(int seat_id, char *seat_label) {
    int seats_per_row = 10;
    int row_number = seat_id / seats_per_row;
    int seat_number = seat_id % seats_per_row + 1;

    char row_char = 'A' + row_number;

    sprintf(seat_label, "%c%d", row_char, seat_number);
}


bool verify_token(const char *token) {
    // Implement your token verification logic here
    // For simplicity, let's assume any non-empty token is valid
    if (token == NULL || strlen(token) == 0) {
        return false;
    }
    return true;
}

int seat_id_to_seat_map_index(int seat_id, int seats_per_row) {
    int row_number = seat_id / seats_per_row;
    int seat_number_in_row = seat_id % seats_per_row;
    int seat_map_index = seat_id + row_number; // Add the number of separators

    return seat_map_index;
}

