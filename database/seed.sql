-- Seed Data for Inventory Management System
USE inventory_db;

-- Sample Users (Password is 'password123' for all)
-- In a real scenario, these would be securely hashed.
INSERT INTO users (full_name, username, password_hash, role) VALUES
('Admin Manager', 'manager', 'ef92b778bafe421e592022c00e8f000b0e9391e56920a03006d917f9e83002f5', 'manager'),
('Jane Employee', 'employee', 'ef92b778bafe421e592022c00e8f000b0e9391e56920a03006d917f9e83002f5', 'employee');
('Jane Employee', 'employee', 'ef92b778bafe421e592022c00e8f000b0e9391e56920a03006d917f9e83002f5', 'employee');

-- Sample Products
INSERT INTO products (product_name, category, quantity, unit_price, reorder_level, description) VALUES
('Laptop Dell XPS 13', 'Electronics', 15, 1200.00, 5, 'High performance ultrabook'),
('Smartphone Samsung Galaxy S21', 'Electronics', 25, 800.00, 8, 'Latest flagship smartphone'),
('Office Chair', 'Furniture', 40, 150.00, 10, 'Ergonomic swivel chair'),
('Coffee Maker', 'Appliances', 12, 80.00, 4, 'Programmable coffee brewer'),
('USB-C Hub', 'Accessories', 100, 45.00, 20, 'Multi-port adapter for laptops'),
('A4 Paper Rim', 'Stationery', 5, 5.50, 15, '500 sheets of high-quality paper'); -- Low stock example

-- Initial Stock Movements
INSERT INTO stock_movements (product_id, movement_type, quantity_changed, previous_qty, new_qty, performed_by) VALUES
(1, 'in', 15, 0, 15, 1),
(2, 'in', 25, 0, 25, 1),
(3, 'in', 40, 0, 40, 1),
(4, 'in', 12, 0, 12, 1),
(5, 'in', 100, 0, 100, 1),
(6, 'in', 5, 0, 5, 1);
