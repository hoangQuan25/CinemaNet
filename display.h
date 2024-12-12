// display.h
#ifndef DISPLAY_H
#define DISPLAY_H

#include "user.h"

void display_films(const char *data);
void display_categories(const char *data);
void display_cinemas(const char *data);
void display_shows(const char *data);
void display_seat_map(const char *seat_map_str);
void display_films_with_length(const char *data);
void display_users(UserInfo users[], int user_count);

#endif // DISPLAY.H
