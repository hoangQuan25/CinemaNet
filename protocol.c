// protocol.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "protocol.h"
#include "database.h"
#include "utils.h"

#define BUFFER_SIZE 4096

bool process_client_message(const char *message, char *response) {
    char *msg_copy = strdup(message);
    char *command = strtok(msg_copy, "\r\n");

    if (strcmp(command, "REGISTER") == 0) {
        char *name = strtok(NULL, "\r\n");
        char *username = strtok(NULL, "\r\n");
        char *password = strtok(NULL, "\r\n");

        if (register_user_db(name, username, password)) {
            strcpy(response, "1020\r\n");
        } else {
            strcpy(response, "2021\r\n");
        }
    } else if (strcmp(command, "LOGIN") == 0) {
        char *username = strtok(NULL, "\r\n");
        char *password = strtok(NULL, "\r\n");

        char token[BUFFER_SIZE];
        int role;
        if (login_user_db(username, password, token, &role)) {
            if (role == 1) {
                sprintf(response, "1010\r\n%s\r\n", token);
            } else {
                sprintf(response, "1011\r\n%s\r\n", token);
            }
        } else {
            strcpy(response, "2011\r\n");
        }
    } else if (strcmp(command, "CHANGE_PASSWORD") == 0) {
        char *username = strtok(NULL, "\r\n");
        char *old_password = strtok(NULL, "\r\n");
        char *new_password = strtok(NULL, "\r\n");
        char *token = strtok(NULL, "\r\n");

        if (change_password_db(username, old_password, new_password, token)) {
            strcpy(response, "1110\r\n");
        } else {
            strcpy(response, "2110\r\n");
        }
    } else if (strcmp(command, "SEARCH_BY_TITLE") == 0) {
        char *title = strtok(NULL, "\r\n");
        char *token = strtok(NULL, "\r\n");

        char films[BUFFER_SIZE];
        if (search_films_by_title_db(title, films)) {
            sprintf(response, "2000\r\n%s\r\n", films);
        } else {
            strcpy(response, "2040\r\n");
        }
    } else if (strcmp(command, "SHOW_CATEGORIES") == 0) {
        char *token = strtok(NULL, "\r\n");

        char categories[BUFFER_SIZE];
        if (get_categories_db(categories)) {
            sprintf(response, "2000\r\n%s\r\n", categories);
        } else {
            strcpy(response, "5000\r\n");
        }
    } else if (strcmp(command, "SHOW_CINEMA") == 0) {
        char *token = strtok(NULL, "\r\n");

        char cinemas[BUFFER_SIZE];
        if (get_cinemas_db(cinemas)) {
            sprintf(response, "2000\r\n%s\r\n", cinemas);
        } else {
            strcpy(response, "5000\r\n");
        }
    } else if (strcmp(command, "BROWSE_FILM") == 0) {
        char *category_id = strtok(NULL, "\r\n");
        char *cinema_id = strtok(NULL, "\r\n");
        char *start_time = strtok(NULL, "\r\n");
        char *end_time = strtok(NULL, "\r\n");
        char *token = strtok(NULL, "\r\n");

        char films[BUFFER_SIZE];
        if (browse_films_db(category_id, cinema_id, start_time, end_time, films)) {
            sprintf(response, "2000\r\n%s\r\n", films);
        } else {
            strcpy(response, "2040\r\n");
        }
    } else if (strcmp(command, "SHOW_CINEMA_BY_FILM") == 0) {
        // Extract parameters
        char *film_id = strtok(NULL, "\r\n");
        char *token = strtok(NULL, "\r\n");

        // Verify token (implement token verification)
        if (!verify_token(token)) {
            strcpy(response, "4010\r\n"); // Unauthorized
            return false;
        }

        // Call the database function
        if (get_cinemas_by_film(film_id, response)) {
            // Prepend success code
            char temp[BUFFER_SIZE];
            strcpy(temp, response);
            sprintf(response, "2000\r\n%s", temp);
        } else {
            strcpy(response, "2050\r\n");
        }
    } else if (strcmp(command, "SHOW_SHOWS") == 0) {
        // Similar handling
        char *film_id = strtok(NULL, "\r\n");
        char *cinema_id = strtok(NULL, "\r\n");
        char *token = strtok(NULL, "\r\n");

        // Verify token (implement token verification)
        if (!verify_token(token)) {
            strcpy(response, "4010\r\n"); // Unauthorized
            return false;
        }

        if (get_shows(film_id, cinema_id, response)) {
            // Prepend success code
            char temp[BUFFER_SIZE];
            strcpy(temp, response);
            sprintf(response, "2000\r\n%s", temp);
        } else {
            strcpy(response, "5000\r\n");
        }
    } else if (strcmp(command, "SHOW_SEAT_MAP") == 0) {
        // Similar handling
        char *show_id = strtok(NULL, "\r\n");
        char *token = strtok(NULL, "\r\n");

        // Verify token
        if (!verify_token(token)) {
            strcpy(response, "4010\r\n"); // Unauthorized
            return false;
        }

        if (get_seat_map(show_id, response)) {
            // Prepend success code
            char temp[BUFFER_SIZE];
            strcpy(temp, response);
            sprintf(response, "2000\r\n%s", temp);
        } else {
            strcpy(response, "5000\r\n");
        }
    } else if (strcmp(command, "BOOK_TICKET") == 0) {
        // Similar handling
        char *username = strtok(NULL, "\r\n");
        char *film_id = strtok(NULL, "\r\n");
        char *cinema_id = strtok(NULL, "\r\n");
        char *show_id = strtok(NULL, "\r\n");
        char *seat_number_str = strtok(NULL, "\r\n");
        char *seat_list_str = strtok(NULL, "\r\n");
        char *token = strtok(NULL, "\r\n");

        // Verify token
        if (!verify_token(token)) {
            strcpy(response, "4010\r\n"); // Unauthorized
            return false;
        }

        int seat_number = atoi(seat_number_str);
        if (book_ticket_db(username, film_id, cinema_id, show_id, seat_number, seat_list_str, response)) {
            // printf("\n=== Message Sent to Client ===\n%s\n", response);
        } else {
            strcpy(response, "5000\r\n");
        }
    } else if (strcmp(command, "LOGOUT") == 0) {
        strcpy(response, "1030\r\n");
    } else {
        // Unknown command
        strcpy(response, "5000\r\n");
    }

    free(msg_copy);
    return true;
}
