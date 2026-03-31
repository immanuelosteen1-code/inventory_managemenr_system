#include "db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

MYSQL* db_connect(void) {
    MYSQL *conn = mysql_init(NULL);

    if (conn == NULL) {
        printf("Status: 500\r\n");
        printf("Content-Type: application/json\r\n\r\n");
        printf("{\"status\":\"error\",\"message\":\"mysql_init() failed\"}");
        return NULL;
    }

    if (mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, DB_PORT, NULL, 0) == NULL) {
        printf("Status: 500\r\n");
        printf("Content-Type: application/json\r\n\r\n");
        printf("{\"status\":\"error\",\"message\":\"mysql_real_connect() failed: %s\"}", mysql_error(conn));
        mysql_close(conn);
        return NULL;
    }

    return conn;
}

void db_close(MYSQL *conn) {
    if (conn != NULL) {
        mysql_close(conn);
    }
}

bool db_execute_query(MYSQL *conn, const char *query) {
    if (conn == NULL || query == NULL) {
        fprintf(stderr, "QUERY ERROR: invalid connection or query\n");
        return false;
    }

    if (mysql_query(conn, query) != 0) {
        fprintf(stderr, "QUERY ERROR: %s\n", mysql_error(conn));
        return false;
    }

    return true;
}

MYSQL_RES* db_fetch_results(MYSQL *conn, const char *query) {
    if (conn == NULL || query == NULL) {
        fprintf(stderr, "FETCH ERROR: invalid connection or query\n");
        return NULL;
    }

    if (mysql_query(conn, query) != 0) {
        fprintf(stderr, "FETCH ERROR: %s\n", mysql_error(conn));
        return NULL;
    }

    return mysql_store_result(conn);
}

void send_json_response(int status_code, const char *json_data) {
    printf("Status: %d\r\n", status_code);
    printf("Content-Type: application/json\r\n\r\n");
    printf("%s", json_data != NULL ? json_data : "{\"status\":\"error\",\"message\":\"Empty response\"}");
}