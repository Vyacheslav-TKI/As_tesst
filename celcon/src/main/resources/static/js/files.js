let ws;
let daemonSessionId = null; // Этот ID должен быть получен после аутентификации

function connectWebSocket() {
    ws = new WebSocket('ws://localhost:8080/events');

    ws.onopen = () => {
        console.log('WebSocket connected');
        // После получения session_id от демона (после auth) регистрируем его
        if (daemonSessionId) {
            ws.send('register:' + daemonSessionId);
        }
    };

    ws.onmessage = (event) => {
        console.log('WebSocket message:', event.data);
        try {
            const msg = JSON.parse(event.data);
            if (msg.event === "FILE_CHANGED") {
                console.log('File changed event:', msg);
                if (msg.id) {
                    updateFileStatus(msg.id, "changed");
                    //alert(`Файл изменён: ${msg.path}`);
                }
            } else if (msg.event === "FILE_UNCHANGED") {
                console.log('File changed event:', msg);
                if (msg.id) {
                    updateFileStatus(msg.id, "unchanged");
                    //alert(`Файл изменён: ${msg.path}`);
                }
            }
            else if (msg.event === "SESSION_EXPIRED") {
                alert("Сессия истекла");
                window.location.href = "/auth";
            }
        } catch (e) {
            console.error('Error parsing WebSocket message:', e);
        }
    };

    ws.onclose = () => {
        console.log('WebSocket disconnected');
        // Попытка переподключения через 5 секунд
        setTimeout(connectWebSocket, 5000);
    };

    ws.onerror = (error) => {
        console.error('WebSocket error:', error);
    };
}

// Вызываем после успешной аутентификации
function setDaemonSessionId(sessionId) {
    daemonSessionId = sessionId;
    if (ws && ws.readyState === WebSocket.OPEN) {
        ws.send('register:' + sessionId);
    }
}

// Запускаем при загрузке страницы
document.addEventListener('DOMContentLoaded', connectWebSocket);

// ========== Утилиты ==========
function showEl(id) {
    document.getElementById(id).style.display = 'flex';
}

function hideEl(id) {
    document.getElementById(id).style.display = 'none';
}

// ========== Вкладки ==========
document.addEventListener('DOMContentLoaded', () => {
    // Переключение вкладок
    document.querySelectorAll('.tab-btn').forEach(button => {
        button.addEventListener('click', () => {
            // Убираем active у всех кнопок и контентов
            document.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active'));
            document.querySelectorAll('.tab-content').forEach(c => c.classList.remove('active'));

            // Активируем нужные
            button.classList.add('active');
            const tabId = button.getAttribute('data-tab') + '-tab';
            const tabContent = document.getElementById(tabId);
            if (tabContent) {
                tabContent.classList.add('active');
            }
        });
    });
    // ========== Выпадающие меню (⋮) ==========
    document.addEventListener('click', (e) => {
        // Скрыть все меню
        document.querySelectorAll('.dropdown-menu').forEach(menu => menu.classList.remove('show'));

        // Если клик по кнопке "⋮"
        if (e.target.classList.contains('menu-btn')) {
            e.stopPropagation();
            const menu = e.target.nextElementSibling;
            if (menu && menu.classList.contains('dropdown-menu')) {
                menu.classList.add('show');
            }
        }
    });

    // Закрытие меню при клике вне
    document.addEventListener('click', (e) => {
        if (!e.target.closest('.dropdown-menu') && !e.target.classList.contains('menu-btn')) {
            document.querySelectorAll('.dropdown-menu').forEach(m => m.classList.remove('show'));
        }
    });

    // Обработка действий в выпадающем меню
    document.addEventListener('click', (e) => {
        if (e.target.classList.contains('dropdown-item')) {
            const action = e.target.textContent.trim();
            const dataAttr = e.target.parentElement.parentElement.classList.contains('file-controls')
                ? 'data-filename'
                : 'data-user-id';
            const target = e.target.closest('.dropdown-menu').previousElementSibling;
            const value = target ? target.textContent.trim() : 'неизвестно';

            if (action === 'Удалить') {
                if (confirm(`Удалить "${value}"?`)) {
                    alert('Удаление пока не реализовано (нужен ID и API)');
                }
            } else if (action === 'Изменить') {
                alert('Изменение пока не реализовано');
            }
        }
    });

    // ========== Модальные окна ==========
    // Внутри DOMContentLoaded — НЕТ!
    // Вместо этого — присваивай глобальным свойствам window:

    window.openAddFileModal = function() {
        document.getElementById('addFileModal').style.display = 'flex';
    };

    window.closeAddFileModal = function() {
        document.getElementById('addFileModal').style.display = 'none';
    };

    window.openReportModal = function() {
        const today = new Date().toISOString().split('T')[0];
        const weekAgo = new Date(Date.now() - 7 * 24 * 60 * 60 * 1000).toISOString().split('T')[0];
        document.getElementById('rep-from').value = weekAgo;
        document.getElementById('rep-to').value = today;
        document.getElementById('reportModal').style.display = 'flex';
    };

    window.closeReportModal = function() {
        document.getElementById('reportModal').style.display = 'none';
    };

    window.onclick = (e) => {
        if (e.target.classList.contains('modal')) {
            hideEl(e.target.id);
        }
    };

    // ========== Добавление файла ==========
    window.confirmAddFile = function () {
        const path = document.getElementById('filePath').value.trim();
        if (!path) {
            alert('Укажите путь к файлу или папке');
            return;
        }

        const algMap = { md5: 0, sha1: 1, sha256: 2 };
        const alg = document.querySelector('input[name="format"]:checked').value;
        const algNum = algMap[alg] ?? 0;

        const file = {
            path: path,
            alg: algNum,
            hash: "",
            for_users: "0,1,2"
        };

        fetch('/api/files/add', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ files: [file] })
        })
        .then(res => res.json())
        .then(data => {
            if (data.code === 200) {
                alert('Файл добавлен');
                hideEl('addFileModal');
                location.reload();
            } else {
                alert('Ошибка: ' + data.answ);
            }
        })
        .catch(err => {
            console.error(err);
            alert('Не удалось добавить файл');
        });
    };
});

function updateFileStatus(fileId, newStatus) { // newStatus: 'pending', 'changed', 'unchanged'
    const statusElement = document.getElementById(`status-${fileId}`);

    if (statusElement) {
        // Обновляем CSS классы
        statusElement.classList.remove('pending', 'changed', 'unchanged');
        statusElement.classList.add(newStatus);

        // Обновляем data атрибут
        statusElement.dataset.status = newStatus;

        // Обновляем текст
        const statusText = {
            'pending': 'ПРОВЕРЯЕТСЯ',
            'changed': 'ИЗМЕНЁН',
            'unchanged': 'НЕ ИЗМЕНЁН'
        };
        statusElement.textContent = statusText[newStatus];
    }
}