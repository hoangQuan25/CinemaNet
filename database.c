// database.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include <time.h>
#include "database.h"
#include "utils.h"

#define BUFFER_SIZE 4096

MYSQL *conn;

bool init_database() {
    conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "mysql_init() failed\n");
        return false;
    }

    // Replace with your database credentials
    if (mysql_real_connect(conn, "localhost", "root", "quan5691", "ticket_booking2", 0, NULL, 0) == NULL) {
        fprintf(stderr, "mysql_real_connect() failed\n");
        mysql_close(conn);
        return false;
    }

    return true;
}

void close_database() {
    mysql_close(conn);
}

bool register_user_db(const char *name, const char *username, const char *password) {
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE, "INSERT INTO users (full_name, username, password, role) VALUES ('%s', '%s', '%s', 1)",
             name, username, password);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Register User Error: %s\n", mysql_error(conn));
        return false;
    }

    return true;
}

bool login_user_db(const char *username, const char *password, char *token, int *role) {
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE, "SELECT id, role FROM users WHERE username='%s' AND password='%s'", username, password);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Login User Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Login User Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row) {
        // Generate token
        sprintf(token, "%s_%ld", username, time(NULL));
        *role = atoi(row[1]);
        mysql_free_result(result);
        return true;
    } else {
        mysql_free_result(result);
        return false;
    }
}

bool change_password_db(const char *username, const char *old_password, const char *new_password, const char *token) {
    // Verify token (For simplicity, we'll skip token validation here)

    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE, "UPDATE users SET password='%s' WHERE username='%s' AND password='%s'",
             new_password, username, old_password);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Change Password Error: %s\n", mysql_error(conn));
        return false;
    }

    if (mysql_affected_rows(conn) > 0) {
        return true;
    } else {
        return false;
    }
}

bool search_films_by_title_db(const char *title, char *films) {
    char query[BUFFER_SIZE];
    char escaped_title[2 * strlen(title) + 1]; // Buffer for escaped title

    // Escape the title to prevent SQL injection
    mysql_real_escape_string(conn, escaped_title, title, strlen(title));

    // Construct the query to select films with scheduled shows
    snprintf(query, BUFFER_SIZE,
        "SELECT DISTINCT films.id, films.film_name, films.description "
        "FROM films "
        "JOIN shows ON films.id = shows.film_id "
        "WHERE films.film_name LIKE '%%%s%%' "
        "AND shows.date >= CURDATE()",
        escaped_title);

    // Execute the query
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Search Films Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Search Films Error: %s\n", mysql_error(conn));
        return false;
    }

    int num_rows = mysql_num_rows(result);
    if (num_rows == 0) {
        mysql_free_result(result);
        return false;
    }

    MYSQL_ROW row;
    strcpy(films, "[");
    while ((row = mysql_fetch_row(result))) {
        strcat(films, "{");
        strcat(films, row[0]); // film_id
        strcat(films, ", ");
        strcat(films, row[1]); // film_name
        strcat(films, ", ");
        strcat(films, row[2]); // description
        strcat(films, "}, ");
    }
    strcat(films, "]");

    mysql_free_result(result);

    return true;
}

bool get_categories_db(char *categories) {
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE, "SELECT id, category_name FROM categories");

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Get Categories Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Get Categories Error: %s\n", mysql_error(conn));
        return false;
    }

    int num_rows = mysql_num_rows(result);
    if (num_rows == 0) {
        // No shows found
        mysql_free_result(result);
        return false; // Indicate no results found
    }

    MYSQL_ROW row;
    strcpy(categories, "[");
    while ((row = mysql_fetch_row(result))) {
        strcat(categories, "{");
        strcat(categories, row[0]); // category_id
        strcat(categories, ", ");
        strcat(categories, row[1]); // category_name
        strcat(categories, "}, ");
    }
    strcat(categories, "]");

    mysql_free_result(result);

    return true;
}

