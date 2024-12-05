// menu.h
#ifndef MENU_H
#define MENU_H

#include <stdbool.h>

void main_menu(int sock);
void register_user(int sock);
int login_user(int sock, char *token, char *username_out, int *user_role);
void change_password(int sock, const char *token);
void search_films_by_title(int sock, const char *token);
void browse_films(int sock, const char *token);
int logout_user(int sock, char *token);
void print_ticket(const char *username, const char *film_name, const char *cinema_name,
                  const char *show_info, int seat_number, const char *seat_list);

void add_new_film(int sock, const char *token);
void show_film(int sock, const char *token);
bool get_categories(int sock, const char *token, char *categories);
bool get_cinemas(int sock, const char *token, char *cinemas);

#endif // MENU_H
