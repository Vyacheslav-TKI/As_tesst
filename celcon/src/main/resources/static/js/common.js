
function showProfileInfo() {
    const menu = document.getElementById('profileInfoMenu');
    menu.style.display = menu.style.display === 'flex' ? 'none' : 'flex';
}
function exitProfile() {
    fetch('/api/logout', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' }
    })
    .then(res => res.json())
    .then(data => {
        if (data.redirectUrl) {
            window.location.href = data.redirectUrl;
        }
    })
    .catch(err => {
        alert('Ошибка при выходе');
    });
}

// Закрыть профиль при клике вне
document.addEventListener('click', (e) => {
    const menu = document.getElementById('profileInfoMenu');
    const btn = document.querySelector('.myprofile-btn');
    if (menu && btn && !menu.contains(e.target) && e.target !== btn) {
        menu.style.display = 'none';
    }
});

// Общие функции для всех страниц

// Функция открытия модалки отчета
window.openReportModal = function() {
    const modal = document.getElementById('reportModal');
    if (modal) {
        modal.style.display = 'flex';
        // Сбрасываем поля при открытии
        document.getElementById('rep-from').value = '';
        document.getElementById('rep-to').value = '';
    } else {
        console.error('Модальное окно отчета не найдено');
    }
};

// Функция закрытия модалки отчета
window.closeReportModal = function() {
    const modal = document.getElementById('reportModal');
    if (modal) {
        modal.style.display = 'none';
    }
};

// Функция выбора всех файлов
window.toggleAllFiles = function() {
    const selectAllCheckbox = document.getElementById('select-all');
    if (!selectAllCheckbox) return;

    const checked = selectAllCheckbox.checked;
    document.querySelectorAll('.file-checkbox').forEach(cb => cb.checked = checked);
};

// Функция генерации отчета
window.generateReport = function() {
    const from = document.getElementById('rep-from').value;
    const to = document.getElementById('rep-to').value;

    if (!from || !to) {
        alert('Укажите период');
        return;
    }

    const selected = Array.from(document.querySelectorAll('.file-checkbox:checked'))
        .map(cb => cb.value);

    if (selected.length === 0) {
        alert('Выберите хотя бы один файл');
        return;
    }

    const format = document.querySelector('input[name="format"]:checked')?.value || 'txt';

    // Здесь будет реальная отправка на сервер
    console.log(`Отправка запроса на генерацию отчета:`, {
        period: { from, to },
        files: selected,
        format: format
    });

    alert(`Отчёт (${format.toUpperCase()}) за ${from}–${to}, файлов: ${selected.length}`);
    closeReportModal(); // Используем функцию закрытия
};

// Инициализация при загрузке страницы
document.addEventListener('DOMContentLoaded', function() {
    // Закрытие модалки по клику вне её области
    const modal = document.getElementById('reportModal');
    if (modal) {
        modal.addEventListener('click', function(e) {
            if (e.target === modal) {
                closeReportModal();
            }
        });
    }

    // Закрытие по Escape
    document.addEventListener('keydown', function(e) {
        if (e.key === 'Escape') {
            const modal = document.getElementById('reportModal');
            if (modal && modal.style.display === 'flex') {
                closeReportModal();
            }
        }
    });
});