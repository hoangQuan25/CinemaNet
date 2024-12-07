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

bool init_database()
{
    conn = mysql_init(NULL);
    if (conn == NULL)
    {
        fprintf(stderr, "mysql_init() failed\n");
        return false;
    }

    // Replace with your database credentials
    if (mysql_real_connect(conn, "localhost", "root", "quan5691", "ticket_booking2", 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "mysql_real_connect() failed\n");
        mysql_close(conn);
        return false;
    }

    return true;
}

void close_database()
{
    mysql_close(conn);
}

bool register_user_db(const char *name, const char *username, const char *password)
{
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE, "INSERT INTO users (full_name, username, password, role) VALUES ('%s', '%s', '%s', 1)",
             name, username, password);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Register User Error: %s\n", mysql_error(conn));
        return false;
    }

    return true;
}

bool login_user_db(const char *username, const char *password, char *token, int *role)
{
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE, "SELECT id, role FROM users WHERE username='%s' AND password='%s'", username, password);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Login User Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL)
    {
        fprintf(stderr, "Login User Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row)
    {
        // Generate token
        sprintf(token, "%s_%ld", username, time(NULL));
        *role = atoi(row[1]);
        mysql_free_result(result);
        return true;
    }
    else
    {
        mysql_free_result(result);
        return false;
    }
}

bool change_password_db(const char *username, const char *old_password, const char *new_password, const char *token)
{
    // Verify token (For simplicity, we'll skip token validation here)

    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE, "UPDATE users SET password='%s' WHERE username='%s' AND password='%s'",
             new_password, username, old_password);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Change Password Error: %s\n", mysql_error(conn));
        return false;
    }

    if (mysql_affected_rows(conn) > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool search_films_by_title_db(const char *title, char *films)
{
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
    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Search Films Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL)
    {
        fprintf(stderr, "Search Films Error: %s\n", mysql_error(conn));
        return false;
    }

    int num_rows = mysql_num_rows(result);
    if (num_rows == 0)
    {
        mysql_free_result(result);
        return false;
    }

    MYSQL_ROW row;
    strcpy(films, "[");
    while ((row = mysql_fetch_row(result)))
    {
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

bool get_categories_db(char *categories)
{
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE, "SELECT id, category_name FROM categories");

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Get Categories Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL)
    {
        fprintf(stderr, "Get Categories Error: %s\n", mysql_error(conn));
        return false;
    }

    int num_rows = mysql_num_rows(result);
    if (num_rows == 0)
    {
        // No shows found
        mysql_free_result(result);
        return false; // Indicate no results found
    }

    MYSQL_ROW row;
    strcpy(categories, "[");
    while ((row = mysql_fetch_row(result)))
    {
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

bool get_cinemas_db(char *cinemas)
{
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE, "SELECT id, cinema_name FROM cinemas");

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Get Cinemas Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL)
    {
        fprintf(stderr, "Get Cinemas Error: %s\n", mysql_error(conn));
        return false;
    }

    int num_rows = mysql_num_rows(result);
    if (num_rows == 0)
    {
        // No shows found
        mysql_free_result(result);
        return false; // Indicate no results found
    }

    MYSQL_ROW row;
    strcpy(cinemas, "[");
    while ((row = mysql_fetch_row(result)))
    {
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

bool browse_films_db(const char *category_id, const char *cinema_id, const char *start_time, const char *end_time, char *films)
{
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE,
             "SELECT DISTINCT f.id, f.film_name, f.description FROM films f "
             "JOIN shows s ON f.id = s.film_id "
             "WHERE f.category_id = %s AND s.cinema_id = %s AND s.start_time BETWEEN '%s' AND '%s'",
             category_id, cinema_id, start_time, end_time);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Browse Films Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL)
    {
        fprintf(stderr, "Browse Films Error: %s\n", mysql_error(conn));
        return false;
    }

    int num_rows = mysql_num_rows(result);
    if (num_rows == 0)
    {
        // No shows found
        mysql_free_result(result);
        return false; // Indicate no results found
    }

    MYSQL_ROW row;
    strcpy(films, "[");
    while ((row = mysql_fetch_row(result)))
    {
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

bool get_cinemas_by_film(const char *film_id, char *cinemas)
{
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE,
             "SELECT DISTINCT cinemas.id, cinemas.cinema_name "
             "FROM cinemas "
             "JOIN shows ON cinemas.id = shows.cinema_id "
             "WHERE shows.film_id = %s "
             "AND shows.date >= CURDATE()",
             film_id);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Get Cinemas Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL)
    {
        fprintf(stderr, "Get Cinemas Error: %s\n", mysql_error(conn));
        return false;
    }

    int num_rows = mysql_num_rows(result);
    if (num_rows == 0)
    {
        // No shows found
        mysql_free_result(result);
        return false; // Indicate no results found
    }

    MYSQL_ROW row;
    strcpy(cinemas, "[");
    while ((row = mysql_fetch_row(result)))
    {
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

bool get_shows(const char *film_id, const char *cinema_id, char *shows)
{
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE,
             "SELECT id, date, start_time, end_time "
             "FROM shows "
             "WHERE film_id = %s AND cinema_id = %s AND date >= CURDATE()",
             film_id, cinema_id);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Get Shows Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL)
    {
        fprintf(stderr, "Get Shows Error: %s\n", mysql_error(conn));
        return false;
    }

    int num_rows = mysql_num_rows(result);
    if (num_rows == 0)
    {
        // No shows found
        mysql_free_result(result);
        return false; // Indicate no results found
    }

    MYSQL_ROW row;
    strcpy(shows, "[");
    while ((row = mysql_fetch_row(result)))
    {
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

bool get_seat_map(const char *show_id, char *seat_map_str)
{
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE,
             "SELECT seat_map FROM shows WHERE id = %s",
             show_id);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Get Seat Map Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL)
    {
        fprintf(stderr, "Get Seat Map Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row == NULL)
    {
        mysql_free_result(result);
        return false;
    }

    strcpy(seat_map_str, row[0]); // seat_map

    mysql_free_result(result);

    return true;
}

bool book_ticket_db(const char *username, const char *film_id, const char *cinema_id, const char *show_id, int seat_number, const char *seat_list_str, char *response)
{
    MYSQL *conn_local = mysql_init(NULL);
    if (conn_local == NULL)
    {
        fprintf(stderr, "mysql_init() failed\n");
        return false;
    }

    // Initialize connection (use your actual database credentials)
    if (mysql_real_connect(conn_local, "localhost", "root", "quan5691", "ticket_booking2", 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "mysql_real_connect() failed\n");
        mysql_close(conn_local);
        return false;
    }

    // Start transaction
    if (mysql_query(conn_local, "START TRANSACTION"))
    {
        fprintf(stderr, "START TRANSACTION Error: %s\n", mysql_error(conn_local));
        mysql_close(conn_local);
        return false;
    }

    // Lock the row for the specific show
    char lock_query[BUFFER_SIZE];
    snprintf(lock_query, BUFFER_SIZE, "SELECT seat_map FROM shows WHERE id = %s FOR UPDATE", show_id);

    if (mysql_query(conn_local, lock_query))
    {
        fprintf(stderr, "Lock Error: %s\n", mysql_error(conn_local));
        mysql_query(conn_local, "ROLLBACK");
        mysql_close(conn_local);
        return false;
    }

    MYSQL_RES *result = mysql_store_result(conn_local);
    if (result == NULL)
    {
        fprintf(stderr, "Lock Error: %s\n", mysql_error(conn_local));
        mysql_query(conn_local, "ROLLBACK");
        mysql_close(conn_local);
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row == NULL)
    {
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

    while (seat_id_str != NULL && index < seat_number)
    {
        int seat_id = atoi(seat_id_str);
        seat_ids[index++] = seat_id;
        seat_id_str = strtok(NULL, ",[]");
    }

    if (index != seat_number)
    {
        fprintf(stderr, "Seat number mismatch\n");
        mysql_query(conn, "ROLLBACK");
        return false;
    }

    // Check seat availability and update seat map
    for (int i = 0; i < seat_number; i++)
    {
        int seat_id = seat_ids[i];
        int seat_index = seat_id_to_seat_map_index(seat_id, seats_per_row);

        if (seat_map[seat_index] == '1')
        {
            fprintf(stderr, "Seat %d is already booked\n", seat_id);
            mysql_query(conn, "ROLLBACK");
            return false;
        }
        else if (seat_map[seat_index] == '5')
        {
            fprintf(stderr, "Seat %d corresponds to a row separator\n", seat_id);
            mysql_query(conn, "ROLLBACK");
            return false;
        }
        else if (seat_map[seat_index] != '0')
        {
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

    if (mysql_query(conn_local, update_query))
    {
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

    if (mysql_query(conn_local, insert_query))
    {
        fprintf(stderr, "Insert Ticket Error: %s\n", mysql_error(conn_local));
        mysql_query(conn_local, "ROLLBACK");
        mysql_close(conn_local);
        return false;
    }

    // After successfully booking the ticket, fetch the additional information
    // Fetch film_name
    char film_name[BUFFER_SIZE];
    if (!get_film_name(film_id, film_name))
    {
        mysql_query(conn, "ROLLBACK");
        return false;
    }

    // Fetch cinema_name
    char cinema_name[BUFFER_SIZE];
    if (!get_cinema_name(cinema_id, cinema_name))
    {
        mysql_query(conn, "ROLLBACK");
        return false;
    }

    // Fetch show date and times
    char show_date[BUFFER_SIZE];
    char start_time[BUFFER_SIZE];
    char end_time[BUFFER_SIZE];
    if (!get_show_times(show_id, show_date, start_time, end_time))
    {
        mysql_query(conn, "ROLLBACK");
        return false;
    }

    // Commit transaction
    if (mysql_query(conn_local, "COMMIT"))
    {
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

// Helper functions for fetching infos of ticket after booking

bool get_film_name(const char *film_id, char *film_name)
{
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE, "SELECT film_name FROM films WHERE id = %s", film_id);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Get Film Name Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL)
    {
        fprintf(stderr, "Get Film Name Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row == NULL)
    {
        mysql_free_result(result);
        return false;
    }

    strcpy(film_name, row[0]);

    mysql_free_result(result);
    return true;
}

bool get_cinema_name(const char *cinema_id, char *cinema_name)
{
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE, "SELECT cinema_name FROM cinemas WHERE id = %s", cinema_id);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Get Cinema Name Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL)
    {
        fprintf(stderr, "Get Cinema Name Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row == NULL)
    {
        mysql_free_result(result);
        return false;
    }

    strcpy(cinema_name, row[0]);

    mysql_free_result(result);
    return true;
}

bool get_show_times(const char *show_id, char *show_date, char *start_time, char *end_time)
{
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE, "SELECT date, start_time, end_time FROM shows WHERE id = %s", show_id);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Get Show Times Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL)
    {
        fprintf(stderr, "Get Show Times Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row == NULL)
    {
        mysql_free_result(result);
        return false;
    }

    strcpy(show_date, row[0]);
    strcpy(start_time, row[1]);
    strcpy(end_time, row[2]);

    mysql_free_result(result);
    return true;
}

int add_film_db(const char *film_name, const char *category_id, const char *description, int length)
{
    char query[BUFFER_SIZE];
    char escaped_film_name[2 * strlen(film_name) + 1];
    char escaped_description[2 * strlen(description) + 1];

    // Escape inputs to prevent SQL injection
    mysql_real_escape_string(conn, escaped_film_name, film_name, strlen(film_name));
    mysql_real_escape_string(conn, escaped_description, description, strlen(description));

    // Check if the film already exists
    snprintf(query, BUFFER_SIZE,
             "SELECT id FROM films WHERE film_name='%s' AND category_id=%s AND length=%d",
             escaped_film_name, category_id, length);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Check Film Exists Error: %s\n", mysql_error(conn));
        return 0; // Server error
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL)
    {
        fprintf(stderr, "Check Film Exists Error: %s\n", mysql_error(conn));
        return 0; // Server error
    }

    if (mysql_num_rows(result) > 0)
    {
        // Film already exists
        mysql_free_result(result);
        return -1; // Indicate film exists
    }
    mysql_free_result(result);

    // Insert new film
    snprintf(query, BUFFER_SIZE,
             "INSERT INTO films (film_name, category_id, description, length) "
             "VALUES ('%s', %s, '%s', %d)",
             escaped_film_name, category_id, escaped_description, length);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Add Film Error: %s\n", mysql_error(conn));
        return 0; // Server error
    }

    return 1; // Success
}

bool check_film_db(const char *film_name, char *film_id, char *description, char *length_str)
{
    char query[BUFFER_SIZE];
    char escaped_film_name[2 * strlen(film_name) + 1];

    // Escape film name to prevent SQL injection
    mysql_real_escape_string(conn, escaped_film_name, film_name, strlen(film_name));

    snprintf(query, BUFFER_SIZE,
             "SELECT id, description, length FROM films WHERE film_name='%s'",
             escaped_film_name);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Check Film Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL)
    {
        fprintf(stderr, "Check Film Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row)
    {
        strcpy(film_id, row[0]);
        strcpy(description, row[1]);
        strcpy(length_str, row[2]);
        mysql_free_result(result);
        return true;
    }
    else
    {
        mysql_free_result(result);
        return false;
    }
}

int add_show_db(const char *film_id, const char *cinema_id, const char *date_str, 
    const char *start_time_str, const char *end_time_str) 
{
    char query[BUFFER_SIZE];

    // Step 1: Check if the show date is today or in the future
    struct tm show_date_tm = {0};
    if (strptime(date_str, "%Y-%m-%d", &show_date_tm) == NULL) {
        fprintf(stderr, "Invalid date format.\n");
        return -1; // Validation failed
    }
    time_t show_date_time = mktime(&show_date_tm);

    time_t current_time = time(NULL);
    struct tm *current_date_tm = localtime(&current_time);
    current_date_tm->tm_hour = 0;
    current_date_tm->tm_min = 0;
    current_date_tm->tm_sec = 0;
    time_t current_date_time = mktime(current_date_tm);

    if (difftime(show_date_time, current_date_time) < 0) {
        fprintf(stderr, "Show date must be today or in the future.\n");
        return -1; // Validation failed
    }

    // Step 2: Fetch the film's length from the database
    snprintf(query, BUFFER_SIZE, "SELECT length FROM films WHERE id = %s", film_id);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Get Film Length Error: %s\n", mysql_error(conn));
        return -1; // Validation failed
    }

    MYSQL_RES *film_result = mysql_store_result(conn);
    if (film_result == NULL) {
        fprintf(stderr, "Get Film Length Error: %s\n", mysql_error(conn));
        return -1; // Validation failed
    }

    MYSQL_ROW film_row = mysql_fetch_row(film_result);
    if (film_row == NULL) {
        mysql_free_result(film_result);
        fprintf(stderr, "Film not found.\n");
        return -1; // Validation failed
    }

    int film_length = atoi(film_row[0]); // in minutes
    mysql_free_result(film_result);

    // Step 3: Calculate the duration between start_time and end_time
    char datetime_start_str[BUFFER_SIZE];
    char datetime_end_str[BUFFER_SIZE];

    snprintf(datetime_start_str, BUFFER_SIZE, "%s %s", date_str, start_time_str);
    snprintf(datetime_end_str, BUFFER_SIZE, "%s %s", date_str, end_time_str);

    struct tm start_tm = {0};
    struct tm end_tm = {0};

    if (strptime(datetime_start_str, "%Y-%m-%d %H:%M", &start_tm) == NULL ||
        strptime(datetime_end_str, "%Y-%m-%d %H:%M", &end_tm) == NULL) {
        // fprintf(stderr, "Invalid time format.\n");
        return -1; // Validation failed
    }

    time_t start_time = mktime(&start_tm);
    time_t end_time = mktime(&end_tm);

    if (difftime(end_time, start_time) <= 0) {
        // fprintf(stderr, "End time must be after start time.\n");
        return -1; // Validation failed
    }

    double show_duration = difftime(end_time, start_time) / 60; // in minutes

    if (show_duration < film_length) {
        // fprintf(stderr, "Show duration (%.0f minutes) is less than film length (%d minutes).\n", show_duration, film_length);
        return -1; // Validation failed
    }

    // Step 4: Get seat_quantity from cinemas table
    snprintf(query, BUFFER_SIZE,
             "SELECT seat_quantity FROM cinemas WHERE id = %s",
             cinema_id);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Get Seat Quantity Error: %s\n", mysql_error(conn));
        return -1; // Validation failed
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Get Seat Quantity Error: %s\n", mysql_error(conn));
        return -1; // Validation failed
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row == NULL) {
        mysql_free_result(result);
        // fprintf(stderr, "Cinema not found.\n");
        return -1; // Validation failed
    }

    int total_seats = atoi(row[0]);
    mysql_free_result(result);

    // Step 5: Generate seat map
    int seats_per_row = 10;
    char seat_map[BUFFER_SIZE] = "";
    for (int i = 0; i < total_seats; i++) {
        if (i > 0 && i % seats_per_row == 0) {
            strcat(seat_map, "5"); // Row separator
        }
        strcat(seat_map, "0"); // Available seat
    }

    // Step 6: Insert new show into the shows table
    snprintf(query, BUFFER_SIZE,
             "INSERT INTO shows (film_id, cinema_id, date, start_time, end_time, seat_map) "
             "VALUES (%s, %s, '%s', '%s', '%s', '%s')",
             film_id, cinema_id, date_str, start_time_str, end_time_str, seat_map);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Add Show Error: %s\n", mysql_error(conn));
        return -1; // Validation failed
    }

    return 1; // Success
}

bool get_films_by_cinema_db(const char *cinema_id, char *films) {
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE,
             "SELECT DISTINCT f.id, f.film_name, f.length "
             "FROM films f "
             "JOIN shows s ON f.id = s.film_id "
             "WHERE s.cinema_id = %s AND s.date >= CURDATE()",
             cinema_id);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Get Films by Cinema Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Get Films by Cinema Error: %s\n", mysql_error(conn));
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
        strcat(films, row[2]); // length
        strcat(films, "}, ");
    }
    strcat(films, "]");
    mysql_free_result(result);
    return true;
}

bool get_shows_db(const char *film_id, const char *cinema_id, char *shows) {
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE,
             "SELECT id, date, start_time, end_time "
             "FROM shows "
             "WHERE film_id = %s AND cinema_id = %s",
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
        mysql_free_result(result);
        return false;
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

bool delete_show_db(const char *show_id) {
    char query[BUFFER_SIZE];

    // Check if the show date is in the future
    snprintf(query, BUFFER_SIZE,
             "SELECT date FROM shows WHERE id = %s",
             show_id);

    if (mysql_query(conn, query)) {
        // fprintf(stderr, "Delete Show Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        // fprintf(stderr, "Delete Show Error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row == NULL) {
        mysql_free_result(result);
        // fprintf(stderr, "Show not found.\n");
        return false;
    }

    char *date_str = row[0];
    mysql_free_result(result);

    // Compare the show date with current date
    char query_date[BUFFER_SIZE];
    snprintf(query_date, BUFFER_SIZE,
             "SELECT DATEDIFF('%s', CURDATE())",
             date_str);

    if (mysql_query(conn, query_date)) {
        // fprintf(stderr, "Delete Show Date Compare Error: %s\n", mysql_error(conn));
        return false;
    }

    result = mysql_store_result(conn);
    if (result == NULL) {
        // fprintf(stderr, "Delete Show Date Compare Error: %s\n", mysql_error(conn));
        return false;
    }

    row = mysql_fetch_row(result);
    if (row == NULL) {
        mysql_free_result(result);
        // fprintf(stderr, "Date comparison failed.\n");
        return false;
    }

    int date_diff = atoi(row[0]);
    mysql_free_result(result);

    if (date_diff < 0) {
        // fprintf(stderr, "Cannot delete past shows.\n");
        return false;
    }

    // Delete the show
    snprintf(query, BUFFER_SIZE,
             "DELETE FROM shows WHERE id = %s",
             show_id);

    if (mysql_query(conn, query)) {
        // fprintf(stderr, "Delete Show Error: %s\n", mysql_error(conn));
        return false;
    }

    return true;
}

int edit_show_db(const char *show_id, const char *date_str, const char *start_time_str, const char *end_time_str) {
    // Perform the same validations as in add_show_db
    // Ensure the new date and times are valid and meet the film length requirements

    // Fetch the film_id associated with the show
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE, "SELECT film_id FROM shows WHERE id = %s", show_id);

    if (mysql_query(conn, query)) {
        // fprintf(stderr, "Edit Show Error: %s\n", mysql_error(conn));
        return -1;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        // fprintf(stderr, "Edit Show Error: %s\n", mysql_error(conn));
        return -1;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row == NULL) {
        mysql_free_result(result);
        // fprintf(stderr, "Show not found.\n");
        return -1;
    }

    char film_id[BUFFER_SIZE];
    strcpy(film_id, row[0]);
    mysql_free_result(result);

    // Fetch the film length
    snprintf(query, BUFFER_SIZE, "SELECT length FROM films WHERE id = %s", film_id);

    if (mysql_query(conn, query)) {
        // fprintf(stderr, "Edit Show Error: %s\n", mysql_error(conn));
        return -1;
    }

    result = mysql_store_result(conn);
    if (result == NULL) {
        // fprintf(stderr, "Edit Show Error: %s\n", mysql_error(conn));
        return -1;
    }

    row = mysql_fetch_row(result);
    if (row == NULL) {
        mysql_free_result(result);
        // fprintf(stderr, "Film not found.\n");
        return -1;
    }

    int film_length = atoi(row[0]);
    mysql_free_result(result);

    // Perform date and time validations (similar to add_show_db)
    // ...

    // Update the show
    snprintf(query, BUFFER_SIZE,
             "UPDATE shows SET date = '%s', start_time = '%s', end_time = '%s' WHERE id = %s",
             date_str, start_time_str, end_time_str, show_id);

    if (mysql_query(conn, query)) {
        // fprintf(stderr, "Edit Show Error: %s\n", mysql_error(conn));
        return -1;
    }

    return 1;
}
