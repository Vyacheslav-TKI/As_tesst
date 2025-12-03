  // Переключение видимости меню
  document.querySelectorAll('.menu-btn').forEach(btn => {
    btn.addEventListener('click', function(e) {
      e.stopPropagation();
      const menu = this.nextElementSibling.nextElementSibling; // dropdown-menu
      document.querySelectorAll('.dropdown-menu').forEach(m => m.classList.remove('show'));
      menu.classList.toggle('show');
    });
  });

  // Закрывать меню при клике вне его
  document.addEventListener('click', () => {
    document.querySelectorAll('.dropdown-menu').forEach(m => m.classList.remove('show'));
  });
  // Переключение вкладок
  document.querySelectorAll('.tab-btn').forEach(btn => {
    btn.addEventListener('click', () => {
      document.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active'));
      document.querySelectorAll('.tab-content').forEach(c => c.classList.remove('active'));
      btn.classList.add('active');
      document.getElementById(btn.dataset.tab + '-tab').classList.add('active');
    });
  });

  // График статистики
  const ctx = document.getElementById('statsChart').getContext('2d');
  const chart = new Chart(ctx, {
    type: 'bar',
    data: {
      labels: ['10.11', '11.11', '12.11', '13.11', '14.11', '15.11', '16.11', '17.11', '18.11', '19.11', '20.11'],
      datasets: [{
        label: 'Изменённые файлы',
        data: [2, 0, 1, 3, 0, 0, 1, 2, 0, 1, 0],
        backgroundColor: '#007bff'
      }]
    },
    options: {
      responsive: true,
      plugins: {
        legend: { display: false }
      },
      scales: {
        y: { beginAtZero: true, ticks: { stepSize: 1 } }
      }
    }
  });

  function updateChart() {
    // Здесь можно обновлять данные через API
    alert('Демо: график обновлён (в реальности — запрос к бэкенду)');
  }

  function showProfileInfo() {
    const menu = document.getElementById('profileInfoMenu');

    const isVisible = menu.style.display === 'flex';

    // Закрываем все другие dropdown-меню
    document.querySelectorAll('.dropdown-menu:not(#profileInfoMenu)').forEach(m => {
        m.classList.remove('show');
    });

    // Переключаем меню профиля
    menu.style.display = isVisible ? 'none' : 'flex';
  }

  function closeProfileInfo() {
    document.getElementById('profileInfoMenu').style.display = 'none';
  }

  // Переключение видимости меню
  document.querySelectorAll('.myprofile-btn').forEach(btn => {
    btn.addEventListener('click', function(e) {
      e.stopPropagation();
      const menu = this.nextElementSibling;
      document.querySelectorAll('.profile-info-menu').forEach(m => m.classList.remove('show'));
      menu.classList.toggle('show');
    });
  });

  // Закрывать меню при клике вне его
    document.addEventListener('click', () => {
    document.querySelectorAll('.profile-info-menu').forEach(m => m.classList.remove('show'));
  });

  // Остановка всплытия для меню профиля
  document.getElementById('profileInfoMenu').addEventListener('click', (e) => {
    e.stopPropagation();
  });

  // Модальное окно отчёта
  function openReportModal() {
    document.getElementById('reportModal').style.display = 'flex';
  }

  function closeReportModal() {
    document.getElementById('reportModal').style.display = 'none';
  }

  // Модальное окно выбора файлов
  function openAddFileModal() {
    document.getElementById('addFileModal').style.display = 'flex';
  }

  function closeAddFileModal() {
    document.getElementById('addFileModal').style.display = 'none';
  }

  function selectFile() {
    var input = document.createElement('input');
    input.type = 'file';
    input.click();
  }

  function selectDirectory() {
    var input = document.createElement('input');
    input.type = 'file';
    input.webkitdirectory = true;
    input.click();
  }

  function toggleAllFiles() {
    const checked = document.getElementById('select-all').checked;
    document.querySelectorAll('#file-check-list input').forEach(cb => cb.checked = checked);
  }

  function generateReport() {
    const format = document.querySelector('input[name="format"]:checked').value;
    const files = Array.from(document.querySelectorAll('#file-check-list input:checked')).map(cb => cb.value);
    if (files.length === 0) {
      alert('Выберите хотя бы один файл');
      return;
    }
    alert(`Отчёт (${format}) сформирован для ${files.length} файлов`);
    closeReportModal();
  }

  // Закрытие модалки по клику вне
  window.onclick = (e) => {
    if (e.target === document.getElementById('reportModal')) closeReportModal();
  };

  // Переключение видимости меню
  document.querySelectorAll('.menu-btn').forEach(btn => {
    btn.addEventListener('click', function(e) {
      e.stopPropagation();
      const menu = this.nextElementSibling; // dropdown-menu
      document.querySelectorAll('.dropdown-menu').forEach(m => m.classList.remove('show'));
      menu.classList.toggle('show');
    });
  });

  // Закрывать меню при клике вне его
    document.addEventListener('click', () => {
    document.querySelectorAll('.dropdown-menu').forEach(m => m.classList.remove('show'));
  });

  // Функции действий
  function editFile() {
    alert('Редактирование — заглушка');
    // Здесь будет вызов API / открытие формы
  }

  function deleteFile() {
    if (confirm('Удалить файл из отслеживания?')) {
      alert('Удалено — заглушка');
      // Здесь — DELETE-запрос к бэкенду
    }
  }

  function exitProfile() {
    alert("Выход - заглушка")
  }
