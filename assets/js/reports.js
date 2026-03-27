/**
 * Reports Management Logic
 * Handles generating and displaying inventory summaries and logs
 */

document.addEventListener('DOMContentLoaded', () => {
    const reportFilterForm = document.getElementById('reportFilterForm');
    const printReportBtn = document.getElementById('printReportBtn');

    if (reportFilterForm) {
        reportFilterForm.addEventListener('submit', async (e) => {
            e.preventDefault();
            const interval = reportFilterForm.reportInterval.value;
            await generateReport(interval);
        });
    }

    if (printReportBtn) {
        printReportBtn.addEventListener('click', () => {
            window.print();
        });
    }

    // Initial load for today
    if (reportFilterForm) {
        generateReport('daily');
    }
});

/**
 * Fetches and displays report data
 * @param {string} interval - daily, weekly, or monthly
 */
async function generateReport(interval) {
    const reportMovementsBody = document.getElementById('reportMovementsBody');
    const reportSummary = document.getElementById('reportSummary');
    
    if (!reportMovementsBody || !reportSummary) return;

    reportMovementsBody.innerHTML = '<tr><td colspan="7" class="text-center">Generating report...</td></tr>';

    try {
        const response = await api.getReports(interval);
        if (response.status === 'success') {
            const data = response.data;
            
            // 1. Update Summary Cards
            if (data.summary) {
                const summary = data.summary;
                reportSummary.innerHTML = `
                    <div class="card">
                        <h3>Total Items</h3>
                        <p style="font-size: 1.5rem; font-weight: bold;">${summary.total_products}</p>
                    </div>
                    <div class="card">
                        <h3>Total Stock</h3>
                        <p style="font-size: 1.5rem; font-weight: bold;">${summary.total_stock}</p>
                    </div>
                    <div class="card">
                        <h3>Low Stock Items</h3>
                        <p style="font-size: 1.5rem; font-weight: bold;">${summary.low_stock_count}</p>
                    </div>
                `;
            }

            // 2. Update Movements Table
            if (data.movements) {
                const movements = data.movements;
                reportMovementsBody.innerHTML = '';
                
                if (movements.length === 0) {
                    reportMovementsBody.innerHTML = '<tr><td colspan="7" class="text-center">No stock movements found for this period.</td></tr>';
                    return;
                }

                movements.forEach(move => {
                    const row = document.createElement('tr');
                    const changeClass = move.movement_type === 'in' ? 'success' : 'danger';
                    row.innerHTML = `
                        <td>${move.created_at}</td>
                        <td>${move.product_name}</td>
                        <td><span style="color: var(--${changeClass}-color); text-transform: uppercase; font-weight: bold;">${move.movement_type}</span></td>
                        <td>${move.movement_type === 'in' ? '+' : '-'}${move.quantity_changed}</td>
                        <td>${move.previous_qty}</td>
                        <td>${move.new_qty}</td>
                        <td>${move.performed_by_name}</td>
                    `;
                    reportMovementsBody.appendChild(row);
                });
            }
        }
    } catch (error) {
        console.error('Error generating report:', error);
        reportMovementsBody.innerHTML = '<tr><td colspan="7" class="text-center" style="color:red;">Failed to generate report.</td></tr>';
    }
}
