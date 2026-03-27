#include "auth.h"
#include "db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool login_user(const char *username, const char *password, User *user) {
    MYSQL *conn = db_connect();
    if (!conn) return false;

    char query[512];
    // In a real project, we use SHA2() in SQL or a proper C library. 
    // Here we use SQL's SHA2 for simplicity and security.
    snprintf(query, sizeof(query), 
        "SELECT id, full_name, username, role, status FROM users "
        "WHERE username='%s' AND password_hash=SHA2('%s', 256)", 
        username, password);

    MYSQL_RES *res = db_fetch_results(conn, query);
    if (!res) {
        db_close(conn);
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    bool success = false;
    if (row) {
        user->id = atoi(row[0]);
        strncpy(user->full_name, row[1], sizeof(user->full_name) - 1);
        strncpy(user->username, row[2], sizeof(user->username) - 1);
        user->role = (strcmp(row[3], "manager") == 0) ? ROLE_MANAGER : ROLE_EMPLOYEE;
        user->active = (strcmp(row[4], "active") == 0);
        success = true;
    }

    mysql_free_result(res);
    db_close(conn);
    return success;
}

bool logout_user(int user_id) {
    // Audit log for logout
    MYSQL *conn = db_connect();
    if (conn) {
        char query[256];
        snprintf(query, sizeof(query), "INSERT INTO audit_logs (user_id, action_type, action_details) VALUES (%d, 'LOGOUT', 'User logged out')", user_id);
        db_execute_query(conn, query);
        db_close(conn);
    }
    return true;
}

bool check_session(const char *session_id, User *user) {
    // For this simple academic project, session handling is simplified.
    // In production, we'd check a sessions table.
    return false; 
}

char* hash_user_password(const char *password) {
    // Handled via SQL SHA2() in login_user for simplicity
    return NULL;
}

bool has_permission(UserRole role, const char *action) {
    if (role == ROLE_MANAGER) return true;
    if (role == ROLE_EMPLOYEE) {
        // Employee can view, search, update stock, but not delete or manage users
        if (strcmp(action, "DELETE_PRODUCT") == 0 || strcmp(action, "MANAGE_USERS") == 0 || strcmp(action, "GENERATE_REPORTS") == 0) {
            return false;
        }
        return true;
    }
    return false;
}
