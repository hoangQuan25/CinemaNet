// utils.c
#include <string.h>
#include "utils.h"
#include "user.h"

void trim_newline(char *str) {
    size_t len = strlen(str);
    if (len == 0)
        return;
    if (str[len - 1] == '\n')
        str[len - 1] = '\0';
    if (len > 1 && str[len - 2] == '\r')
        str[len - 2] = '\0';
}

void trim_spaces(char *str) {
    // Trim leading spaces
    char *start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }

    // Trim trailing spaces
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
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

void parse_users(const char *data, UserInfo users[], int *user_count) {
    *user_count = 0;
    if (!data) return;

    char data_copy[BUFFER_SIZE];
    strncpy(data_copy, data, BUFFER_SIZE - 1);
    data_copy[BUFFER_SIZE - 1] = '\0';

    // Strip off leading '[' and trailing ']'
    char *start = strchr(data_copy, '[');
    char *end = strrchr(data_copy, ']');
    if (start && end) {
        *end = '\0';
        start++;
    } else {
        // If no brackets found, just start at the beginning
        start = data_copy;
    }

    char *entry = start;
    while (entry && *entry != '\0') {
        // Find the next "}, " delimiter
        char *next = strstr(entry, "}, ");
        if (next != NULL) {
            *next = '\0';   // terminate current entry
            next += 3;       // move past "}, "
        } else {
            // If "}, " not found, we might be at the last user
            char *end_brace = strchr(entry, '}');
            if (end_brace) {
                *end_brace = '\0';
                next = NULL;
            } else {
                // No proper ending brace - malformed data
                next = NULL;
            }
        }

        // Remove leading '{' if present
        if (entry[0] == '{') {
            memmove(entry, entry + 1, strlen(entry));
        }

        // Now entry should look like: "1, hoangquan, Hoang Quan, 1"
        // Split by commas to get the four fields
        char *user_id_str = strtok(entry, ",");
        char *username = strtok(NULL, ",");
        char *full_name = strtok(NULL, ",");
        char *role_str = strtok(NULL, ",");

        if (!user_id_str || !username || !full_name || !role_str) {
            // Malformed user entry, skip it
            entry = next;
            continue;
        }

        // Trim whitespace from each field
        trim_whitespace(user_id_str);
        trim_whitespace(username);
        trim_whitespace(full_name);
        trim_whitespace(role_str);

        // Populate the UserInfo structure
        users[*user_count].user_id = atoi(user_id_str);
        strncpy(users[*user_count].username, username, BUFFER_SIZE - 1);
        users[*user_count].username[BUFFER_SIZE - 1] = '\0';
        strncpy(users[*user_count].full_name, full_name, BUFFER_SIZE - 1);
        users[*user_count].full_name[BUFFER_SIZE - 1] = '\0';
        users[*user_count].role = atoi(role_str);

        (*user_count)++;

        // Move on to the next user entry
        entry = next;
    }
}