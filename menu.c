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
    char username[BUFFER_SIZE] = {0};
    int is_logged_in = 0; // 0: Not logged in, 1: Logged in
    int user_role = 0;    // 0: Normal user, 1: Admin/Seller

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
                    if (login_user(sock, token, username, &user_role)) {
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
            if (user_role == 0) {
                // Normal user menu
                printf("\n===== CinemaNet =====\n");
                printf("1. Search Films by Title\n");
                printf("2. Browse Films\n");
                printf("3. Book ticket\n");
                printf("4. Change password\n");
                printf("5. Logout\n");
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
                        book_ticket(sock, username, token);
                        break;
                    case 4:
                        change_password(sock, token);
                        break;
                    case 5:
                        if (logout_user(sock, token)) {
                            is_logged_in = 0;
                            memset(token, 0, BUFFER_SIZE);
                            memset(username, 0, BUFFER_SIZE);
                            user_role = 0;
                        }
                        break;
                    default:
                        printf("Invalid choice. Please try again.\n");
                }
            } else if (user_role == 1) {
                // Admin/Seller menu
                printf("\n===== CinemaNet (Admin) =====\n");
                printf("1. Add New Film\n");
                printf("2. Show Film\n");
                printf("3. Edit Show\n");
                printf("4. Logout\n");
                printf("Enter your choice: ");
                scanf("%d", &choice);
                getchar(); // Consume newline

                switch (choice) {
                    case 1:
                        add_new_film(sock, token);
                        break;
                    case 2:
                        show_film(sock, token);
                        break;
                    case 3:
                        // edit_show(sock, token);
                        break;
                    case 4:
                        if (logout_user(sock, token)) {
                            is_logged_in = 0;
                            memset(token, 0, BUFFER_SIZE);
                            memset(username, 0, BUFFER_SIZE);
                            user_role = 0;
                        }
                        break;
                    default:
                        printf("Invalid choice. Please try again.\n");
                }
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

    // // Print the message being sent
    // printf("\n=== Message Sent to Server ===\n%s\n", message);

    // Send message
    send(sock, message, strlen(message), 0);

    // Receive server response
    int read_size = recv(sock, server_reply, BUFFER_SIZE, 0);
    server_reply[read_size] = '\0';

    // // Print the message received from server
    // printf("\n=== Message Received from Server ===\n%s\n", server_reply);

    // Process server response
    if (strcmp(server_reply, "1020\r\n") == 0) {
        printf("Registration successful.\n");
    } else if (strcmp(server_reply, "2021\r\n") == 0) {
        printf("Registration failed: Username already exists.\n");
    } else {
        printf("An error occurred during registration.\n");
    }
}

