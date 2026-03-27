#ifndef REPORTS_H
#define REPORTS_H

#include <mysql.h>

typedef enum {
    REPORT_DAILY,
    REPORT_WEEKLY,
    REPORT_MONTHLY
} ReportInterval;

typedef struct {
    int total_products;
    int total_stock;
    double total_inventory_value;
    int low_stock_count;
} InventorySummary;

// Report generation functions
InventorySummary* generate_summary();
MYSQL_RES* generate_stock_movement_report(ReportInterval interval);
MYSQL_RES* generate_stock_status_report();

// JSON conversion for reports
char* summary_to_json(const InventorySummary *summary);
char* movement_report_to_json(MYSQL_RES *results);

#endif // REPORTS_H
