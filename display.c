// menu.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include "display.h"
#include "utils.h"

#define BUFFER_SIZE 4096

void display_films(const char *data) {
    char data_copy[BUFFER_SIZE];
    strcpy(data_copy, data);
    char *start = strchr(data_copy, '[');
    char *end = strrchr(data_copy, ']');
    if (start && end) {
        *end = '\0';
        start++;
    } else {
        start = data_copy; // If brackets are missing
    }

    int count = 1;
    char *film_entry = start;
    while (film_entry && *film_entry != '\0') {
        // Find the next occurrence of "}, "
        char *next = strstr(film_entry, "}, ");
        if (next != NULL) {
            *next = '\0'; // Null-terminate the current film entry
            next += 3; // Move past "}, "
        } else {
            // Check for ending '}'
            char *end_brace = strchr(film_entry, '}');
            if (end_brace != NULL) {
                *end_brace = '\0';
                next = NULL;
            } else {
                // No more entries
                next = NULL;
            }
        }

        // Remove leading '{' if present
        if (film_entry[0] == '{') {
            film_entry++;
        }

        // Now parse the film_entry
        // First, get the ID
        char *id_end = strchr(film_entry, ',');
        if (id_end == NULL) {
            // Malformed entry
            break;
        }
        *id_end = '\0';
        char *id_str = film_entry;
        trim_whitespace(id_str);

        // Get the rest of the entry (name and description)
        char *rest = id_end + 1;

        // Now, get the name and description
        // Find the next comma
        char *name_end = strchr(rest, ',');
        if (name_end == NULL) {
            // Malformed entry
            break;
        }
        *name_end = '\0';
        char *name = rest;
        trim_whitespace(name);

        // The rest is the description
        char *description = name_end + 1;
        trim_whitespace(description);

        // Display the film
        printf("%d. [ID: %s] %s\n", count, id_str, name);
        printf("   Description: %s\n\n", description);

        // Move to the next film entry
        film_entry = next;
        count++;
    }
}

void display_categories(const char *data) {
    char data_copy[BUFFER_SIZE];
    strcpy(data_copy, data);
    char *start = strchr(data_copy, '[');
    char *end = strrchr(data_copy, ']');
    if (start && end) {
        *end = '\0';
        start++;
    } else {
        start = data_copy;
    }

    int count = 1;
    char *entry = start;
    while (entry && *entry != '\0') {
        // Find the next occurrence of "}, "
        char *next = strstr(entry, "}, ");
        if (next != NULL) {
            *next = '\0'; // Null-terminate the current entry
            next += 3; // Move past "}, "
        } else {
            char *end_brace = strchr(entry, '}');
            if (end_brace != NULL) {
                *end_brace = '\0';
                next = NULL;
            } else {
                next = NULL;
            }
        }

        // Remove leading '{' if present
        if (entry[0] == '{') {
            entry++;
        }

        // Split the entry into ID and name
        char *id_end = strchr(entry, ',');
        if (id_end == NULL) {
            // Malformed entry
            break;
        }
        *id_end = '\0';
        char *id_str = entry;
        trim_whitespace(id_str);

        char *name = id_end + 1;
        trim_whitespace(name);

        // Display the category
        printf("%d. [ID: %s] %s\n", count, id_str, name);

        entry = next;
        count++;
    }
}

void display_cinemas(const char *data) {
    // Similar parsing logic as display_categories
    display_categories(data); // If the format is the same
}

