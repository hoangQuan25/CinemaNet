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

