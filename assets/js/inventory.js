/**
 * Inventory Management Logic
 * Handles display, CRUD, and searching
 */

document.addEventListener('DOMContentLoaded', () => {
    const inventoryTableBody = document.getElementById('inventoryTableBody');
    const searchInput = document.getElementById('searchInput');
    const searchBtn = document.getElementById('searchBtn');
    const addProductForm = document.getElementById('addProductForm');

    // Initial Load
    if (inventoryTableBody) {
        loadInventory();
    }

    // Search Handling
    if (searchBtn) {
        searchBtn.addEventListener('click', () => {
            loadInventory(searchInput.value);
        });
    }

    if (searchInput) {
        searchInput.addEventListener('keypress', (e) => {
            if (e.key === 'Enter') {
                loadInventory(searchInput.value);
            }
        });
    }

    // Add Product Handling
    if (addProductForm) {
        addProductForm.addEventListener('submit', async (e) => {
            e.preventDefault();
            const productData = {
                product_name: addProductForm.productName.value,
                category: addProductForm.category.value,
                quantity: parseInt(addProductForm.quantity.value),
                unit_price: parseFloat(addProductForm.unitPrice.value),
                reorder_level: parseInt(addProductForm.reorderLevel.value),
                description: addProductForm.description.value
            };

            const response = await api.addProduct(productData);
            if (response.status === 'success') {
                alert('Product added successfully!');
                addProductForm.reset();
                loadInventory();
            } else {
                alert('Error: ' + response.message);
            }
        });
    }
});

/**
 * Loads inventory items into the table
 * @param {string} query - Search keyword
 */
async function loadInventory(query = '') {
    const tableBody = document.getElementById('inventoryTableBody');
    if (!tableBody) return;

    tableBody.innerHTML = '<tr><td colspan="7" class="text-center">Loading inventory...</td></tr>';

    try {
        const response = await api.getInventory({ search: query });
        if (response.status === 'success') {
            const products = response.data;
            tableBody.innerHTML = '';

            if (products.length === 0) {
                tableBody.innerHTML = '<tr><td colspan="7" class="text-center">No products found.</td></tr>';
                return;
            }

            products.forEach(product => {
                const row = document.createElement('tr');
                const isLowStock = product.quantity <= product.reorder_level;
                
                row.innerHTML = `
                    <td>${product.id}</td>
                    <td>${product.product_name} ${isLowStock ? '<span style="color:red;">(Low Stock)</span>' : ''}</td>
                    <td>${product.category}</td>
                    <td>${product.quantity}</td>
                    <td>$${product.unit_price.toFixed(2)}</td>
                    <td>
                        <button onclick="openStockUpdateModal(${product.id}, '${product.product_name}')" class="btn btn-success btn-sm">Update Stock</button>
                        ${authHelper.getUser().role === 'manager' ? `
                            <button onclick="editProduct(${product.id})" class="btn btn-primary btn-sm">Edit</button>
                            <button onclick="deleteProduct(${product.id})" class="btn btn-danger btn-sm">Delete</button>
                        ` : ''}
                    </td>
                `;
                tableBody.appendChild(row);
            });
        }
    } catch (error) {
        console.error('Error loading inventory:', error);
        tableBody.innerHTML = '<tr><td colspan="7" class="text-center" style="color:red;">Failed to load inventory.</td></tr>';
    }
}

/**
 * Handles stock updates
 */
async function openStockUpdateModal(id, name) {
    const change = prompt(`Update stock for ${name}. Enter quantity to change (e.g., +5 for stock-in, -3 for stock-out):`);
    if (change === null || change === '') return;

    const quantityChange = parseInt(change);
    if (isNaN(quantityChange)) {
        alert('Invalid quantity change.');
        return;
    }

    const type = quantityChange > 0 ? 'in' : 'out';
    const response = await api.updateStock(id, Math.abs(quantityChange), type);
    
    if (response.status === 'success') {
        alert('Stock updated successfully!');
        loadInventory();
    } else {
        alert('Error: ' + response.message);
    }
}

/**
 * Manager-only delete product
 */
async function deleteProduct(id) {
    if (confirm('Are you sure you want to delete this product?')) {
        const response = await api.deleteProduct(id);
        if (response.status === 'success') {
            loadInventory();
        } else {
            alert('Error deleting product.');
        }
    }
}
