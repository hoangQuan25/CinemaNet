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
