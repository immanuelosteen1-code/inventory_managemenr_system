# Web-Enabled Inventory Management System Using C

A complete academic semester project for a small retail shop to track stock in real time, manage inventory records, and generate reports.

## Project Overview
- **Title**: A Web-Enabled Inventory Management System Using C for Small Retail Shop Operations
- **Architecture**: 3-Tier (Frontend: HTML/CSS/JS, Backend: C (CGI), Database: MySQL)
- **Roles**: Manager (Full Access) and Employee (Restricted Access)
- **Scope**: Single retail shop operations (No e-commerce, online payments, or cloud deployment)

---

## 1. Project Structure
```text
inventory_management_system/
├── assets/                 # Shared UI assets (CSS, JS)
├── cgi-bin/                # Compiled C executables (CGI backend)
├── database/               # SQL scripts (Schema and Seed data)
├── public/                 # Web interface (HTML pages)
├── src/                    # C Source code and Header files
├── tests/                  # Unit tests and testing documentation
└── README.md               # Setup and Installation Guide
```

---

## 2. Prerequisites
Before installing, ensure you have the following installed on your machine:
1. **Local Server Environment**: XAMPP, WAMP, or Linux Apache with CGI support.
2. **C Compiler**: GCC (MinGW for Windows or build-essential for Linux).
3. **Database Management**: MySQL/MariaDB.
4. **C-MySQL Connector**: `libmysqlclient` (Linux) or `mysql-connector-c` (Windows).

---

## 3. Installation & How to Run

### Step 1: Database Setup
1. Start your **MySQL** server (e.g., via XAMPP Control Panel).
2. Open your MySQL client (phpMyAdmin).
3. Import the database schema:
   - Run the contents of `database/schema.sql`.
4. Import the seed data (test users and products):
   - Run the contents of `database/seed.sql`.

### Step 2: Compile the C Backend
The backend must be compiled into a CGI executable. Use the following command (adjust paths to your MySQL installation):

```bash
# For Windows (using MinGW/GCC)
gcc src/main.c src/auth.c src/inventory.c src/db_utils.c src/reports.c \
    -Isrc/include \
    -I"/c/Program Files/MySQL/MySQL Connector C 6.1/include" \
    -L"/c/Program Files/MySQL/MySQL Connector C 6.1/lib/vs14" \
    -llibmysql -o cgi-bin/main.cgi
```

### Step 3: Server Configuration (XAMPP Example)
1. **Move Files**: Copy the entire `inventory_management_system` folder to `C:/xampp/htdocs/`.
2. **CGI Permissions**: 
   - Ensure the `main.cgi` is inside the `cgi-bin/` folder.
   - Open `C:/xampp/apache/conf/httpd.conf` and ensure the following line is uncommented:
     `AddHandler cgi-script .cgi`
   - Also, ensure the `<Directory "C:/xampp/htdocs">` section has `ExecCGI` in its `Options`.
3. **Restart Apache**: Restart the Apache module in the XAMPP Control Panel.

### Step 4: Access the System
1. Open your browser.
2. Navigate to: `http://localhost/inventory_management_system/public/index.html`.
3. Log in using the test accounts below.

---

## 4. Test Accounts
| Role | Username | Password | Full Name |
| :--- | :--- | :--- | :--- |
| **Manager** | `manager` | `password123` | Admin Manager |
| **Employee** | `employee` | `password123` | Jane Employee |

---

## 5. Deployment Instructions (Local)
1. Start **Apache** and **MySQL** modules in your control panel (e.g., XAMPP Control Panel).
2. Open your web browser and navigate to `http://localhost/inventory/public/index.html`.
3. Log in with the test accounts provided above.

---

## 6. Project Documentation
- **Frontend Logic**: [api.js](assets/js/api.js) handles AJAX requests to the C backend.
- **Backend Routing**: [main.c](src/main.c) dispatches requests based on the `action` parameter.
- **Testing**: See [guide.md](tests/guide.md) for detailed unit, integration, and user acceptance test cases.

---

## 7. Educational Insights
- **3-Tier Architecture**: Demonstrates the separation of presentation, application, and data layers.
- **CGI (Common Gateway Interface)**: Shows how a low-level language like C can serve dynamic web content without a high-level framework.
- **RBAC**: Implements secure, role-based access control directly in the backend logic.
