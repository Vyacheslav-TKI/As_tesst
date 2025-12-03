document.getElementById('authForm').addEventListener('submit', async (e) => {
  e.preventDefault();

  const login = document.getElementById('login').value.trim();
  const token = document.getElementById('token').value.trim();
  const errorMsg = document.getElementById('errorMsg');

  if (!login || !token) {
    showError('Заполните все поля');
    return;
  }

  try {
    const response = await fetch('/api/auth', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json'
      },
      body: JSON.stringify({ login, token })
    });

    const data = await response.json();

    if (data.code === 200) {
      // Сохраняем session_id (например, в sessionStorage или cookie через бэкенд)
      // В простейшем случае — просто редирект
      window.location.href = '/'; // Главная страница
    } else {
      showError(data.answ || 'Ошибка авторизации');
    }
  } catch (err) {
    showError('Не удалось связаться с сервером');
  }
});

function showError(message) {
  const errorMsg = document.getElementById('errorMsg');
  errorMsg.textContent = message;
  errorMsg.style.display = 'block';
}