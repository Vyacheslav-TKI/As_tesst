// ========== Модальные окна ==========
function openAddUserModal() {
    document.getElementById('addUserModal').style.display = 'flex';
}
function closeAddUserModal() {
    document.getElementById('addUserModal').style.display = 'none';
}

// ========== Профиль ==========
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
function deleteUser(id) {
    if (confirm('Удалить пользователя? Это действие нельзя отменить.')) {
        alert('Удаление ' + id + ' — заглушка');
    }
}

function confirmAddUser() {
    const fio = document.getElementById('userFio').value.trim();
    const post = document.getElementById('userPost').value.trim();
    const role = document.getElementById('userRole').value;

    if (!fio || !post) {
        alert('Заполните все поля');
        return;
    }

    // В будущем: отправка на /api/users/add
    alert(`Пользователь "${fio}" будет добавлен с ролью ${role}`);
    closeAddUserModal();
}

// Закрытие модалок по клику на фон
window.onclick = (e) => {
    if (e.target.classList.contains('modal')) {
        e.target.style.display = 'none';
    }
};