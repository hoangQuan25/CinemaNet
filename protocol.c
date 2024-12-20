// protocol.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "protocol.h"
#include "database.h"
#include "utils.h"

#define BUFFER_SIZE 4096

bool process_client_message(const char *message, char *response)
{
    char *msg_copy = strdup(message);
    char *command = strtok(msg_copy, "\r\n");

    if (strcmp(command, "REGISTER") == 0)
    {
        char *name = strtok(NULL, "\r\n");
        char *username = strtok(NULL, "\r\n");
        char *password = strtok(NULL, "\r\n");

        if (register_user_db(name, username, password))
        {
            strcpy(response, "1020\r\n");
        }
        else
        {
            strcpy(response, "2021\r\n");
        }
    }
    else if (strcmp(command, "LOGIN") == 0)
    {
        char *username = strtok(NULL, "\r\n");
        char *password = strtok(NULL, "\r\n");

        char token[BUFFER_SIZE];
        int role;
        if (login_user_db(username, password, token, &role))
        {
            if (role == 1)
            {
                sprintf(response, "1010\r\n%s\r\n", token);
            }
            else if (role == 0)
            {
                sprintf(response, "1011\r\n%s\r\n", token);
            } else if (role == 2) {
                sprintf(response, "1012\r\n%s\r\n", token);
            }
        }
        else
        {
            strcpy(response, "2011\r\n");
        }
    }
    else if (strcmp(command, "CHANGE_PASSWORD") == 0)
    {
        char *username = strtok(NULL, "\r\n");
        char *old_password = strtok(NULL, "\r\n");
        char *new_password = strtok(NULL, "\r\n");
        char *token = strtok(NULL, "\r\n");

        if (change_password_db(username, old_password, new_password, token))
        {
            strcpy(response, "1110\r\n");
        }
        else
        {
            strcpy(response, "2110\r\n");
        }
    }
    else if (strcmp(command, "SEARCH_SHOWING_FILM_BY_TITLE") == 0)
    {
        char *title = strtok(NULL, "\r\n");
        char *token = strtok(NULL, "\r\n");

        char films[BUFFER_SIZE];
        if (search_showing_films_by_title_db(title, films))
        {
            sprintf(response, "2000\r\n%s\r\n", films);
        }
        else
        {
            strcpy(response, "2040\r\n");
        }
    }
    else if (strcmp(command, "SHOW_CATEGORIES") == 0)
    {
        char *token = strtok(NULL, "\r\n");

        char categories[BUFFER_SIZE];
        if (get_categories_db(categories))
        {
            sprintf(response, "2000\r\n%s\r\n", categories);
        }
        else
        {
            strcpy(response, "5000\r\n");
        }
    }
    else if (strcmp(command, "SHOW_CINEMA") == 0)
    {
        char *token = strtok(NULL, "\r\n");

        char cinemas[BUFFER_SIZE];
        if (get_cinemas_db(cinemas))
        {
            sprintf(response, "2000\r\n%s\r\n", cinemas);
        }
        else
        {
            strcpy(response, "5000\r\n");
        }
    }
    else if (strcmp(command, "BROWSE_FILM") == 0)
    {
        char *category_id = strtok(NULL, "\r\n");
        char *cinema_id = strtok(NULL, "\r\n");
        char *start_time = strtok(NULL, "\r\n");
        char *end_time = strtok(NULL, "\r\n");
        char *token = strtok(NULL, "\r\n");

        char films[BUFFER_SIZE];
        if (browse_films_db(category_id, cinema_id, start_time, end_time, films))
        {
            sprintf(response, "2000\r\n%s\r\n", films);
        }
        else
        {
            strcpy(response, "2040\r\n");
        }
    }
    else if (strcmp(command, "SHOW_CINEMA_BY_FILM") == 0)
    {
        // Extract parameters
        char *film_id = strtok(NULL, "\r\n");
        char *token = strtok(NULL, "\r\n");

        // Verify token (implement token verification)
        if (!verify_token(token))
        {
            strcpy(response, "4010\r\n"); // Unauthorized
            return false;
        }

        // Call the database function
        if (get_cinemas_by_film(film_id, response))
        {
            // Prepend success code
            char temp[BUFFER_SIZE];
            strcpy(temp, response);
            sprintf(response, "2000\r\n%s", temp);
        }
        else
        {
            strcpy(response, "2050\r\n");
        }
    }
    else if (strcmp(command, "SHOW_SHOWS") == 0)
    {
        // Similar handling
        char *film_id = strtok(NULL, "\r\n");
        char *cinema_id = strtok(NULL, "\r\n");
        char *token = strtok(NULL, "\r\n");

        // Verify token (implement token verification)
        if (!verify_token(token))
        {
            strcpy(response, "4010\r\n"); // Unauthorized
            return false;
        }

        if (get_shows(film_id, cinema_id, response))
        {
            // Prepend success code
            char temp[BUFFER_SIZE];
            strcpy(temp, response);
            sprintf(response, "2000\r\n%s", temp);
        }
        else
        {
            strcpy(response, "5000\r\n");
        }
    }
    else if (strcmp(command, "SHOW_SEAT_MAP") == 0)
    {
        // Similar handling
        char *show_id = strtok(NULL, "\r\n");
        char *token = strtok(NULL, "\r\n");

        // Verify token
        if (!verify_token(token))
        {
            strcpy(response, "4010\r\n"); // Unauthorized
            return false;
        }

        if (get_seat_map(show_id, response))
        {
            // Prepend success code
            char temp[BUFFER_SIZE];
            strcpy(temp, response);
            sprintf(response, "2000\r\n%s", temp);
        }
        else
        {
            strcpy(response, "5000\r\n");
        }
    }
    else if (strcmp(command, "BOOK_TICKET") == 0)
    {
        // Similar handling
        char *username = strtok(NULL, "\r\n");
        char *film_id = strtok(NULL, "\r\n");
        char *cinema_id = strtok(NULL, "\r\n");
        char *show_id = strtok(NULL, "\r\n");
        char *seat_number_str = strtok(NULL, "\r\n");
        char *seat_list_str = strtok(NULL, "\r\n");
        char *token = strtok(NULL, "\r\n");

        // Verify token
        if (!verify_token(token))
        {
            strcpy(response, "4010\r\n"); // Unauthorized
            return false;
        }

        int seat_number = atoi(seat_number_str);
        if (book_ticket_db(username, film_id, cinema_id, show_id, seat_number, seat_list_str, response))
        {
            // printf("\n=== Message Sent to Client ===\n%s\n", response);
        }
        else
        {
            strcpy(response, "5000\r\n");
        }
    }
    else if (strcmp(command, "ADD_FILM") == 0)
    {
        char *film_name = strtok(NULL, "\r\n");
        char *category_id = strtok(NULL, "\r\n");
        char *description = strtok(NULL, "\r\n");
        char *length_str = strtok(NULL, "\r\n");
        char *token = strtok(NULL, "\r\n");

        // Verify token and seller role
        if (!verify_token(token))
        {
            strcpy(response, "4001\r\n"); // Unauthorized
            free(msg_copy);
            return false;
        }

        int length = atoi(length_str);

        int result = add_film_db(film_name, category_id, description, length);
        if (result == 1)
        {
            strcpy(response, "2000\r\n");
        }
        else if (result == -1)
        {
            strcpy(response, "2060\r\n"); // Film already exists
        }
        else
        {
            strcpy(response, "5000\r\n");
        }
    }
    else if (strcmp(command, "SEARCH_FILM_BY_TITLE") == 0)
    {
        char *title = strtok(NULL, "\r\n");
        char *token = strtok(NULL, "\r\n");

        char films[BUFFER_SIZE];
        if (search_films_by_title_db(title, films))
        {
            sprintf(response, "2000\r\n%s\r\n", films);
        }
        else
        {
            strcpy(response, "2040\r\n");
        }
    }
    else if (strcmp(command, "SHOW_FILM") == 0)
    {
        char *film_id = strtok(NULL, "\r\n");
        char *cinema_id = strtok(NULL, "\r\n");
        char *date = strtok(NULL, "\r\n");
        char *start_time = strtok(NULL, "\r\n");
        char *end_time = strtok(NULL, "\r\n");
        char *token = strtok(NULL, "\r\n");

        // Verify token and seller role
        if (!verify_token(token))
        {
            strcpy(response, "4001\r\n"); // Unauthorized
            free(msg_copy);
            return false;
        }

        int result = add_show_db(film_id, cinema_id, date, start_time, end_time);
        if (result == 1)
        {
            strcpy(response, "2000\r\n");
        }
        else
        {
            strcpy(response, "2070\r\n"); // Add show failed
        }
    }
    else if (strcmp(command, "SHOW_FILM_BY_CINEMA") == 0)
    {
        char *cinema_id = strtok(NULL, "\r\n");
        char *token = strtok(NULL, "\r\n");

        // Verify token and seller role
        if (!verify_token(token))
        {
            strcpy(response, "4001\r\n"); // Unauthorized
            free(msg_copy);
            return false;
        }

        char films[BUFFER_SIZE];
        if (get_films_by_cinema_db(cinema_id, films))
        {
            sprintf(response, "2000\r\n%s\r\n", films);
        }
        else
        {
            strcpy(response, "2080\r\n");
        }
    }
    else if (strcmp(command, "SHOW_SHOWS") == 0)
    {
        char *film_id = strtok(NULL, "\r\n");
        char *cinema_id = strtok(NULL, "\r\n");
        char *token = strtok(NULL, "\r\n");

        // Verify token and seller role
        if (!verify_token(token))
        {
            strcpy(response, "4001\r\n"); // Unauthorized
            free(msg_copy);
            return false;
        }

        char shows[BUFFER_SIZE];
        if (get_shows_db(film_id, cinema_id, shows))
        {
            sprintf(response, "2000\r\n%s\r\n", shows);
        }
        else
        {
            strcpy(response, "2081\r\n");
        }
    }
    else if (strcmp(command, "ADD_SHOW") == 0)
    {
        char *cinema_id = strtok(NULL, "\r\n");
        char *film_id = strtok(NULL, "\r\n");
        char *date = strtok(NULL, "\r\n");
        char *start_time = strtok(NULL, "\r\n");
        char *end_time = strtok(NULL, "\r\n");
        char *token = strtok(NULL, "\r\n");

        // Verify token and seller role
        if (!verify_token(token))
        {
            strcpy(response, "4001\r\n");
            free(msg_copy);
            return false;
        }

        int result = add_show_db(film_id, cinema_id, date, start_time, end_time);
        if (result == 1)
        {
            strcpy(response, "2000\r\n");
        }
        else
        {
            strcpy(response, "2070\r\n");
        }
    }
    else if (strcmp(command, "DELETE_SHOW") == 0)
    {
        char *show_id = strtok(NULL, "\r\n");
        char *token = strtok(NULL, "\r\n");

        // Verify token and seller role
        if (!verify_token(token))
        {
            strcpy(response, "4001\r\n");
            free(msg_copy);
            return false;
        }

        if (delete_show_db(show_id))
        {
            strcpy(response, "2000\r\n");
        }
        else
        {
            strcpy(response, "5000\r\n");
        }
    }
    else if (strcmp(command, "EDIT_SHOW") == 0)
    {
        char *show_id = strtok(NULL, "\r\n");
        char *date = strtok(NULL, "\r\n");
        char *start_time = strtok(NULL, "\r\n");
        char *end_time = strtok(NULL, "\r\n");
        char *token = strtok(NULL, "\r\n");

        // Verify token and seller role
        if (!verify_token(token))
        {
            strcpy(response, "4001\r\n");
            free(msg_copy);
            return false;
        }

        int result = edit_show_db(show_id, date, start_time, end_time);
        if (result == 1)
        {
            strcpy(response, "2000\r\n");
        }
        else
        {
            strcpy(response, "2070\r\n");
        }
    } else if (strcmp(command, "SEARCH_USER") == 0) {
        char *username_keyword = strtok(NULL, "\r\n");
        char *token = strtok(NULL, "\r\n");
        char role_str[BUFFER_SIZE];
        if (!verify_token(token)) {
            strcpy(response, "4010\r\n");
            free(msg_copy);
            return false;
        }

        char users_list[BUFFER_SIZE];
        if (search_users_db(username_keyword, users_list)) {
            snprintf(response, BUFFER_SIZE, "2000\r\n%s\r\n", users_list);
        } else {
            strcpy(response, "5000\r\n");
        }
    }
    else if (strcmp(command, "ASSIGN_ROLE") == 0) {
        char *user_id_str = strtok(NULL, "\r\n");
        char *old_role_str = strtok(NULL, "\r\n");
        char *new_role_str = strtok(NULL, "\r\n");
        char *token = strtok(NULL, "\r\n");

        char role_str[BUFFER_SIZE];
        if (!verify_token(token)) {
            strcpy(response, "4010\r\n");
            free(msg_copy);
            return false;
        }

        int old_role = atoi(old_role_str);
        int new_role = atoi(new_role_str);

        // Validate roles
        // allowed: (old=0,seller -> new=1,user) or (old=1,user -> new=0,seller)
        if ((old_role == new_role) || (new_role != 0 && new_role != 1)) {
            strcpy(response, "4002\r\n");
            free(msg_copy);
            return false;
        }

        if (assign_role_db(user_id_str, new_role)) {
            strcpy(response, "2000\r\n");
        } else {
            strcpy(response, "5000\r\n");
        }
    }

    else if (strcmp(command, "LOGOUT") == 0)
    {
        strcpy(response, "1030\r\n");
    }
    else
    {
        // Unknown command
        strcpy(response, "5000\r\n");
    }

    free(msg_copy);
    return true;
}