void display_shows(const char *data) {
    char data_copy[BUFFER_SIZE];
    strcpy(data_copy, data);
    char *start = strchr(data_copy, '[');
    char *end = strrchr(data_copy, ']');
    if (start && end) {
        *end = '\0';
        start++;
    } else {
        start = data_copy;
    }

    int count = 1;
    char *entry = start;
    while (entry && *entry != '\0') {
        // Find the next occurrence of "}, "
        char *next = strstr(entry, "}, ");
        if (next != NULL) {
            *next = '\0'; // Null-terminate the current entry
            next += 3; // Move past "}, "
        } else {
            char *end_brace = strchr(entry, '}');
            if (end_brace != NULL) {
                *end_brace = '\0';
                next = NULL;
            } else {
                next = NULL;
            }
        }

        // Remove leading '{' if present
        if (entry[0] == '{') {
            entry++;
        }

        // Split the entry into components
        char *id_end = strchr(entry, ',');
        if (id_end == NULL) {
            break;
        }
        *id_end = '\0';
        char *show_id = entry;
        trim_whitespace(show_id);

        char *rest = id_end + 1;
        char *date_end = strchr(rest, ',');
        if (date_end == NULL) {
            break;
        }
        *date_end = '\0';
        char *date = rest;
        trim_whitespace(date);

        char *start_time_end = strchr(date_end + 1, ',');
        if (start_time_end == NULL) {
            break;
        }
        *start_time_end = '\0';
        char *start_time = date_end + 1;
        trim_whitespace(start_time);

        char *end_time = start_time_end + 1;
        trim_whitespace(end_time);

        // Display the show
        printf("%d. [Show ID: %s] Date: %s, Start: %s, End: %s\n", count, show_id, date, start_time, end_time);

        entry = next;
        count++;
    }
}

void display_seat_map(const char *seat_map_str) {
    int seats_per_row = 10;
    int row_number = 0;
    char row_label = 'A';
    int seat_in_row = 0;
    char seat_map[BUFFER_SIZE];
    strcpy(seat_map, seat_map_str);

    printf("   ");
    for (int i = 1; i <= seats_per_row; i++) {
        printf("%3d", i);
    }
    printf("\n");

    printf("%c ", row_label + row_number);

    for (int i = 0; seat_map[i] != '\0'; i++) {
        if (seat_map[i] == '5') {
            // Start new row
            row_number++;
            printf("\n");
            printf("%c ", row_label + row_number);
            seat_in_row = 0;
            continue;
        }

        // Display seat status
        if (seat_map[i] == '0') {
            printf("  -");
        } else if (seat_map[i] == '1') {
            printf("  x");
        } else {
            printf("  ?");
        }

        seat_in_row++;

        // If no row delimiter and seat_in_row reaches seats_per_row
        if (seat_in_row == seats_per_row && seat_map[i + 1] != '\0' && seat_map[i + 1] != '5') {
            row_number++;
            printf("\n");
            printf("%c ", row_label + row_number);
            seat_in_row = 0;
        }
    }
    printf("\n");
}

// display_films_with_length.c
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

#define BUFFER_SIZE 1024

void display_films_with_length(const char *data) {
    char data_copy[BUFFER_SIZE];
    snprintf(data_copy, BUFFER_SIZE, "%s", data); // Safe copy
    char *start = strchr(data_copy, '[');
    char *end = strrchr(data_copy, ']');

    if (start && end) {
        *end = '\0'; // Null-terminate after ']'
        start++;      // Move past '['
    } else {
        start = data_copy; // If brackets are missing
    }

    int count = 1;
    char *film_entry = start;

    while (film_entry && *film_entry != '\0') {
        // Find the next occurrence of "}, "
        char *next = strstr(film_entry, "}, ");
        if (next != NULL) {
            *next = '\0'; // Null-terminate current film entry
            next += 3;     // Move past "}, "
        } else {
            // Check for ending '}'
            char *end_brace = strchr(film_entry, '}');
            if (end_brace != NULL) {
                *end_brace = '\0';
                next = NULL;
            } else {
                // No more entries
                next = NULL;
            }
        }

        // Remove leading '{' if present
        if (film_entry[0] == '{') {
            film_entry++;
        }

        // Parse the film_entry
        // Expected format: "film_id, film_name, length"

        char *token = strtok(film_entry, ",");
        if (token == NULL) {
            break; // Malformed entry
        }
        char *id_str = token;
        trim_whitespace(id_str);

        token = strtok(NULL, ",");
        if (token == NULL) {
            break; // Malformed entry
        }
        char *name = token;
        trim_whitespace(name);

        token = strtok(NULL, ",");
        if (token == NULL) {
            break; // Malformed entry
        }
        char *length = token;
        trim_whitespace(length);

        // Display the film
        printf("%d. [ID: %s] %s\n", count, id_str, name);
        printf("   Length: %s minutes\n\n", length);

        // Move to the next film entry
        film_entry = next;
        count++;
    }

    if (count == 1) {
        printf("No films to display.\n");
    }
}





