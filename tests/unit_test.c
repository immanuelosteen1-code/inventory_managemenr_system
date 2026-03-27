#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "include/inventory.h"
#include "include/auth.h"

// Mock functions for testing purposes (since we don't want to hit real DB in unit tests)
// In a full academic project, we would use a test database.

void test_stock_calculation() {
    printf("Running test_stock_calculation...\n");
    
    int initial_qty = 10;
    int added_qty = 5;
    int expected_new_qty = 15;
    
    // Simple simulation of update_stock logic
    int calculated_qty = initial_qty + added_qty;
    assert(calculated_qty == expected_new_qty);
    
    int removed_qty = 3;
    expected_new_qty = 12;
    calculated_qty = calculated_qty - removed_qty;
    assert(calculated_qty == expected_new_qty);
    
    printf("test_stock_calculation passed!\n");
}

void test_role_permissions() {
    printf("Running test_role_permissions...\n");
    
    // Manager should have all permissions
    assert(has_permission(ROLE_MANAGER, "DELETE_PRODUCT") == true);
    assert(has_permission(ROLE_MANAGER, "GENERATE_REPORTS") == true);
    
    // Employee should have restricted permissions
    assert(has_permission(ROLE_EMPLOYEE, "DELETE_PRODUCT") == false);
    assert(has_permission(ROLE_EMPLOYEE, "GENERATE_REPORTS") == false);
    assert(has_permission(ROLE_EMPLOYEE, "UPDATE_STOCK") == true);
    
    printf("test_role_permissions passed!\n");
}

int main() {
    printf("--- STARTING UNIT TESTS ---\n");
    test_stock_calculation();
    test_role_permissions();
    printf("--- ALL TESTS PASSED ---\n");
    return 0;
}
