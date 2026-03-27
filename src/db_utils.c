#include "db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

MYSQL* db_connect() {
    MYSQL *conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "mysql_init() failed\n");
        return NULL;
    }

    if (mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0) == NULL) {
        fprintf(stderr, "mysql_real_connect() failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return NULL;
    }

    return conn;
}

void db_close(MYSQL *conn) {
    if (conn) {
        mysql_close(conn);
    }
}

bool db_execute_query(MYSQL *conn, const char *query) {
    if (mysql_query(conn, query)) {
        fprintf(stderr, "QUERY ERROR: %s\n", mysql_error(conn));
        return false;
    }
    return true;
}

MYSQL_RES* db_fetch_results(MYSQL *conn, const char *query) {
    if (mysql_query(conn, query)) {
        fprintf(stderr, "FETCH ERROR: %s\n", mysql_error(conn));
        return NULL;
    }
    return mysql_store_result(conn);
}

void send_json_response(int status_code, const char *json_data) {
    printf("Status: %d\n", status_code);
    printf("Content-Type: application/json\n\n");
    printf("%s", json_data);
}
