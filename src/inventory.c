#include "inventory.h"
#include "db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool add_product(const Product *product) {
    MYSQL *conn = db_connect();
    if (!conn) return false;

    char query[1024];
    snprintf(query, sizeof(query), 
        "INSERT INTO products (product_name, category, quantity, unit_price, reorder_level, description) "
        "VALUES ('%s', '%s', %d, %.2f, %d, '%s')",
        product->product_name, product->category, product->quantity, product->unit_price, 
        product->reorder_level, product->description);

    bool res = db_execute_query(conn, query);
    db_close(conn);
    return res;
}

bool update_product(int product_id, const Product *product) {
    MYSQL *conn = db_connect();
    if (!conn) return false;

    char query[1024];
    snprintf(query, sizeof(query), 
        "UPDATE products SET product_name='%s', category='%s', unit_price=%.2f, reorder_level=%d, description='%s' "
        "WHERE id=%d",
        product->product_name, product->category, product->unit_price, 
        product->reorder_level, product->description, product_id);

    bool res = db_execute_query(conn, query);
    db_close(conn);
    return res;
}

bool delete_product(int product_id) {
    MYSQL *conn = db_connect();
    if (!conn) return false;

    char query[256];
    snprintf(query, sizeof(query), "DELETE FROM products WHERE id=%d", product_id);

    bool res = db_execute_query(conn, query);
    db_close(conn);
    return res;
}

Product* get_product_by_id(int product_id) {
    MYSQL *conn = db_connect();
    if (!conn) return NULL;

    char query[256];
    snprintf(query, sizeof(query), "SELECT id, product_name, category, quantity, unit_price, reorder_level, description FROM products WHERE id=%d", product_id);

    MYSQL_RES *res = db_fetch_results(conn, query);
    if (!res) {
        db_close(conn);
        return NULL;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    Product *p = NULL;
    if (row) {
        p = (Product*)malloc(sizeof(Product));
        p->id = atoi(row[0]);
        strncpy(p->product_name, row[1], sizeof(p->product_name)-1);
        strncpy(p->category, row[2], sizeof(p->category)-1);
        p->quantity = atoi(row[3]);
        p->unit_price = atof(row[4]);
        p->reorder_level = atoi(row[5]);
        strncpy(p->description, row[6], sizeof(p->description)-1);
    }

    mysql_free_result(res);
    db_close(conn);
    return p;
}

MYSQL_RES* get_all_products() {
    MYSQL *conn = db_connect();
    if (!conn) return NULL;
    return db_fetch_results(conn, "SELECT id, product_name, category, quantity, unit_price, reorder_level FROM products ORDER BY id DESC");
}

MYSQL_RES* search_products(const char *keyword) {
    MYSQL *conn = db_connect();
    if (!conn) return NULL;
    char query[512];
    snprintf(query, sizeof(query), "SELECT id, product_name, category, quantity, unit_price, reorder_level FROM products WHERE product_name LIKE '%%%s%%' OR category LIKE '%%%s%%'", keyword, keyword);
    return db_fetch_results(conn, query);
}

bool update_stock(int product_id, int quantity_change, const char *movement_type, int user_id) {
    MYSQL *conn = db_connect();
    if (!conn) return false;

    // 1. Get current qty
    char query[256];
    snprintf(query, sizeof(query), "SELECT quantity FROM products WHERE id=%d", product_id);
    MYSQL_RES *res = db_fetch_results(conn, query);
    if (!res) { db_close(conn); return false; }
    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) { mysql_free_result(res); db_close(conn); return false; }
    
    int old_qty = atoi(row[0]);
    int new_qty = (strcmp(movement_type, "in") == 0) ? old_qty + quantity_change : old_qty - quantity_change;
    mysql_free_result(res);

    if (new_qty < 0) { db_close(conn); return false; } // Can't have negative stock

    // 2. Update products table
    snprintf(query, sizeof(query), "UPDATE products SET quantity=%d WHERE id=%d", new_qty, product_id);
    if (!db_execute_query(conn, query)) { db_close(conn); return false; }

    // 3. Log movement
    log_stock_movement(product_id, movement_type, quantity_change, old_qty, new_qty, user_id);

    db_close(conn);
    return true;
}

MYSQL_RES* get_low_stock_items() {
    MYSQL *conn = db_connect();
    if (!conn) return NULL;
    return db_fetch_results(conn, "SELECT id, product_name, quantity, reorder_level FROM products WHERE quantity <= reorder_level");
}

bool log_stock_movement(int product_id, const char *type, int change, int old_qty, int new_qty, int user_id) {
    MYSQL *conn = db_connect();
    if (!conn) return false;

    char query[512];
    snprintf(query, sizeof(query), 
        "INSERT INTO stock_movements (product_id, movement_type, quantity_changed, previous_qty, new_qty, performed_by) "
        "VALUES (%d, '%s', %d, %d, %d, %d)",
        product_id, type, change, old_qty, new_qty, user_id);

    bool res = db_execute_query(conn, query);
    db_close(conn);
    return res;
}