int login_user(int sock, char *token, char *username_out, int *user_role) {
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

    // // Print the message being sent
    // printf("\n=== Message Sent to Server ===\n%s\n", message);

    // Send message
    send(sock, message, strlen(message), 0);

    // Receive server response
    int read_size = recv(sock, server_reply, BUFFER_SIZE, 0);
    server_reply[read_size] = '\0';

    // // Print the message received from server
    // printf("\n=== Message Received from Server ===\n%s\n", server_reply);

    // Process server response
    char *code = strtok(server_reply, "\r\n");
    if (strcmp(code, "1010") == 0 || strcmp(code, "1011") == 0) {
        char *received_token = strtok(NULL, "\r\n");
        strcpy(token, received_token);
        printf("Login successful.\n");
        // After successful login
        strcpy(username_out, username);
        // Set user role based on code
        if (strcmp(code, "1010") == 0) {
            *user_role = 0; // Normal user
        } else if (strcmp(code, "1011") == 0) {
            *user_role = 1; // Admin/Seller
        }
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

    // // Print the message being sent
    // printf("\n=== Message Sent to Server ===\n%s\n", message);

    // Send message
    send(sock, message, strlen(message), 0);

    // Receive server response
    int read_size = recv(sock, server_reply, BUFFER_SIZE, 0);
    server_reply[read_size] = '\0';

    // // Print the message received from server
    // printf("\n=== Message Received from Server ===\n%s\n", server_reply);

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

    // // Print the message being sent
    // printf("\n=== Message Sent to Server ===\n%s\n", message);

    // Send message
    send(sock, message, strlen(message), 0);

    // Receive server response
    int read_size = recv(sock, server_reply, BUFFER_SIZE, 0);
    server_reply[read_size] = '\0';

    // // Print the message received from server
    // printf("\n=== Message Received from Server ===\n%s\n", server_reply);

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

    // // Print the message being sent
    // printf("\n=== Message Sent to Server ===\n%s\n", message);

    send(sock, message, strlen(message), 0);

    int read_size = recv(sock, server_reply, BUFFER_SIZE, 0);
    server_reply[read_size] = '\0';

    // // Print the message received from server
    // printf("\n=== Message Received from Server ===\n%s\n", server_reply);

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

    // // Print the message being sent
    // printf("\n=== Message Sent to Server ===\n%s\n", message);

    send(sock, message, strlen(message), 0);

    read_size = recv(sock, server_reply, BUFFER_SIZE, 0);
    server_reply[read_size] = '\0';

    // // Print the message received from server
    // printf("\n=== Message Received from Server ===\n%s\n", server_reply);

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

    // // Print the message being sent
    // printf("\n=== Message Sent to Server ===\n%s\n", message);

    send(sock, message, strlen(message), 0);

    read_size = recv(sock, server_reply, BUFFER_SIZE, 0);
    server_reply[read_size] = '\0';

    // // Print the message received from server
    // printf("\n=== Message Received from Server ===\n%s\n", server_reply);

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

void book_ticket(int sock, const char *username, const char *token) {
    // Variables
    char title[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    char server_reply[BUFFER_SIZE];

    // Step 1: Search for films
    printf("Enter film title to search: ");
    fgets(title, BUFFER_SIZE, stdin);
    trim_newline(title);

    // Construct message
    sprintf(message, "SEARCH_BY_TITLE\r\n%s\r\n%s", title, token);

    // Send message
    send(sock, message, strlen(message), 0);

    // Receive server response
    int read_size = recv(sock, server_reply, BUFFER_SIZE - 1, 0);
    server_reply[read_size] = '\0';

    // Process server response
    char *code = strtok(server_reply, "\r\n");
    if (strcmp(code, "2000") == 0) {
        char *data = strtok(NULL, "\r\n");
        printf("Films found:\n");
        display_films(data);

        // User selects a film
        char film_id[BUFFER_SIZE];
        printf("Enter the Film ID to select: ");
        fgets(film_id, BUFFER_SIZE, stdin);
        trim_newline(film_id);

        // Step 2: Display cinemas showing the selected film
        sprintf(message, "SHOW_CINEMA_BY_FILM\r\n%s\r\n%s", film_id, token);
        send(sock, message, strlen(message), 0);

        // Receive server response
        read_size = recv(sock, server_reply, BUFFER_SIZE - 1, 0);
        server_reply[read_size] = '\0';

        // Process server response
        code = strtok(server_reply, "\r\n");
        if (strcmp(code, "2000") == 0) {
            char *data = strtok(NULL, "\r\n");
            printf("Cinemas showing the selected film:\n");
            display_cinemas(data);

            // User selects a cinema
            char cinema_id[BUFFER_SIZE];
            printf("Enter the Cinema ID to select: ");
            fgets(cinema_id, BUFFER_SIZE, stdin);
            trim_newline(cinema_id);

            // Step 3: Display available shows
            sprintf(message, "SHOW_SHOWS\r\n%s\r\n%s\r\n%s", film_id, cinema_id, token);
            send(sock, message, strlen(message), 0);

            // Receive server response
            read_size = recv(sock, server_reply, BUFFER_SIZE - 1, 0);
            server_reply[read_size] = '\0';

            // Process server response
            code = strtok(server_reply, "\r\n");
            if (strcmp(code, "2000") == 0) {
                char *data = strtok(NULL, "\r\n");
                printf("Available Shows:\n");
                display_shows(data);

                // User selects a show
                char show_id[BUFFER_SIZE];
                printf("Enter the Show ID to select: ");
                fgets(show_id, BUFFER_SIZE, stdin);
                trim_newline(show_id);

                // Step 4: Display seat map
                sprintf(message, "SHOW_SEAT_MAP\r\n%s\r\n%s", show_id, token);
                send(sock, message, strlen(message), 0);

                // Receive server response
                read_size = recv(sock, server_reply, BUFFER_SIZE - 1, 0);
                server_reply[read_size] = '\0';

                // Process server response
                code = strtok(server_reply, "\r\n");
                if (strcmp(code, "2000") == 0) {
                    char *data = strtok(NULL, "\r\n");
                    printf("Seat Map:\n");
                    display_seat_map(data);

                    // Step 5: User selects seats
                    int num_seats;
                    printf("Enter the number of seats you want to book: ");
                    scanf("%d", &num_seats);
                    getchar(); // Consume newline

                    char seat_list[BUFFER_SIZE] = "";
                    char seat_input[10];
                    for (int i = 0; i < num_seats; i++) {
                        printf("Enter seat %d (e.g., A1): ", i + 1);
                        fgets(seat_input, sizeof(seat_input), stdin);
                        trim_newline(seat_input);

                        // Convert seat_input to seat_id
                        int seat_id = seat_label_to_id(seat_input);
                        if (seat_id == -1) {
                            printf("Invalid seat: %s\n", seat_input);
                            i--;
                            continue;
                        }

                        // Append seat_id to seat_list
                        char seat_id_str[10];
                        sprintf(seat_id_str, "%d", seat_id);
                        strcat(seat_list, seat_id_str);
                        if (i < num_seats - 1) {
                            strcat(seat_list, ",");
                        }
                    }

                    // Step 6: Book ticket
                    sprintf(message, "BOOK_TICKET\r\n%s\r\n%s\r\n%s\r\n%s\r\n%d\r\n[%s]\r\n%s", username, film_id, cinema_id, show_id, num_seats, seat_list, token);
                    send(sock, message, strlen(message), 0);

                    // Receive server response
                    read_size = recv(sock, server_reply, BUFFER_SIZE - 1, 0);
                    server_reply[read_size] = '\0';

                    // Process server response
                    char *code = strtok(server_reply, "\r\n");
                    if (strcmp(code, "2000") == 0) {
                        // Extract information
                        char *username_resp = strtok(NULL, "\r\n");
                        char *film_name = strtok(NULL, "\r\n");
                        char *cinema_name = strtok(NULL, "\r\n");
                        char *show_info = strtok(NULL, "\r\n"); // [yyyy-mm-dd, hh:mm, hh:mm]
                        char *seat_number_str = strtok(NULL, "\r\n");
                        char *seat_list_resp = strtok(NULL, "\r\n"); // [seat_id1, seat_id2,...]

                        int seat_number = atoi(seat_number_str);

                        // Print ticket
                        print_ticket(username_resp, film_name, cinema_name, show_info, seat_number, seat_list_resp);
                    } else {
                        printf("An error occurred during booking.\n");
                    }

                } else {
                    printf("An error occurred while fetching seat map.\n");
                    return;
                }

            } else {
                printf("An error occurred while fetching shows.\n");
                return;
            }

        } else if (strcmp(code, "2050") == 0) {
            printf("No cinemas are showing this film.\n");
            return;
        } else {
            printf("An error occurred while fetching cinemas.\n");
            return;
        }

    } else if (strcmp(code, "2040") == 0) {
        printf("No films found with the given title.\n");
    } else {
        printf("An error occurred during film search.\n");
    }

    
}


int logout_user(int sock, char *token) {
    char message[BUFFER_SIZE];
    char server_reply[BUFFER_SIZE];

    sprintf(message, "LOGOUT");

    // // Print the message being sent
    // printf("\n=== Message Sent to Server ===\n%s\n", message);

    send(sock, message, strlen(message), 0);

    int read_size = recv(sock, server_reply, BUFFER_SIZE, 0);
    server_reply[read_size] = '\0';

    // // Print the message received from server
    // printf("\n=== Message Received from Server ===\n%s\n", server_reply);

    if (strcmp(server_reply, "1030\r\n") == 0) {
        printf("Logged out successfully.\n");
        return 1; // Success
    } else {
        printf("An error occurred during logout.\n");
        return 0; // Failure
    }
}

void print_ticket(const char *username, const char *film_name, const char *cinema_name,
                  const char *show_info, int seat_number, const char *seat_list) {
    // Parse show_info to extract date, start_time, end_time
    char show_info_copy[BUFFER_SIZE];
    strcpy(show_info_copy, show_info);
    char *date = strtok(show_info_copy, ",[]");
    char *start_time = strtok(NULL, ", ");
    char *end_time = strtok(NULL, ", ");

    // Convert seat IDs to labels
    char seat_list_labels[BUFFER_SIZE] = "";
    char seat_list_copy[BUFFER_SIZE];
    strcpy(seat_list_copy, seat_list);

    char *seat_id_str = strtok(seat_list_copy, ",[]");
    while (seat_id_str != NULL) {
        int seat_id = atoi(seat_id_str);
        char seat_label[10];
        seat_id_to_label(seat_id, seat_label);
        strcat(seat_list_labels, seat_label);
        seat_id_str = strtok(NULL, ",[]");
        if (seat_id_str != NULL) {
            strcat(seat_list_labels, ",");
        }
    }

    printf("\n===== CINEMANET TICKET =====\n");
    printf("Username: %s\n", username);
    printf("Film: %s\n", film_name);
    printf("Cinema: %s\n", cinema_name);
    printf("Show Time: %s, %s - %s\n", date, start_time, end_time);
    printf("Number of Seats: %d\n", seat_number);
    printf("Seats: %s\n", seat_list_labels);
    printf("Thank you for your purchase!\n");
    printf("============================\n");
}


// seller side
void add_new_film(int sock, const char *token) {
    char film_name[BUFFER_SIZE];
    char category_id[BUFFER_SIZE];
    char length_str[BUFFER_SIZE];
    char description[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    char server_reply[BUFFER_SIZE];
    char categories[BUFFER_SIZE];

    // Step 1: Seller inputs film name
    printf("Enter film name: ");
    fgets(film_name, BUFFER_SIZE, stdin);
    trim_newline(film_name);

    // Step 2: Display category list and ask for selection
    if (get_categories(sock, token, categories)) {
        printf("Available Categories:\n");
        display_categories(categories);

        // Ask the seller to select a category
        printf("Enter category ID: ");
        fgets(category_id, BUFFER_SIZE, stdin);
        trim_newline(category_id);
    } else {
        printf("An error occurred while fetching categories.\n");
        return;
    }

    // Step 3: Input description
    printf("Enter description: ");
    fgets(description, BUFFER_SIZE, stdin);
    trim_newline(description);

    // Step 4: Input length
    printf("Enter film length (in minutes): ");
    fgets(length_str, BUFFER_SIZE, stdin);
    trim_newline(length_str);

    // Step 5: Send data to server
    sprintf(message, "ADD_FILM\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s", film_name, category_id, description, length_str, token);

    // Send message
    send(sock, message, strlen(message), 0);

    // Receive server response
    int read_size = recv(sock, server_reply, BUFFER_SIZE, 0);
    server_reply[read_size] = '\0';

    // Process server response
    char *code = strtok(server_reply, "\r\n");
    if (strcmp(code, "2000") == 0) {
        printf("Film added successfully.\n");
    } else if (strcmp(code, "2060") == 0) {
        printf("Film already exists.\n");
    } else if (strcmp(code, "4001") == 0) {
        printf("Unauthorized access.\n");
    } else if (strcmp(code, "5000") == 0) {
        printf("Server error occurred.\n");
    } else {
        printf("An unexpected error occurred.\n");
    }
}

void show_film(int sock, const char *token) {
    char cinema_id[BUFFER_SIZE];
    char film_name[BUFFER_SIZE];
    char film_id[BUFFER_SIZE];
    char date[BUFFER_SIZE];
    char start_time[BUFFER_SIZE];
    char end_time[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    char server_reply[BUFFER_SIZE];
    char cinemas[BUFFER_SIZE];
    char film_details[BUFFER_SIZE];
    
    // Step 1: Display list of cinemas
    if (get_cinemas(sock, token, cinemas)) {
        printf("Available Cinemas:\n");
        display_cinemas(cinemas);
    } else {
        printf("An error occurred while fetching cinemas.\n");
        return;
    }
    
    // Seller selects a cinema
    printf("Enter Cinema ID: ");
    fgets(cinema_id, BUFFER_SIZE, stdin);
    trim_newline(cinema_id);

    // Step 2: Seller inputs film name
    printf("Enter Film Name to schedule: ");
    fgets(film_name, BUFFER_SIZE, stdin);
    trim_newline(film_name);
    
    // Construct CHECK_FILM message
    sprintf(message, "CHECK_FILM\r\n%s\r\n%s", film_name, token);
    
    // Send CHECK_FILM request
    send(sock, message, strlen(message), 0);
    
    // Receive server response
    int read_size = recv(sock, server_reply, BUFFER_SIZE, 0);
    server_reply[read_size] = '\0';
    
    // Process server response
    char *code = strtok(server_reply, "\r\n");
    if (strcmp(code, "2000") == 0) {
        // Film exists, extract film details
        strcpy(film_id, strtok(NULL, "\r\n"));
        char *film_name_resp = strtok(NULL, "\r\n");
        char *description = strtok(NULL, "\r\n");
        char *length = strtok(NULL, "\r\n");
        
        // Display film details to the seller
        printf("Film Details:\n");
        printf("Film ID: %s\n", film_id);
        printf("Film Name: %s\n", film_name_resp);
        printf("Description: %s\n", description);
        printf("Length: %s minutes\n", length);
    } else if (strcmp(code, "2040") == 0) {
        printf("Film not found.\n");
        return;
    } else {
        printf("An error occurred while checking film.\n");
        return;
    }

        // Step 3: Seller inputs date and time
    printf("Enter Date (yyyy-mm-dd): ");
    fgets(date, BUFFER_SIZE, stdin);
    trim_newline(date);
    
    printf("Enter Start Time (hh:mm): ");
    fgets(start_time, BUFFER_SIZE, stdin);
    trim_newline(start_time);
    
    printf("Enter End Time (hh:mm): ");
    fgets(end_time, BUFFER_SIZE, stdin);
    trim_newline(end_time);

    // Step 4: Send SHOW_FILM request to schedule the film
    sprintf(message, "SHOW_FILM\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s",
            film_id, cinema_id, date, start_time, end_time, token);
    
    // Send SHOW_FILM request
    send(sock, message, strlen(message), 0);
    
    // Receive server response
    read_size = recv(sock, server_reply, BUFFER_SIZE, 0);
    server_reply[read_size] = '\0';
    
    // Process server response
    code = strtok(server_reply, "\r\n");
    if (strcmp(code, "2000") == 0) {
        printf("Film scheduled successfully.\n");
    } else if (strcmp(code, "5000") == 0) {
        printf("An error occurred while scheduling the film.\n");
    } else {
        printf("An unexpected error occurred.\n");
    }
}



// helper function to fetch categories
bool get_categories(int sock, const char *token, char *categories) {
    char message[BUFFER_SIZE];
    char server_reply[BUFFER_SIZE];

    // Construct message
    sprintf(message, "SHOW_CATEGORIES\r\n%s", token);

    // Send message
    send(sock, message, strlen(message), 0);

    // Receive server response
    int read_size = recv(sock, server_reply, BUFFER_SIZE, 0);
    server_reply[read_size] = '\0';

    // Process server response
    char *code = strtok(server_reply, "\r\n");
    if (strcmp(code, "2000") == 0) {
        char *data = strtok(NULL, "\r\n");
        strcpy(categories, data);
        return true;
    } else {
        return false;
    }
}

// helper function to fetch cinemas
bool get_cinemas(int sock, const char *token, char *cinemas) {
    char message[BUFFER_SIZE];
    char server_reply[BUFFER_SIZE];

    // Construct message
    sprintf(message, "SHOW_CINEMA\r\n%s", token);

    // Send message
    send(sock, message, strlen(message), 0);

    // Receive server response
    int read_size = recv(sock, server_reply, BUFFER_SIZE, 0);
    server_reply[read_size] = '\0';

    // Process server response
    char *code = strtok(server_reply, "\r\n");
    if (strcmp(code, "2000") == 0) {
        char *data = strtok(NULL, "\r\n");
        strcpy(cinemas, data);
        return true;
    } else {
        return false;
    }
}





