// menu.h
#ifndef MENU_H
#define MENU_H

void main_menu(int sock);
void register_user(int sock);
int login_user(int sock, char *token);
void change_password(int sock, const char *token);
void search_films_by_title(int sock, const char *token);
void browse_films(int sock, const char *token);
int logout_user(int sock, char *token);

#endif // MENU_H
