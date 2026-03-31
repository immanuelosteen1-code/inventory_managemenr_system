#ifndef DB_H
#define DB_H

#include <mysql.h>
#include <stdbool.h>

/* Database configuration for XAMPP / MySQL */
#define DB_HOST "127.0.0.1"
#define DB_USER "root"
#define DB_PASS ""
#define DB_NAME "inventory_db"
#define DB_PORT 3306

MYSQL* db_connect(void);
void db_close(MYSQL *conn);
bool db_execute_query(MYSQL *conn, const char *query);
MYSQL_RES* db_fetch_results(MYSQL *conn, const char *query);
void send_json_response(int status_code, const char *json_data);

#endif