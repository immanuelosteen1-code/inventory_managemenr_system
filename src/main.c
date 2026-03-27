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
        // Simple mock: in a real CGI, we'd read stdin for JSON body
        // For academic demo, we assume the frontend sends credentials
        char username[50], password[50];
        // In a real implementation, we'd use a JSON parser here.
        // For now, we'll use a placeholder response for demonstration.
        send_json_response(200, "{\"status\": \"success\", \"user\": {\"id\": 1, \"full_name\": \"Admin Manager\", \"role\": \"manager\", \"token\": \"mock_token_123\"}}");
    } 
    
    // --- Inventory Actions ---
    else if (strcmp(action, "get_inventory") == 0) {
        MYSQL_RES *res = get_all_products();
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
        } else {
            send_json_response(500, "{\"status\": \"error\", \"message\": \"Database error\"}");
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
