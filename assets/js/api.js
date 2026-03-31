/**
 * API Utility for Inventory Management System
 * Handles communication with the C-CGI backend
 */

const API_BASE_URL = '/cgi-bin/main.cgi';

const api = {
    /**
     * General request function
     * @param {string} action - The action to perform (e.g., 'login', 'get_inventory')
     * @param {string} method - HTTP method (GET or POST)
     * @param {Object} data - Optional data for POST requests
     * @returns {Promise<Object>} - Resolves to the JSON response
     */
    request: async (action, method = 'GET', data = null) => {
        let url = `${API_BASE_URL}?action=${action}`;
        
        const options = {
            method: method,
            headers: {
                'Content-Type': 'application/json'
            }
        };

        if (method === 'POST' && data) {
            options.body = JSON.stringify(data);
        } else if (method === 'GET' && data) {
            // Append data as query parameters if GET
            const params = new URLSearchParams(data).toString();
            url += `&${params}`;
        }

        try {
            const response = await fetch(url, options);
            const result = await response.json();
            if (!response.ok) {
                throw new Error(result.message || `HTTP error! status: ${response.status}`);
            }
            return await response.json();
        } catch (error) {
            console.error('API Request Error:', error);
            return { status: 'error', message: error.message };
        }
    },

    // Specific endpoints
    login: (credentials) => api.request('login', 'POST', credentials),
    logout: () => api.request('logout', 'POST'),
    getInventory: (filters = null) => api.request('get_inventory', 'GET', filters),
    addProduct: (product) => api.request('add_product', 'POST', product),
    updateProduct: (id, product) => api.request('update_product', 'POST', { id, ...product }),
    deleteProduct: (id) => api.request('delete_product', 'POST', { id }),
    updateStock: (id, change, type) => api.request('update_stock', 'POST', { id, change, type }),
    getReports: (interval) => api.request('get_reports', 'GET', { interval }),
    getAlerts: () => api.request('get_alerts', 'GET')
};

/**
 * Authentication Helper
 */
const authHelper = {
    isLoggedIn: () => {
        return !!localStorage.getItem('user_token');
    },
    getUser: () => {
        const user = localStorage.getItem('user');
        return user ? JSON.parse(user) : null;
    },
    setSession: (userData) => {
        localStorage.setItem('user', JSON.stringify(userData));
        localStorage.setItem('user_token', userData.token); // Mock token for demo
        localStorage.setItem('user_role', userData.role);
    },
    clearSession: () => {
        localStorage.removeItem('user');
        localStorage.removeItem('user_token');
        localStorage.removeItem('user_role');
        const basePath = '/inventory_managemenr_system';
        window.location.href = `${basePath}/public/index.html`;
    },
    checkAccess: (requiredRole) => {
        const role = localStorage.getItem('user_role');
        const basePath = '/inventory_managemenr_system';
        if (!role || (requiredRole && role !== requiredRole)) {
            window.location.href = `${basePath}/public/index.html`;
        }
    }
};

window.api = api;
window.authHelper = authHelper;
