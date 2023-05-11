# Клиент-серверное приложение Messenger

## Описание
В ходе проекта требовалось написать мессенджер с использованием языка С++. Необходимо было реализовать и клиент, и сервер. Ожидаемая топология подключения клиентов - звезда (один сервер, который знает всё и обо всех клиентах).

Реализованный набор функциональности:
*	Авторизация
*	Регистрация новых пользователей
*	Отправка и прием текстовых сообщений
*	Хранение истории сообщений
*	Вход с нескольких устройств с синхронизацией сообщений
*	Обмен любыми файлами

## Серверная часть
Сервер позволяет принимать входящие TCP-соединения и обрабатывать запросы клиентов. 
Во время запуска сервер выставляет порт и включает прослушивание по адресам, инициализирует подключение к базе данных и выгружает оттуда необходимые данные.

Сервер принимает запросы 8 видов:
*	Авторизация - проверка существования пользователя в БД.
*	Регистрация - добавление нового пользователя в БД.
*	Текстовое сообщение - сохранение сообщения в БД и рассылка всем подключенным пользователям.
*	Проверка подключения - запрос для проверки стабильности подключения.
*	История сообщений - отправка набора из 10 сообщений с требуемого индекса. 
*	Отправка файла клиенту - отправка файла с сервера пользователю.
*	Скачивание файла от клиента - скачивание и сохранение нового файла от пользователя.
*	Подписка на рассылку новых сообщений - добавить сокет в список подключенных клиентов.

Работа с базой данных осуществляется через SQL запросы. БД используется для авторизации и регистрации пользователей, просмотра истории и сохранения новых сообщений, загрузки новых и скачивания существующих файлов.
Отправка сообщений и файлов происходит через сокеты. При подключении нового клиента для него создается новый сокет. При отправке файлов между клиентом и сервером, с целью возможности передачи больших данных, включается режим скачивания и файлы передаются/принимаются по частям.

## Клиентская часть
Приложение с графическим интерфейсом на QT. Использует сокеты для подключения к серверу.
Состоит из двух экранов:
*	Авторизация
*	Мессенджер
 
Экран авторизации позволяет выполнить вход и регистрацию пользователя используя интерфейс. Данные проверяются на корректность, затем отправляется соответствующий запрос на сервер.
Экран с мессенджером позволяет получать и отправлять сообщения. Сообщения могут быть с текстом или файлом, а также указан создатель сообщения и время создания. Прикрепленные к сообщению файлы можно скачать на компьютер.
Для взаимодействия с сервером создается 2 сокета, первый для отправки и получения текстовых сообщений, а второй для отправки и скачивания файлов. Это сделано, чтобы пользователь мог продолжать использовать чат во время загрузки больших файлов. Также есть возможность загрузить историю сообщений.
  
## База данных:
Для хранения и обработки информации в структурированном виде используется встроенная реляционная база данных SQLite.
Таблицы в БД:
*	Users - хранит информацию о пользователях в системе (логин, пароль).
*	Messenges - хранит историю сообщений (id, логин создателя, текст сообщения, время создания, тип сообщения, id прикрепленного файла)
*	Files - хранит отправленные пользователями файлы (id файла, название и расширение файла, файл в формате массива байтов)

## Диаграмма классов
...
