# ЦЕЛКОН – CLI клиент

Тонкий клиент, который подключается к демону ЦЕЛКОН (C) через TLS
и выполняет команды:

- login
- list files
- add file
- delete file
- update hash algorithm
- get statistics

Фронтенд **ничего не хранит**, полностью работает как транслятор
пользовательских команд в протокол демона.

## Запуск
mvn package
java -jar target/celkon-cli.jar


## Настройка TLS

В корне должен лежать `truststore.jks`.
