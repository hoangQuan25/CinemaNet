// menu.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include "menu.h"
#include "protocol.h"
#include "utils.h"
#include "display.h"

#define BUFFER_SIZE 4096

void main_menu(int sock) {
    int choice;
    char token[BUFFER_SIZE] = {0};
    int is_logged_in = 0; // 0: Not logged in, 1: Logged in

    while (1) {
        if (!is_logged_in) {
            // User is not logged in
            printf("\n===== CinemaNet =====\n");
            printf("1. Register\n");
            printf("2. Login\n");
            printf("3. Exit\n");
            printf("Enter your choice: ");
            scanf("%d", &choice);
            getchar(); // Consume newline

            switch (choice) {
                case 1:
                    register_user(sock);
                    break;
                case 2:
                    if (login_user(sock, token)) {
                        is_logged_in = 1;
                    }
                    break;
                case 3:
                    printf("Exiting...\n");
                    return;
                default:
                    printf("Invalid choice. Please try again.\n");
            }
        } else {
            // User is logged in
            printf("\n===== CinemaNet =====\n");
            printf("1. Search Films by Title\n");
            printf("2. Browse Films\n");
            printf("3. Change Password\n");
            printf("4. Logout\n");
            printf("Enter your choice: ");
            scanf("%d", &choice);
            getchar(); // Consume newline

            switch (choice) {
                case 1:
                    search_films_by_title(sock, token);
                    break;
                case 2:
                    browse_films(sock, token);
                    break;
                case 3:
                    change_password(sock, token);
                    break;
                case 4:
                    if (logout_user(sock, token)) {
                        is_logged_in = 0;
                        memset(token, 0, BUFFER_SIZE);
                    }
                    break;
                default:
                    printf("Invalid choice. Please try again.\n");
            }
        }
    }
}


