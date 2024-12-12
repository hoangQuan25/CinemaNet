// utils.h
#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include "user.h"

void trim_newline(char *str);
void trim_spaces(char *str);
void trim_whitespace(char *str);
int seat_label_to_id(const char *seat_label);
bool verify_token(const char *token);
int seat_id_to_seat_map_index(int seat_id, int seats_per_row);
void seat_id_to_label(int seat_id, char *seat_label);
void parse_users(const char *data, UserInfo users[], int *user_count);

#endif // UTILS_H
