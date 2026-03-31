#include "db.h"
#include "auth.h"
#include "inventory.h"
#include "reports.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper to get query parameter value
char* get_query_param(const char *query, const char *key) {
    if (!query || !key) return NULL;
    char *found = strstr(query, key);
    if (!found) return NULL;
    
    found += strlen(key) + 1; // skip "key="
    char *end = strchr(found, '&');
    int len = end ? (int)(end - found) : (int)strlen(found);
    
    char *val = (char*)malloc(len + 1);
    strncpy(val, found, len);
    val[len] = '\0';
    return val;
}

// Helper to get value from simple JSON string (e.g., {"key": "val"})
char* get_json_val(const char *json, const char *key) {
    if (!json || !key) return NULL;
    char search_key[128];
    snprintf(search_key, sizeof(search_key), "\"%s\":", key);
    char *found = strstr(json, search_key);
    if (!found) return NULL;
    
    found += strlen(search_key);
    // Skip spaces
    while (*found == ' ') found++;
    
    if (*found == '\"') {
        found++; // Skip opening quote
        char *end = strchr(found, '\"');
        if (!end) return NULL;
        int len = (int)(end - found);
        char *val = (char*)malloc(len + 1);
        strncpy(val, found, len);
        val[len] = '\0';
        return val;
    } else {
        // Handle numbers
        char *end = strpbrk(found, ",}");
        int len = end ? (int)(end - found) : (int)strlen(found);
        char *val = (char*)malloc(len + 1);
        strncpy(val, found, len);
        val[len] = '\0';
        return val;
    }
}

