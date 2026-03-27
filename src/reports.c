#include "reports.h"
#include "db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

InventorySummary* generate_summary() {
    MYSQL *conn = db_connect();
    if (!conn) return NULL;

    InventorySummary *summary = (InventorySummary*)malloc(sizeof(InventorySummary));
    
    // 1. General stats
    MYSQL_RES *res = db_fetch_results(conn, "SELECT COUNT(*), SUM(quantity), SUM(quantity * unit_price) FROM products");
    if (res) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row) {
            summary->total_products = row[0] ? atoi(row[0]) : 0;
            summary->total_stock = row[1] ? atoi(row[1]) : 0;
            summary->total_inventory_value = row[2] ? atof(row[2]) : 0.0;
        }
        mysql_free_result(res);
    }

    // 2. Low stock count
    res = db_fetch_results(conn, "SELECT COUNT(*) FROM products WHERE quantity <= reorder_level");
    if (res) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row) {
            summary->low_stock_count = row[0] ? atoi(row[0]) : 0;
        }
        mysql_free_result(res);
    }

    db_close(conn);
    return summary;
}

MYSQL_RES* generate_stock_movement_report(ReportInterval interval) {
    MYSQL *conn = db_connect();
    if (!conn) return NULL;

    const char *interval_sql;
    switch (interval) {
        case REPORT_DAILY: interval_sql = "CURDATE()"; break;
        case REPORT_WEEKLY: interval_sql = "DATE_SUB(CURDATE(), INTERVAL 1 WEEK)"; break;
        case REPORT_MONTHLY: interval_sql = "DATE_SUB(CURDATE(), INTERVAL 1 MONTH)"; break;
        default: interval_sql = "CURDATE()";
    }

    char query[1024];
    snprintf(query, sizeof(query), 
        "SELECT sm.created_at, p.product_name, sm.movement_type, sm.quantity_changed, "
        "sm.previous_qty, sm.new_qty, u.full_name "
        "FROM stock_movements sm "
        "JOIN products p ON sm.product_id = p.id "
        "JOIN users u ON sm.performed_by = u.id "
        "WHERE sm.created_at >= %s "
        "ORDER BY sm.created_at DESC", interval_sql);

    return db_fetch_results(conn, query);
}

MYSQL_RES* generate_stock_status_report() {
    MYSQL *conn = db_connect();
    if (!conn) return NULL;
    return db_fetch_results(conn, "SELECT product_name, quantity, reorder_level, (quantity <= reorder_level) as is_low FROM products");
}

char* summary_to_json(const InventorySummary *summary) {
    char *json = (char*)malloc(512);
    snprintf(json, 512, 
        "{\"total_products\": %d, \"total_stock\": %d, \"total_inventory_value\": %.2f, \"low_stock_count\": %d}",
        summary->total_products, summary->total_stock, summary->total_inventory_value, summary->low_stock_count);
    return json;
}

char* movement_report_to_json(MYSQL_RES *results) {
    if (!results) return strdup("[]");

    char *json = (char*)malloc(1024 * 10); // 10KB buffer for movements
    strcpy(json, "[");
    
    MYSQL_ROW row;
    int first = 1;
    while ((row = mysql_fetch_row(results))) {
        if (!first) strcat(json, ",");
        char entry[1024];
        snprintf(entry, sizeof(entry), 
            "{\"created_at\": \"%s\", \"product_name\": \"%s\", \"movement_type\": \"%s\", "
            "\"quantity_changed\": %s, \"previous_qty\": %s, \"new_qty\": %s, \"performed_by_name\": \"%s\"}",
            row[0], row[1], row[2], row[3], row[4], row[5], row[6]);
        strcat(json, entry);
        first = 0;
    }
    strcat(json, "]");
    mysql_free_result(results);
    return json;
}
