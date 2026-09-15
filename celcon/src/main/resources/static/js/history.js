// history.js
function getChartData() {
    // Используем данные из window.chartData
    if (window.chartData) {
        console.log('window.chartData:', window.chartData);

        // Важно: changesByDate может быть строкой или объектом
        let changesByDate = window.chartData.changesByDate || {};

        // Если это строка, парсим её
        if (typeof changesByDate === 'string') {
            try {
                changesByDate = JSON.parse(changesByDate);
                console.log('Распарсили строку в объект:', changesByDate);
            } catch (e) {
                console.error('Не удалось распарсить changesByDate строку:', changesByDate);
                changesByDate = {};
            }
        }

        return {
            changesByDate: changesByDate,
            dateFrom: window.chartData.dateFrom || '',
            dateTo: window.chartData.dateTo || '',
            totalChanges: window.chartData.totalChanges || 0
        };
    }

    // Fallback на старый метод через data-атрибуты
    const dataElement = document.getElementById('chart-data');
    if (!dataElement) return null;

    try {
        // Пробуем распарсить data-changes
        let changesByDate = {};
        const changesStr = dataElement.dataset.changes;
        console.log('Raw changes string:', changesStr);

        if (changesStr) {
            try {
                changesByDate = JSON.parse(changesStr);
            } catch (e) {
                console.error('Ошибка парсинга changes JSON:', e);
                // Пробуем другой формат
                changesByDate = {};
            }
        }

        return {
            changesByDate: changesByDate,
            dateFrom: dataElement.dataset.from || '',
            dateTo: dataElement.dataset.to || '',
            totalChanges: parseInt(dataElement.dataset.total || '0')
        };
    } catch (e) {
        console.error('Ошибка получения данных:', e);
        return null;
    }
}

function initChart() {
    console.log('initChart started');

    const data = getChartData();
    console.log('Chart data after processing:', data);

    if (!data) {
        console.error('Нет данных для графика');
        return;
    }

    let changesByDate = data.changesByDate;
    console.log('Changes by date type:', typeof changesByDate);
    console.log('Changes by date value:', changesByDate);

    // Если это все еще строка (на всякий случай)
    if (typeof changesByDate === 'string') {
        try {
            changesByDate = JSON.parse(changesByDate);
            console.log('Повторно распарсили строку:', changesByDate);
        } catch (e) {
            console.error('Не удалось распарсить строку:', changesByDate);
            changesByDate = {};
        }
    }

    // Проверяем, что changesByDate - это объект
    if (!changesByDate || typeof changesByDate !== 'object' || Array.isArray(changesByDate)) {
        console.error('changesByDate не является объектом:', changesByDate);
        // Создаем пустой объект как запасной вариант
        changesByDate = {};
    }

    // Получаем даты и значения
    const dates = Object.keys(changesByDate).sort();
    const values = dates.map(date => changesByDate[date]);

    console.log('Dates:', dates);
    console.log('Values:', values);

    const ctx = document.getElementById('statsChart').getContext('2d');

    // Уничтожаем старый график если есть
    if (window.myChart) {
        window.myChart.destroy();
    }

    if (dates.length === 0) {
        // Показываем сообщение о пустых данных
        window.myChart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: ['Нет данных за период'],
                datasets: [{
                    label: 'Количество изменений',
                    data: [0],
                    borderColor: '#999',
                    backgroundColor: 'rgba(150,150,150,0.1)'
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                    title: {
                        display: true,
                        text: 'Нет данных за выбранный период'
                    }
                }
            }
        });
        return;
    }

    // Форматируем даты для отображения (ДД.ММ)
    const labels = dates.map(date => {
        const parts = date.split('-');
        return parts.length === 3 ? `${parts[2]}.${parts[1]}` : date;
    });

    // Создаем новый график
    window.myChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: labels,
            datasets: [{
                label: 'Количество изменений',
                data: values,
                borderColor: '#0d6efd',
                backgroundColor: 'rgba(13,110,253,0.1)',
                borderWidth: 2,
                pointBackgroundColor: '#0d6efd',
                pointBorderColor: '#fff',
                pointRadius: 4,
                pointHoverRadius: 6,
                tension: 0.2,
                fill: true
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            plugins: {
                legend: {
                    display: false
                },
                tooltip: {
                    callbacks: {
                        label: function(context) {
                            return `Изменений: ${context.raw}`;
                        }
                    }
                }
            },
            scales: {
                y: {
                    beginAtZero: true,
                    ticks: {
                        stepSize: 1,
                        precision: 0
                    }
                }
            }
        }
    });

    console.log('Chart created successfully');
}

