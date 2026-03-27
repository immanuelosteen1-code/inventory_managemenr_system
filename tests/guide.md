# Testing Guide: Inventory Management System

This guide outlines the testing strategies used to verify the functionality, security, and reliability of the Web-Enabled Inventory Management System.

## 1. Unit Testing
Unit tests focus on individual C functions and logic.

### Key Test Cases
- **Stock Calculations**: Verify that `new_qty = old_qty + change` (stock-in) and `new_qty = old_qty - change` (stock-out).
- **Role-Based Access Control (RBAC)**: Ensure `has_permission()` correctly returns `false` for restricted actions (e.g., Employee deleting a product).
- **Data Validation**: Check that input strings (product names, categories) are within defined buffer sizes.

### Running Unit Tests
Compile and run the test suite:
```bash
gcc -o unit_test tests/unit_test.c src/auth.c -I src
./unit_test
```

---

## 2. Integration Testing
Verifies communication between the Frontend, C-CGI Backend, and MySQL Database.

### Test Checklist
- [ ] **Database Connection**: C module successfully connects to `inventory_db` using `mysql_real_connect`.
- [ ] **API Endpoint (CGI)**: `main.cgi?action=get_inventory` returns a valid JSON array of products.
- [ ] **Frontend Fetch**: JavaScript `fetch()` successfully calls the CGI endpoint and renders the table.
- [ ] **Stock Update Loop**: Update stock on frontend -> C backend writes to MySQL -> frontend refreshes with new quantity.

---

## 3. User Acceptance Testing (UAT)
Final verification from the user's perspective (Manager and Employee roles).

### Manager Workflow
| Task | Expected Result |
| :--- | :--- |
| Login as 'manager' | Redirected to Manager Dashboard |
| Add new product | Product appears in inventory table |
| View Low Stock Alerts | Items below reorder level are highlighted |
| Generate Monthly Report | Summary and movement logs are displayed for the past 30 days |
| Delete a product | Product is permanently removed from the table |

### Employee Workflow
| Task | Expected Result |
| :--- | :--- |
| Login as 'employee' | Redirected to Employee Dashboard |
| Search for a product | List filters in real-time based on keyword |
| Update stock quantity | Quantity changes, but "Delete" and "Edit" buttons are hidden |
| Access Reports page | System denies access or redirects to login |

---

## 4. Security Testing
- **SQL Injection**: Attempt to login with `' OR '1'='1`. (Prevented by sanitized queries).
- **Unauthorized Access**: Attempt to access `/public/manager/dashboard.html` without logging in. (Prevented by `authHelper.checkAccess`).
- **Input Sanitization**: Ensure negative stock updates are rejected by the backend.