bool get_cinemas_db(char *cinemas) {
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE, "SELECT id, cinema_name FROM cinemas");

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Get Cinemas Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Get Cinemas Error: %s\n", mysql_error(conn));
        return false;
    }

    int num_rows = mysql_num_rows(result);
    if (num_rows == 0) {
        // No shows found
        mysql_free_result(result);
        return false; // Indicate no results found
    }

    MYSQL_ROW row;
    strcpy(cinemas, "[");
    while ((row = mysql_fetch_row(result))) {
        strcat(cinemas, "{");
        strcat(cinemas, row[0]); // cinema_id
        strcat(cinemas, ", ");
        strcat(cinemas, row[1]); // cinema_name
        strcat(cinemas, "}, ");
    }
    strcat(cinemas, "]");

    mysql_free_result(result);

    return true;
}

bool browse_films_db(const char *category_id, const char *cinema_id, const char *start_time, const char *end_time, char *films) {
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE,
             "SELECT DISTINCT f.id, f.film_name, f.description FROM films f "
             "JOIN shows s ON f.id = s.film_id "
             "WHERE f.category_id = %s AND s.cinema_id = %s AND s.start_time BETWEEN '%s' AND '%s'",
             category_id, cinema_id, start_time, end_time);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Browse Films Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Browse Films Error: %s\n", mysql_error(conn));
        return false;
    }

    int num_rows = mysql_num_rows(result);
    if (num_rows == 0) {
        // No shows found
        mysql_free_result(result);
        return false; // Indicate no results found
    }

    MYSQL_ROW row;
    strcpy(films, "[");
    while ((row = mysql_fetch_row(result))) {
        strcat(films, "{");
        strcat(films, row[0]); // film_id
        strcat(films, ", ");
        strcat(films, row[1]); // film_name
        strcat(films, ", ");
        strcat(films, row[2]); // description
        strcat(films, "}, ");
    }
    strcat(films, "]");

    mysql_free_result(result);

    return true;
}

bool get_cinemas_by_film(const char *film_id, char *cinemas) {
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE,
        "SELECT DISTINCT cinemas.id, cinemas.cinema_name "
        "FROM cinemas "
        "JOIN shows ON cinemas.id = shows.cinema_id "
        "WHERE shows.film_id = %s "
        "AND shows.date >= CURDATE()",
        film_id);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Get Cinemas Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Get Cinemas Error: %s\n", mysql_error(conn));
        return false;
    }

    int num_rows = mysql_num_rows(result);
    if (num_rows == 0) {
        // No shows found
        mysql_free_result(result);
        return false; // Indicate no results found
    }

    MYSQL_ROW row;
    strcpy(cinemas, "[");
    while ((row = mysql_fetch_row(result))) {
        strcat(cinemas, "{");
        strcat(cinemas, row[0]); // cinema_id
        strcat(cinemas, ", ");
        strcat(cinemas, row[1]); // cinema_name
        strcat(cinemas, "}, ");
    }
    strcat(cinemas, "]");

    mysql_free_result(result);

    return true;
}

bool get_shows(const char *film_id, const char *cinema_id, char *shows) {
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE,
        "SELECT id, date, start_time, end_time "
        "FROM shows "
        "WHERE film_id = %s AND cinema_id = %s AND date >= CURDATE()",
        film_id, cinema_id);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Get Shows Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Get Shows Error: %s\n", mysql_error(conn));
        return false;
    }

    int num_rows = mysql_num_rows(result);
    if (num_rows == 0) {
        // No shows found
        mysql_free_result(result);
        return false; // Indicate no results found
    }

    MYSQL_ROW row;
    strcpy(shows, "[");
    while ((row = mysql_fetch_row(result))) {
        strcat(shows, "{");
        strcat(shows, row[0]); // show_id
        strcat(shows, ", ");
        strcat(shows, row[1]); // date
        strcat(shows, ", ");
        strcat(shows, row[2]); // start_time
        strcat(shows, ", ");
        strcat(shows, row[3]); // end_time
        strcat(shows, "}, ");
    }
    strcat(shows, "]");

    mysql_free_result(result);

    return true;
}

