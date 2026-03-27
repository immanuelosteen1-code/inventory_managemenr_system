#ifndef INVENTORY_H
#define INVENTORY_H

#include <stdbool.h>

#include <mysql.h>

typedef struct {
    int id;
    char product_name[150];
    char category[50];
    int quantity;
    double unit_price;
    int reorder_level;
    char description[500];
    char updated_at[20];
} Product;

// Product CRUD functions
bool add_product(const Product *product);
bool update_product(int product_id, const Product *product);
bool delete_product(int product_id);
Product* get_product_by_id(int product_id);
MYSQL_RES* get_all_products();
MYSQL_RES* search_products(const char *keyword);

// Stock management
bool update_stock(int product_id, int quantity_change, const char *movement_type, int user_id);
MYSQL_RES* get_low_stock_items();

// Stock movement logging
bool log_stock_movement(int product_id, const char *type, int change, int old_qty, int new_qty, int user_id);

#endif // INVENTORY_H