void register_user(int sock) {
    char name[BUFFER_SIZE];
    char username[BUFFER_SIZE];
    char password[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    char server_reply[BUFFER_SIZE];

    printf("Enter your full name: ");
    fgets(name, BUFFER_SIZE, stdin);
    trim_newline(name);

    printf("Enter username: ");
    fgets(username, BUFFER_SIZE, stdin);
    trim_newline(username);

    printf("Enter password: ");
    fgets(password, BUFFER_SIZE, stdin);
    trim_newline(password);

    // Construct message
    sprintf(message, "REGISTER\r\n%s\r\n%s\r\n%s", name, username, password);

    // Send message
    send(sock, message, strlen(message), 0);

    // Receive server response
    int read_size = recv(sock, server_reply, BUFFER_SIZE, 0);
    server_reply[read_size] = '\0';

    // Process server response
    if (strcmp(server_reply, "1020\r\n") == 0) {
        printf("Registration successful.\n");
    } else if (strcmp(server_reply, "2021\r\n") == 0) {
        printf("Registration failed: Username already exists.\n");
    } else {
        printf("An error occurred during registration.\n");
    }
}

int login_user(int sock, char *token) {
    char username[BUFFER_SIZE];
    char password[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    char server_reply[BUFFER_SIZE];

    printf("Enter username: ");
    fgets(username, BUFFER_SIZE, stdin);
    trim_newline(username);

    printf("Enter password: ");
    fgets(password, BUFFER_SIZE, stdin);
    trim_newline(password);

    // Construct message
    sprintf(message, "LOGIN\r\n%s\r\n%s", username, password);

    // Send message
    send(sock, message, strlen(message), 0);

    // Receive server response
    int read_size = recv(sock, server_reply, BUFFER_SIZE, 0);
    server_reply[read_size] = '\0';

    // Process server response
    char *code = strtok(server_reply, "\r\n");
    if (strcmp(code, "1010") == 0 || strcmp(code, "1011") == 0) {
        char *received_token = strtok(NULL, "\r\n");
        strcpy(token, received_token);
        printf("Login successful.\n");
        return 1; // Success
    } else if (strcmp(code, "2011") == 0) {
        printf("Login failed: Invalid credentials.\n");
        return 0; // Failure
    } else {
        printf("An error occurred during login.\n");
        return 0; // Failure
    }
}

void change_password(int sock, const char *token) {
    char username[BUFFER_SIZE];
    char old_password[BUFFER_SIZE];
    char new_password[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    char server_reply[BUFFER_SIZE];

    printf("Enter username: ");
    fgets(username, BUFFER_SIZE, stdin);
    trim_newline(username);

    printf("Enter old password: ");
    fgets(old_password, BUFFER_SIZE, stdin);
    trim_newline(old_password);

    printf("Enter new password: ");
    fgets(new_password, BUFFER_SIZE, stdin);
    trim_newline(new_password);

    // Construct message
    sprintf(message, "CHANGE_PASSWORD\r\n%s\r\n%s\r\n%s\r\n%s", username, old_password, new_password, token);

    // Send message
    send(sock, message, strlen(message), 0);

    // Receive server response
    int read_size = recv(sock, server_reply, BUFFER_SIZE, 0);
    server_reply[read_size] = '\0';

    // Process server response
    if (strcmp(server_reply, "1110\r\n") == 0) {
        printf("Password changed successfully.\n");
    } else if (strcmp(server_reply, "2110\r\n") == 0) {
        printf("Password change failed: Incorrect old password or invalid token.\n");
    } else {
        printf("An error occurred during password change.\n");
    }
}

void search_films_by_title(int sock, const char *token) {
    char title[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    char server_reply[BUFFER_SIZE];

    printf("Enter film title to search: ");
    fgets(title, BUFFER_SIZE, stdin);
    trim_newline(title);

    // Construct message
    sprintf(message, "SEARCH_BY_TITLE\r\n%s\r\n%s", title, token);

    // Send message
    send(sock, message, strlen(message), 0);

    // Receive server response
    int read_size = recv(sock, server_reply, BUFFER_SIZE, 0);
    server_reply[read_size] = '\0';

    // Process server response
    char *code = strtok(server_reply, "\r\n");
    if (strcmp(code, "2000") == 0) {
        char *data = strtok(NULL, "\r\n");
        printf("Films found:\n");
        display_films(data);
    } else if (strcmp(code, "2040") == 0) {
        printf("No films found with the given title.\n");
    } else {
        printf("An error occurred during film search.\n");
    }
}

void browse_films(int sock, const char *token) {
    char message[BUFFER_SIZE];
    char server_reply[BUFFER_SIZE];
    char category_id[BUFFER_SIZE];
    char cinema_id[BUFFER_SIZE];
    char start_time[BUFFER_SIZE];
    char end_time[BUFFER_SIZE];

    // Step 1: Get categories
    sprintf(message, "SHOW_CATEGORIES\r\n%s", token);
    send(sock, message, strlen(message), 0);

    int read_size = recv(sock, server_reply, BUFFER_SIZE, 0);
    server_reply[read_size] = '\0';

    char *code = strtok(server_reply, "\r\n");
    if (strcmp(code, "2000") == 0) {
        // After receiving categories
        char *categories = strtok(NULL, "\r\n");
        printf("Available Categories:\n");
        display_categories(categories);
    } else {
        printf("An error occurred while fetching categories.\n");
        return;
    }

    // Get category selection
    printf("Enter category ID: ");
    fgets(category_id, BUFFER_SIZE, stdin);
    trim_newline(category_id);

    // Step 2: Get cinemas
    sprintf(message, "SHOW_CINEMA\r\n%s", token);
    send(sock, message, strlen(message), 0);

    read_size = recv(sock, server_reply, BUFFER_SIZE, 0);
    server_reply[read_size] = '\0';

    code = strtok(server_reply, "\r\n");
    if (strcmp(code, "2000") == 0) {
        // After receiving cinemas
        char *cinemas = strtok(NULL, "\r\n");
        printf("Available Cinemas:\n");
        display_cinemas(cinemas);
    } else {
        printf("An error occurred while fetching cinemas.\n");
        return;
    }

    // Get cinema selection
    printf("Enter cinema ID: ");
    fgets(cinema_id, BUFFER_SIZE, stdin);
    trim_newline(cinema_id);

    // Get time frame
    printf("Enter start time (hh:mm): ");
    fgets(start_time, BUFFER_SIZE, stdin);
    trim_newline(start_time);

    printf("Enter end time (hh:mm): ");
    fgets(end_time, BUFFER_SIZE, stdin);
    trim_newline(end_time);

    // Step 3: Browse films
    sprintf(message, "BROWSE_FILM\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s", category_id, cinema_id, start_time, end_time, token);
    send(sock, message, strlen(message), 0);

    read_size = recv(sock, server_reply, BUFFER_SIZE, 0);
    server_reply[read_size] = '\0';

    code = strtok(server_reply, "\r\n");
    if (strcmp(code, "2000") == 0) {
        // After receiving films
        char *films = strtok(NULL, "\r\n");
        printf("Available Films:\n");
        display_films(films);
    } else if (strcmp(code, "2040") == 0) {
        printf("No films found for the given criteria.\n");
    } else {
        printf("An error occurred while browsing films.\n");
    }
}


int logout_user(int sock, char *token) {
    char message[BUFFER_SIZE];
    char server_reply[BUFFER_SIZE];

    sprintf(message, "LOGOUT");

    send(sock, message, strlen(message), 0);

    int read_size = recv(sock, server_reply, BUFFER_SIZE, 0);
    server_reply[read_size] = '\0';

    if (strcmp(server_reply, "1030\r\n") == 0) {
        printf("Logged out successfully.\n");
        return 1; // Success
    } else {
        printf("An error occurred during logout.\n");
        return 0; // Failure
    }
}

