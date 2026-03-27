#ifndef DB_H
#define DB_H

#include <mysql.h>
#include <stdbool.h>

// Database configuration
#define DB_HOST "localhost"
#define DB_USER "root"
#define DB_PASS ""
#define DB_NAME "inventory_db"

// Function prototypes
MYSQL* db_connect();
void db_close(MYSQL *conn);
bool db_execute_query(MYSQL *conn, const char *query);
MYSQL_RES* db_fetch_results(MYSQL *conn, const char *query);

// JSON utility for CGI response
void send_json_response(int status_code, const char *json_data);

#endif // DB_H
