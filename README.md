# server-client_multimetr

Cервер и клиент, имитирующие удаленное управление прибором.\
Прибор - универсальный многоканальный мультиметр (2..n каналов измерения напряжения).\
### Архитектура

Операционная система - linux.
Сервер - приложение, реализующая непосредственное управление аппаратными средствами прибора.
Клиент - приложение Qt , реализующее управление сервером и графический интерфес
пользователя на базе qml/qtquick 2.15 . Клиент и сервер взаимодействуют с
помощью socket unix domain .

### Протокол 

Текстовый протокол. Запрос и ответ - строка заказчивающаяся CR.\
Формат команд:\
"command-name parameter1, parameter2, ... parameterN\n" \
Формат ответа: \
"ok|fail, result1, result2, ... resultN\n" \
Например: \
запрос: "set_range channel0, range0". \
ответ : "ok, range0". \

### Команды управления

|Команда|Описание|Параметры|Возвращаемый результат|
|-|-|-|-|
|start_measure|запуск измерения|channel|ok/fail|
|set_range|настроить диапазон|channel, range|ok/fail, range|
|stop_measure|останов измерения|channel|ok/fail|
|get_status|получение статуса канала/устройства|channel|ok/fail, Код состояния - error_state/idle_state/measure_state/busy_state;|
|get_result|получения результата измерения|channel|ok/fail, result1, ... resultN|

Количество каналов прибора - N , задается на стадии компиляции, N >= 2. Канал\
задается в в виде строки channel[целое число от 0 до N-1] , например, "channel0",\
"channel1", "channel7". Диапазон канала измерения - Строка вида range[целое число от 0\
до M-1] , где M равно 4. Например "range0", "range1", "range2", "range3". Сервер\
возвращает значения канала только в диапазоне данного канала. Использовать следующие\
параметры для канала: range0: соответствует диапазону [0.0000001 ... 0.001) В range1:\
соответствует диапазону [0.001 ... 1) В range2: соответствует диапазону [1 ... 1000) В\
range3: соответствует диапазону [1000 ... 1000000) В\
Тип результата - текущее значение напряжения канала - float . Каналы работают\
независимо, возможно запускать и останавливать работу отдельного канала и получать\
статус. Коды состояния канала прибора: error_state - ошибка работы канала, должна\
быть сгенерирована в случайный момент времени при работе с сервером, клиент должен\
адекватно обработать состояние и индицировать его; idle_state - состояние готовности\
канала к измерениям; measure_state - состояние измерения канала; busy_state -\
состояние занятости канала к получению данных, генерируется случайным образом в\
процессе взаимодействия.\

Используемый стандарт языка с++17. Библиотека Qt версия 5.15.

### Инструкция по сборке:

Установите [Qt 5.15.2](https://github.com/qt/qt5/tree/5.15.2) (возможно потребуется поправить исходники, на момент сборки проекта в них находились ошибки)
Клонируйте проект и соберите оба приложения в [Qt creator](https://github.com/qt-creator/qt-creator?ysclid=lfjfvafimd531962902)
