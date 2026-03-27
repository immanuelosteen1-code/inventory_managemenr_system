document.addEventListener('DOMContentLoaded', () => {
    const loginForm = document.getElementById('loginForm');
    const errorMessage = document.getElementById('errorMessage');
    const logoutBtn = document.getElementById('logoutBtn');
    const userNameDisplay = document.getElementById('userNameDisplay');

    const BASE_PATH = '/inventory_managemenr_system';
    const MANAGER_DASHBOARD = `${BASE_PATH}/public/manager/dashboard.html`;
    const EMPLOYEE_DASHBOARD = `${BASE_PATH}/public/employee/dashboard.html`;
    const LOGIN_PAGE = `${BASE_PATH}/public/index.html`;

    function showError(message) {
        if (!errorMessage) return;
        errorMessage.textContent = message;
        errorMessage.style.display = 'block';
    }

    function hideError() {
        if (!errorMessage) return;
        errorMessage.style.display = 'none';
        errorMessage.textContent = '';
    }

    function redirectByRole(user) {
        if (!user || !user.role) {
            showError('User role is missing.');
            return;
        }

        if (user.role === 'manager') {
            window.location.href = MANAGER_DASHBOARD;
        } else if (user.role === 'employee') {
            window.location.href = EMPLOYEE_DASHBOARD;
        } else {
            showError('Login succeeded, but user role is not recognized.');
        }
    }

    if (loginForm) {
        loginForm.addEventListener('submit', async (e) => {
            e.preventDefault();
            hideError();

            const username = loginForm.username.value.trim();
            const password = loginForm.password.value;

            if (!username || !password) {
                showError('Please enter both username and password.');
                return;
            }

            try {
                const response = await api.login({ username, password });

                if (response && response.status === 'success' && response.user) {
                    const userData = response.user;
                    authHelper.setSession(userData);
                    redirectByRole(userData);
                } else {
                    showError(
                        (response && response.message) ||
                        'Invalid username or password.'
                    );
                }
            } catch (error) {
                console.error('Login error:', error);
                showError('An error occurred while logging in. Please try again.');
            }
        });
    }

    if (logoutBtn) {
        logoutBtn.addEventListener('click', async (e) => {
            e.preventDefault();

            try {
                if (typeof api !== 'undefined' && typeof api.logout === 'function') {
                    await api.logout();
                }
            } catch (error) {
                console.error('Logout error:', error);
            } finally {
                authHelper.clearSession();
                window.location.href = LOGIN_PAGE;
            }
        });
    }

    if (userNameDisplay) {
        const user = authHelper.getUser();
        if (user && user.full_name) {
            userNameDisplay.textContent = user.full_name;
        } else {
            window.location.href = LOGIN_PAGE;
        }
    }
});