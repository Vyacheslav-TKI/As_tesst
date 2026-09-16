Инструкция по сборке:

1) Клонируем git-репозиторий
2) Переходим в каталог daemon
    cd daemon
3) Создаём папку build:
    mkdir build
4) cmake ..
5) make

Генерируем сертификат:

6) sudo mkdir -p /etc/celcon
7) sudo openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
    -keyout /etc/celcon/key.pem \
    -out /etc/celcon/cert.pem \
    -subj "/C=RU/ST=Astra/O=CELCON/CN=localhost"

8) После этого можем запускать:
    sudo ./celcond

9) В случае успешного запуска демона в терминале появится сообщение:
    Celcon daemon started at PID = 22900

    (PID может быть любым другим числом)

10) По-умолчанию (только для теста) соединение будет через порт 9999