int main(int argc, char *argv[]) {
    char *method = getenv("REQUEST_METHOD");
    char *query = getenv("QUERY_STRING");
    char *action = get_query_param(query, "action");

    if (!action) {
        send_json_response(400, "{\"status\": \"error\", \"message\": \"No action specified\"}");
        return 0;
    }

    // --- Authentication Actions ---
    if (strcmp(action, "login") == 0 && method && strcmp(method, "POST") == 0) {
        int content_len = 0;
        char *cl = getenv("CONTENT_LENGTH");
        if (cl) content_len = atoi(cl);
        if (content_len > 0) {
            char *post_data = (char*)malloc(content_len + 1);
            fread(post_data, 1, content_len, stdin);
            post_data[content_len] = '\0';
            char *uname = get_json_val(post_data, "username");
            char *pass = get_json_val(post_data, "password");
            if (uname && pass) {
                User u;
                if (login_user(uname, pass, &u) && u.active) {
                    const char *role_str = (u.role == ROLE_MANAGER) ? "manager" : "employee";
                    char resp[512];
                    snprintf(resp, sizeof(resp),
                        "{\"status\": \"success\", \"user\": {\"id\": %d, \"full_name\": \"%s\", \"role\": \"%s\", \"token\": \"session_mock\"}}",
                        u.id, u.full_name, role_str);
                    send_json_response(200, resp);
                } else {
                    send_json_response(401, "{\"status\": \"error\", \"message\": \"Invalid credentials or inactive user\"}");
                }
            } else {
                send_json_response(400, "{\"status\": \"error\", \"message\": \"Missing username or password\"}");
            }
            if (uname) free(uname);
            if (pass) free(pass);
            free(post_data);
        } else {
            send_json_response(400, "{\"status\": \"error\", \"message\": \"No data received\"}");
        }
    }
    
    // --- Inventory Actions ---
    else if (strcmp(action, "add_product") == 0 && method && strcmp(method, "POST") == 0) {
        char content_len_str[16];
        int content_len = 0;
        char *cl = getenv("CONTENT_LENGTH");
        if (cl) content_len = atoi(cl);
        
        if (content_len > 0) {
            char *post_data = (char*)malloc(content_len + 1);
            fread(post_data, 1, content_len, stdin);
            post_data[content_len] = '\0';
            
            Product p;
            char *name = get_json_val(post_data, "product_name");
            char *cat = get_json_val(post_data, "category");
            char *qty = get_json_val(post_data, "quantity");
            char *price = get_json_val(post_data, "unit_price");
            char *reorder = get_json_val(post_data, "reorder_level");
            char *desc = get_json_val(post_data, "description");
            
            if (name) strncpy(p.product_name, name, sizeof(p.product_name)-1);
            if (cat) strncpy(p.category, cat, sizeof(p.category)-1);
            p.quantity = qty ? atoi(qty) : 0;
            p.unit_price = price ? atof(price) : 0.0;
            p.reorder_level = reorder ? atoi(reorder) : 10;
            if (desc) strncpy(p.description, desc, sizeof(p.description)-1);
            
            if (add_product(&p)) {
                send_json_response(200, "{\"status\": \"success\", \"message\": \"Product added successfully\"}");
            } else {
                send_json_response(500, "{\"status\": \"error\", \"message\": \"Failed to add product to database\"}");
            }
            
            if (name) free(name); if (cat) free(cat); if (qty) free(qty);
            if (price) free(price); if (reorder) free(reorder); if (desc) free(desc);
            free(post_data);
        } else {
            send_json_response(400, "{\"status\": \"error\", \"message\": \"No data received\"}");
        }
    }

    else if (strcmp(action, "get_inventory") == 0) {
        MYSQL_RES *res = NULL;
        char *search = get_query_param(query, "search");
        if (search && strlen(search) > 0) {
            res = search_products(search);
        } else {
            res = get_all_products();
        }
        if (res) {
            char *json = (char*)malloc(1024 * 10);
            strcpy(json, "{\"status\": \"success\", \"data\": [");
            MYSQL_ROW row;
            int first = 1;
            while ((row = mysql_fetch_row(res))) {
                if (!first) strcat(json, ",");
                char entry[512];
                snprintf(entry, sizeof(entry), 
                    "{\"id\": %s, \"product_name\": \"%s\", \"category\": \"%s\", \"quantity\": %s, \"unit_price\": %s, \"reorder_level\": %s}",
                    row[0], row[1], row[2], row[3], row[4], row[5]);
                strcat(json, entry);
                first = 0;
            }
            strcat(json, "]}");
            send_json_response(200, json);
            free(json);
            mysql_free_result(res);
            if (search) free(search);
        } else {
            send_json_response(500, "{\"status\": \"error\", \"message\": \"Database error\"}");
        }
    }
    
    else if (strcmp(action, "update_stock") == 0 && method && strcmp(method, "POST") == 0) {
        int content_len = 0;
        char *cl = getenv("CONTENT_LENGTH");
        if (cl) content_len = atoi(cl);
        if (content_len > 0) {
            char *post_data = (char*)malloc(content_len + 1);
            fread(post_data, 1, content_len, stdin);
            post_data[content_len] = '\0';
            char *id = get_json_val(post_data, "id");
            char *chg = get_json_val(post_data, "change");
            char *type = get_json_val(post_data, "type");
            int ok = 0;
            if (id && chg && type) {
                ok = update_stock(atoi(id), atoi(chg), type, 1) ? 1 : 0;
            }
            if (ok) {
                send_json_response(200, "{\"status\": \"success\", \"message\": \"Stock updated\"}");
            } else {
                send_json_response(500, "{\"status\": \"error\", \"message\": \"Failed to update stock\"}");
            }
            if (id) free(id); if (chg) free(chg); if (type) free(type);
            free(post_data);
        } else {
            send_json_response(400, "{\"status\": \"error\", \"message\": \"No data received\"}");
        }
    }
    
    else if (strcmp(action, "delete_product") == 0 && method && strcmp(method, "POST") == 0) {
        int content_len = 0;
        char *cl = getenv("CONTENT_LENGTH");
        if (cl) content_len = atoi(cl);
        if (content_len > 0) {
            char *post_data = (char*)malloc(content_len + 1);
            fread(post_data, 1, content_len, stdin);
            post_data[content_len] = '\0';
            char *id = get_json_val(post_data, "id");
            int ok = 0;
            if (id) {
                ok = delete_product(atoi(id)) ? 1 : 0;
            }
            if (ok) {
                send_json_response(200, "{\"status\": \"success\", \"message\": \"Product deleted\"}");
            } else {
                send_json_response(500, "{\"status\": \"error\", \"message\": \"Failed to delete product\"}");
            }
            if (id) free(id);
            free(post_data);
        } else {
            send_json_response(400, "{\"status\": \"error\", \"message\": \"No data received\"}");
        }
    }
    
    else if (strcmp(action, "update_product") == 0 && method && strcmp(method, "POST") == 0) {
        int content_len = 0;
        char *cl = getenv("CONTENT_LENGTH");
        if (cl) content_len = atoi(cl);
        if (content_len > 0) {
            char *post_data = (char*)malloc(content_len + 1);
            fread(post_data, 1, content_len, stdin);
            post_data[content_len] = '\0';
            char *id = get_json_val(post_data, "id");
            Product p;
            char *name = get_json_val(post_data, "product_name");
            char *cat = get_json_val(post_data, "category");
            char *price = get_json_val(post_data, "unit_price");
            char *reorder = get_json_val(post_data, "reorder_level");
            char *desc = get_json_val(post_data, "description");
            if (name) strncpy(p.product_name, name, sizeof(p.product_name)-1);
            if (cat) strncpy(p.category, cat, sizeof(p.category)-1);
            p.unit_price = price ? atof(price) : 0.0;
            p.reorder_level = reorder ? atoi(reorder) : 10;
            if (desc) strncpy(p.description, desc, sizeof(p.description)-1);
            int ok = 0;
            if (id) {
                ok = update_product(atoi(id), &p) ? 1 : 0;
            }
            if (ok) {
                send_json_response(200, "{\"status\": \"success\", \"message\": \"Product updated\"}");
            } else {
                send_json_response(500, "{\"status\": \"error\", \"message\": \"Failed to update product\"}");
            }
            if (id) free(id);
            if (name) free(name); if (cat) free(cat);
            if (price) free(price); if (reorder) free(reorder); if (desc) free(desc);
            free(post_data);
        } else {
            send_json_response(400, "{\"status\": \"error\", \"message\": \"No data received\"}");
        }
    }
    
    else if (strcmp(action, "get_alerts") == 0) {
        MYSQL_RES *res = get_low_stock_items();
        if (res) {
            char *json = (char*)malloc(1024 * 5);
            strcpy(json, "{\"status\": \"success\", \"data\": [");
            MYSQL_ROW row;
            int first = 1;
            while ((row = mysql_fetch_row(res))) {
                if (!first) strcat(json, ",");
                char entry[256];
                snprintf(entry, sizeof(entry), 
                    "{\"id\": %s, \"product_name\": \"%s\", \"quantity\": %s, \"reorder_level\": %s}",
                    row[0], row[1], row[2], row[3]);
                strcat(json, entry);
                first = 0;
            }
            strcat(json, "]}");
            send_json_response(200, json);
            free(json);
            mysql_free_result(res);
        }
    }

    // --- Report Actions ---
    else if (strcmp(action, "get_reports") == 0) {
        char *interval_str = get_query_param(query, "interval");
        if (interval_str && strcmp(interval_str, "summary") == 0) {
            InventorySummary *sum = generate_summary();
            char *sum_json = summary_to_json(sum);
            char resp[1024];
            snprintf(resp, sizeof(resp), "{\"status\": \"success\", \"data\": %s}", sum_json);
            send_json_response(200, resp);
            free(sum); free(sum_json);
        } else {
            ReportInterval interval = REPORT_DAILY;
            if (interval_str) {
                if (strcmp(interval_str, "weekly") == 0) interval = REPORT_WEEKLY;
                else if (strcmp(interval_str, "monthly") == 0) interval = REPORT_MONTHLY;
            }
            MYSQL_RES *res = generate_stock_movement_report(interval);
            char *movements_json = movement_report_to_json(res);
            InventorySummary *sum = generate_summary();
            char *sum_json = summary_to_json(sum);
            
            char *final_resp = (char*)malloc(strlen(movements_json) + strlen(sum_json) + 256);
            sprintf(final_resp, "{\"status\": \"success\", \"data\": {\"summary\": %s, \"movements\": %s}}", sum_json, movements_json);
            send_json_response(200, final_resp);
            
            free(movements_json); free(sum_json); free(sum); free(final_resp);
        }
        if (interval_str) free(interval_str);
    }

    else {
        send_json_response(404, "{\"status\": \"error\", \"message\": \"Action not found or method not allowed\"}");
    }

    if (action) free(action);
    return 0;
}
