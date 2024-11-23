// database.h
#ifndef DATABASE_H
#define DATABASE_H

#include <stdbool.h>

bool init_database();
void close_database();

bool register_user_db(const char *name, const char *username, const char *password);
bool login_user_db(const char *username, const char *password, char *token, int *role);
bool change_password_db(const char *username, const char *old_password, const char *new_password, const char *token);

bool search_films_by_title_db(const char *title, char *films);
bool get_categories_db(char *categories);
bool get_cinemas_db(char *cinemas);
bool browse_films_db(const char *category_id, const char *cinema_id, const char *start_time, const char *end_time, char *films);

#endif // DATABASE_H