// Функционал изменения размера колонок как в Excel
function makeResizable(table) {
    const cols = table.querySelectorAll('th');
    let currentResize;

    cols.forEach(col => {
        const handle = document.createElement('div');
        handle.className = 'resize-handle';
        col.appendChild(handle);

        handle.addEventListener('mousedown', function(e) {
            e.preventDefault();
            currentResize = {
                col: col,
                startX: e.pageX,
                startWidth: col.offsetWidth
            };

            document.body.classList.add('resizing');

            document.addEventListener('mousemove', onMouseMove);
            document.addEventListener('mouseup', onMouseUp);
        });
    });

    function onMouseMove(e) {
        if (!currentResize) return;

        const diff = e.pageX - currentResize.startX;
        const newWidth = Math.max(50, currentResize.startWidth + diff);

        // Устанавливаем новую ширину
        currentResize.col.style.width = newWidth + 'px';

        // Обновляем ширину всех ячеек в колонке
        const index = currentResize.col.cellIndex;
        const rows = table.querySelectorAll('tbody tr');
        rows.forEach(row => {
            const cell = row.cells[index];
            if (cell) {
                cell.style.width = newWidth + 'px';
            }
        });
    }

    function onMouseUp() {
        document.body.classList.remove('resizing');
        document.removeEventListener('mousemove', onMouseMove);
        document.removeEventListener('mouseup', onMouseUp);
        currentResize = null;
    }
}

// Добавьте в history.js
document.addEventListener('DOMContentLoaded', function() {
    // Обрезаем SHA-256 хэши до формата "abcd...wxyz"
    document.querySelectorAll('.history-hash').forEach(el => {
        const fullHash = el.textContent.trim();
        if (fullHash && fullHash.length > 20) {
            const shortHash = fullHash.substring(0, 8) + '...' + fullHash.substring(fullHash.length - 8);
            el.textContent = shortHash;
            el.setAttribute('data-full-hash', fullHash);

            // Добавляем тултип с полным хэшем
            el.setAttribute('title', 'Полный хэш: ' + fullHash);
        }
    });

    // Для длинных путей
    document.querySelectorAll('.history-file-path').forEach(el => {
        const fullPath = el.textContent.trim();
        if (fullPath && fullPath.length > 40) {
            const pathParts = fullPath.split('/');
            const fileName = pathParts.pop() || '';
            const shortPath = '.../' + fileName;
            el.textContent = shortPath;
            el.setAttribute('data-full-path', fullPath);
            el.setAttribute('title', fullPath);
        }
    });
});

// В history.js
function shortenPathStart(path, maxLength = 40) {
    if (!path || path.length <= maxLength) return path;

    const fileName = path.split('/').pop() || path;
    const pathWithoutFile = path.substring(0, path.length - fileName.length);

    if (fileName.length > maxLength - 3) {
        return '...' + fileName.substring(fileName.length - (maxLength - 3));
    }

    const availableForPath = maxLength - fileName.length - 3;
    const shortenedPath = '...' + pathWithoutFile.substring(pathWithoutFile.length - availableForPath);

    return shortenedPath + fileName;
}

document.addEventListener('DOMContentLoaded', function() {
    document.querySelectorAll('.path-content').forEach(el => {
        const fullPath = el.textContent.trim();
        el.textContent = shortenPathStart(fullPath, 40);
    });
});

// Инициализация при загрузке
document.addEventListener('DOMContentLoaded', function() {
    const table = document.querySelector('.history-table');
    if (table) {
        makeResizable(table);
    }
});

// Запускаем после загрузки DOM
document.addEventListener('DOMContentLoaded', function() {
    console.log('DOM loaded');
    initChart();
});