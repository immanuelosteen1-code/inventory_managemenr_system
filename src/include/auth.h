#ifndef AUTH_H
#define AUTH_H

#include <stdbool.h>
#include <mysql.h>

typedef enum {
    ROLE_MANAGER,
    ROLE_EMPLOYEE,
    ROLE_NONE
} UserRole;

typedef struct {
    int id;
    char full_name[100];
    char username[50];
    UserRole role;
    bool active;
} User;

// Authentication functions
bool login_user(const char *username, const char *password, User *user);
bool logout_user(int user_id);
bool check_session(const char *session_id, User *user);

// Password utility
char* hash_user_password(const char *password);

// Role-based access check
bool has_permission(UserRole role, const char *action);

#endif // AUTH_H