bool get_seat_map(const char *show_id, char *seat_map_str) {
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE,
        "SELECT seat_map FROM shows WHERE id = %s",
        show_id);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Get Seat Map Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Get Seat Map Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row == NULL) {
        mysql_free_result(result);
        return false;
    }

    strcpy(seat_map_str, row[0]); // seat_map

    mysql_free_result(result);

    return true;
}

bool book_ticket_db(const char *username, const char *film_id, const char *cinema_id, const char *show_id, int seat_number, const char *seat_list_str, char *response) {
    MYSQL *conn_local = mysql_init(NULL);
    if (conn_local == NULL) {
        fprintf(stderr, "mysql_init() failed\n");
        return false;
    }

    // Initialize connection (use your actual database credentials)
    if (mysql_real_connect(conn_local, "localhost", "root", "quan5691", "ticket_booking2", 0, NULL, 0) == NULL) {
        fprintf(stderr, "mysql_real_connect() failed\n");
        mysql_close(conn_local);
        return false;
    }

    // Start transaction
    if (mysql_query(conn_local, "START TRANSACTION")) {
        fprintf(stderr, "START TRANSACTION Error: %s\n", mysql_error(conn_local));
        mysql_close(conn_local);
        return false;
    }

    // Lock the row for the specific show
    char lock_query[BUFFER_SIZE];
    snprintf(lock_query, BUFFER_SIZE, "SELECT seat_map FROM shows WHERE id = %s FOR UPDATE", show_id);

    if (mysql_query(conn_local, lock_query)) {
        fprintf(stderr, "Lock Error: %s\n", mysql_error(conn_local));
        mysql_query(conn_local, "ROLLBACK");
        mysql_close(conn_local);
        return false;
    }

    MYSQL_RES *result = mysql_store_result(conn_local);
    if (result == NULL) {
        fprintf(stderr, "Lock Error: %s\n", mysql_error(conn_local));
        mysql_query(conn_local, "ROLLBACK");
        mysql_close(conn_local);
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row == NULL) {
        mysql_free_result(result);
        mysql_query(conn_local, "ROLLBACK");
        mysql_close(conn_local);
        return false;
    }

    char seat_map[BUFFER_SIZE];
    strcpy(seat_map, row[0]); // Current seat map

    mysql_free_result(result);
    int seats_per_row = 10;

    // Process seat_list_str to get seat IDs
    char seat_list_copy[BUFFER_SIZE];
    strcpy(seat_list_copy, seat_list_str);
    char *seat_id_str = strtok(seat_list_copy, ",[]");
    int seat_ids[seat_number];
    int index = 0;

    while (seat_id_str != NULL && index < seat_number) {
        int seat_id = atoi(seat_id_str);
        seat_ids[index++] = seat_id;
        seat_id_str = strtok(NULL, ",[]");
    }

    if (index != seat_number) {
        fprintf(stderr, "Seat number mismatch\n");
        mysql_query(conn, "ROLLBACK");
        return false;
    }

    // Check seat availability and update seat map
    for (int i = 0; i < seat_number; i++) {
        int seat_id = seat_ids[i];
        int seat_index = seat_id_to_seat_map_index(seat_id, seats_per_row);

        if (seat_map[seat_index] == '1') {
            fprintf(stderr, "Seat %d is already booked\n", seat_id);
            mysql_query(conn, "ROLLBACK");
            return false;
        } else if (seat_map[seat_index] == '5') {
            fprintf(stderr, "Seat %d corresponds to a row separator\n", seat_id);
            mysql_query(conn, "ROLLBACK");
            return false;
        } else if (seat_map[seat_index] != '0') {
            fprintf(stderr, "Invalid seat status at index %d\n", seat_index);
            mysql_query(conn, "ROLLBACK");
            return false;
        }

        // Mark seat as booked
        seat_map[seat_index] = '1';
    }

    // Update the seat map in the database
    char update_query[BUFFER_SIZE];
    snprintf(update_query, BUFFER_SIZE,
        "UPDATE shows SET seat_map = '%s' WHERE id = %s",
        seat_map, show_id);

    if (mysql_query(conn_local, update_query)) {
        fprintf(stderr, "Update Seat Map Error: %s\n", mysql_error(conn_local));
        mysql_query(conn_local, "ROLLBACK");
        mysql_close(conn_local);
        return false;
    }

    // Insert ticket into tickets table
    char insert_query[BUFFER_SIZE];
    snprintf(insert_query, BUFFER_SIZE,
        "INSERT INTO tickets (user_id, film_id, cinema_id, show_id, seat_number, seat_list) "
        "VALUES ((SELECT id FROM users WHERE username = '%s'), %s, %s, %s, %d, '%s')",
        username, film_id, cinema_id, show_id, seat_number, seat_list_str);

    if (mysql_query(conn_local, insert_query)) {
        fprintf(stderr, "Insert Ticket Error: %s\n", mysql_error(conn_local));
        mysql_query(conn_local, "ROLLBACK");
        mysql_close(conn_local);
        return false;
    }

    // After successfully booking the ticket, fetch the additional information
    // Fetch film_name
    char film_name[BUFFER_SIZE];
    if (!get_film_name(film_id, film_name)) {
        mysql_query(conn, "ROLLBACK");
        return false;
    }

    // Fetch cinema_name
    char cinema_name[BUFFER_SIZE];
    if (!get_cinema_name(cinema_id, cinema_name)) {
        mysql_query(conn, "ROLLBACK");
        return false;
    }

    // Fetch show date and times
    char show_date[BUFFER_SIZE];
    char start_time[BUFFER_SIZE];
    char end_time[BUFFER_SIZE];
    if (!get_show_times(show_id, show_date, start_time, end_time)) {
        mysql_query(conn, "ROLLBACK");
        return false;
    }

    // Commit transaction
    if (mysql_query(conn_local, "COMMIT")) {
        fprintf(stderr, "COMMIT Error: %s\n", mysql_error(conn_local));
        mysql_query(conn_local, "ROLLBACK");
        mysql_close(conn_local);
        return false;
    }

    // Construct response
    sprintf(response, "2000\r\n%s\r\n%s\r\n%s\r\n[%s, %s, %s]\r\n%d\r\n%s",
            username, film_name, cinema_name, show_date, start_time, end_time,
            seat_number, seat_list_str);

    mysql_close(conn_local);
    return true;
}

//Helper functions for fetching infos of ticket after booking

bool get_film_name(const char *film_id, char *film_name) {
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE, "SELECT film_name FROM films WHERE id = %s", film_id);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Get Film Name Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Get Film Name Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row == NULL) {
        mysql_free_result(result);
        return false;
    }

    strcpy(film_name, row[0]);

    mysql_free_result(result);
    return true;
}

bool get_cinema_name(const char *cinema_id, char *cinema_name) {
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE, "SELECT cinema_name FROM cinemas WHERE id = %s", cinema_id);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Get Cinema Name Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Get Cinema Name Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row == NULL) {
        mysql_free_result(result);
        return false;
    }

    strcpy(cinema_name, row[0]);

    mysql_free_result(result);
    return true;
}

bool get_show_times(const char *show_id, char *show_date, char *start_time, char *end_time) {
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE, "SELECT date, start_time, end_time FROM shows WHERE id = %s", show_id);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Get Show Times Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Get Show Times Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row == NULL) {
        mysql_free_result(result);
        return false;
    }

    strcpy(show_date, row[0]);
    strcpy(start_time, row[1]);
    strcpy(end_time, row[2]);

    mysql_free_result(result);
    return true;
}
