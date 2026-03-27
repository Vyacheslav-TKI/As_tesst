// ========== Модальные окна ==========
function openAddUserModal() {
    document.getElementById('addUserModal').style.display = 'flex';
}
function closeAddUserModal() {
    document.getElementById('addUserModal').style.display = 'none';
}

// ========== Выпадающее меню (⋮) ==========
document.addEventListener('click', (e) => {
    // Скрыть все
    document.querySelectorAll('.dropdown-menu').forEach(m => m.classList.remove('show'));

    if (e.target.classList.contains('menu-btn')) {
        e.stopPropagation();
        const menu = e.target.nextElementSibling;
        if (menu && menu.classList.contains('dropdown-menu')) {
            menu.classList.add('show');
        }
    }
});

// Закрыть при клике вне
document.addEventListener('click', (e) => {
    if (!e.target.closest('.dropdown-menu') && !e.target.classList.contains('menu-btn')) {
        document.querySelectorAll('.dropdown-menu').forEach(m => m.classList.remove('show'));
    }
});

// ========== Действия ==========
function editUser(id) {
    alert('Редактирование пользователя ' + id + ' — заглушка');
}
async function deleteUser(id) {
    if (confirm('Удалить пользователя? Это действие нельзя отменить.')) {
        // alert('Удаление ' + id + ' — заглушка');
        try {
                const response = await fetch('/api/users/delete', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json'
                    },
                    body: JSON.stringify({id})
                });

                const data = await response.json();

                if (data.code === 200) {
                    alert(`Пользователь будет удалён`);
                    closeAddUserModal();
                } else {
                    alert(data.answ || 'Ошибка добавления пользователя');
                }
            } catch (err) {
                alert('Ошибка сети или сервера: ' + err.message);
            }
    }
}

async function confirmAddUser() {
    const fio = document.getElementById('userFio').value.trim();
    const post = document.getElementById('userPost').value.trim();
    const role = Number(document.getElementById('userRole').value);
    const login = document.getElementById('userLogin').value.trim();
    const password = document.getElementById('userPassword').value;

    // Проверяем ВСЕ обязательные поля
    if (!fio || !post || !role || !login || !password) {
        alert('Заполните все поля');
        return;
    }

    try {
        const response = await fetch('/api/users/add', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({ fio, post, role, login, password })
        });

        const data = await response.json();

        if (data.code === 200) {
            alert(`Пользователь "${fio}" будет добавлен с ролью ${role}`);
            closeAddUserModal();
        } else {
            alert(data.answ || 'Ошибка добавления пользователя');
        }
    } catch (err) {
        alert('Ошибка сети или сервера: ' + err.message);
    }
    // Уберите этот alert, если хотите чтобы после успешного добавления
    // происходил redirect на /users
    // alert(`Пользователь "${fio}" будет добавлен с ролью ${role}`);
    // closeAddUserModal();
}

// Закрытие модалок по клику на фон
window.onclick = (e) => {
    if (e.target.classList.contains('modal')) {
        e.target.style.display = 'none';
    }
};