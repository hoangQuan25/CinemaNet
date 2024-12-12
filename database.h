// database.h
#ifndef DATABASE_H
#define DATABASE_H

#include <stdbool.h>

bool init_database();
void close_database();

bool register_user_db(const char *name, const char *username, const char *password);
bool login_user_db(const char *username, const char *password, char *token, int *role);
bool change_password_db(const char *username, const char *old_password, const char *new_password, const char *token);

bool search_showing_films_by_title_db(const char *title, char *films);
bool search_films_by_title_db(const char *title, char *films);
bool get_categories_db(char *categories);
bool get_cinemas_db(char *cinemas);
bool browse_films_db(const char *category_id, const char *cinema_id, const char *start_time, const char *end_time, char *films);

bool get_cinemas_by_film(const char *film_id, char *cinemas);
bool get_shows(const char *film_id, const char *cinema_id, char *shows);
bool get_seat_map(const char *show_id, char *seat_map_str);
bool get_film_name(const char *film_id, char *film_name);
bool get_cinema_name(const char *cinema_id, char *cinema_name);
bool get_show_times(const char *show_id, char *show_date, char *start_time, char *end_time);
bool book_ticket_db(const char *username, const char *film_id, const char *cinema_id, const char *show_id, int seat_number, const char *seat_list_str, char *response);
int add_film_db(const char *film_name, const char *category_id, const char *description, int length);
int add_show_db(const char *film_id, const char *cinema_id, const char *date_str, const char *start_time_str, const char *end_time_str);
bool get_films_by_cinema_db(const char *cinema_id, char *films);
bool get_shows_db(const char *film_id, const char *cinema_id, char *shows);
bool delete_show_db(const char *show_id);
int edit_show_db(const char *show_id, const char *date_str, const char *start_time_str, const char *end_time_str);
bool search_users_db(const char *keyword, char *users_list);
bool assign_role_db(const char *user_id_str, int new_role);

#endif // DATABASE_H
