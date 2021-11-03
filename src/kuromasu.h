#pragma once//ver 1.5
#define _CRT_SECURE_NO_WARNINGS
/*
* подключаемые заголовочные файлы
*/

#include <stdio.h>
#include <locale.h>
#include <math.h>
#include <malloc.h>

#include <stdlib.h>
#include <Windows.h>
#include <string.h>
#include <dirent.h>

/*
* константы для настройки работы приложения
*/

#define WHITE_CELL 0
#define BLACK_CELL -1
#define LIFE_COUNT 3
#define MAX_FILES_IN_DIR 20
#define MIN_FIELD_SIZE 4
#define MAX_FIELD_SIZE 10
#define FILES_ON_PAGE 3
#define clear_frame(void) system("cls")

/*
* константы для отрисовки баннеров
*/

#define WIN_LABEL "\t=====  |   |      |===|  |   |  |  /|  |===  |===|    \\      \\ \n\t  |    |=| |      |--/   |=| |  | / |  |     |__-|   /_\\    / \\ \n\t  |    |_| |      |___|  |_| |  |/  |  |     |      /   \\  /   \\ \n"
#define LOSE_LABEL  "\t=====  |   |      |===|  |===|  |===|  |  /|  |===  |===|    \\      \\ \n\t  |    |=| |      |   |  |__-|  |   |  | / |  |     |__-|   /_\\    / \\\n\t  |    |_| |      |   |  |      |___|  |/  |  |     |      /   \\  /   \\ \n"
#define NAME_LABEL  "\t\t| /  \\  /   |==|  |==|  |\\ /|    \\    |==|  \\  /\n\t\t|\\     /    |__|  |  |  | \\ |   /_\\   |       /\n\t\t| \\   /     |	  |__|  |   |  /   \\  |__|   /"

/*
* создание типа KEY_CODE для работы с клавишами
* значения присваиваются в соответствии с кодами клавиатурных символов
*/

typedef enum
{
	key_a = 97,
	key_d = 100,
	key_s = 115,
	key_w = 119,
	key_space = 32,
	key_esc = 27
} KEY_CODE;

/*
* определение типа color_t для маркировки цветов, которые используются при отрисовки интерфейса
* значения соответствуют кодам цветов.
*/

typedef enum
{
	RED = 64,
	GREEN = 32,
	DEFAULT = 7,
	SELECT = 112,
	MIX = 95,
} color_t;

/*
* тип gstate_t необходим для работы с текущим внутриигровым состоянием:
*	STATE_RUNNING - обычное состояние
*	STATE_LOSE - пользователь допустил ошибку во время игры
*	STATE_WIN - пользователь заполнил поле правильно
*/
typedef enum { STATE_RUNNING, STATE_WIN, STATE_LOSE } gstate_t;

/*
* тип kaction_t необходим для обработки состояний нажатых клавишь пользователем:
*	INPUT_ERROR - пользователь нажал не верную клавишу
*	INPUT_NORMALLY - пользователь нажал клавишу для смены положение курсора
*	INPUT_ACCEPT - пользователь нажал клавишу для потверждения
*	INPUT_BACK - пользователь нажал клавишу "возврата"
*/
typedef enum { INPUT_ERROR, INPUT_NORMALLY, INPUT_ACCEPT, INPUT_BACK } kaction_t;
/*
* тип fupdate_t предназначен для обработки состояния отрисовки интерфейса
*	FRAME_CONTINUE - отрисовка продолжается
*	FRAME_RETURN - вернуться в начало отрисовки
*	FRAME_EXIT - выйти из отрисовки данного кадра и вернуть значения
*/
typedef enum { FRAME_CONTINUE, FRAME_RETURN, FRAME_EXIT } fupdate_t;

typedef char name_t[261]; //строковый тип определенный для работы с названиями
typedef _Bool bool; //опредение булевого типа 

/*
* структура gupdate_t определяет состояние и действие для текущего кадра
*	frame_update_state - для хранения состояния
*	return_value - указатель на возвращаемое значение
*/

typedef struct gupdate_t
{
	fupdate_t frame_update_state;
	void* return_value;
} gupdate_t;
gupdate_t gupdate_c(fupdate_t frame_update_state, void* return_value); // конструктор для структуры gupdate_t


/*
* структура tuple_t хранение двух значений (координат)
*/

typedef struct tuple_t { int x, y; } tuple_t;
tuple_t tuple_c(int x, int y); // конструктор для структуры tuple_t

/*
* структура cell_t описывает свойства клетки игрового поля
*	free_value - кортеж для хранение видимых клеток по двум осям
*	check_value - значение клетки
*	color - цвет для отрисовки
*/

typedef struct cell_t
{
	tuple_t free_value;
	int check_value;
	color_t color;
} cell_t;
cell_t cell_c(int check, int free, color_t color);// конструктор для структуры cell_t

/*
* структура dir_t описывает значения и размер массива строк
*	array - массив строк
*	size - размер массива
*/

typedef struct dir_t
{
	name_t* array;
	int size;
} dir_t;
dir_t dir_c(name_t* dir, int size); // конструктор для структуры dir_t

/*
* структура field_t описывает игровое поле
*	array - массив клеток типа cell_t
*	size - размер поля (массива)
*	name - название поля
*/

typedef struct field_t
{
	cell_t* array;
	name_t name;
	int size;
}field_t;
field_t field_c(cell_t* array, name_t name, int size); // конструктор для структуры field_t

/*
* определение типа указателя на функцию, вызываемой при отрисовки кадра  
* функция - возвращает состояние fupdate_t и указатель на переменную
* args - указатель входное значение
* action - пользовательская команда
* pos - положение курсора 
*/
typedef gupdate_t(*update_action_function)(void* args, kaction_t action, tuple_t pos);

/*
* функция set_line - пересчет значений выбранной оси
* dir - коэфициент, характеризующий направление движения по полю
* last - указатель на начальный индекс
* break_point - индекс клекти линии где сработало истключение
* cell - указатель на начальную клетку
*/
void set_line(int dir, int* last, int i, int* cell); 

/*
* функция draw_list - отрисовывает список
* cursor - индекс выбранного пункта
* param - список для отрисовки
* begin - индекс первого объекта для отрисовкаparam[]
* end - индекс последнего объекта для отрисовка
*/
void draw_list(int cursor, dir_t* param, int begin, int end);

/*
* функция draw_list - отрисовывает игровое поле
* pos - координаты курсора
* param - указатель на структуру, характеризующее поле
*/
void draw_field(tuple_t pos, field_t* param);

/*
* update_frame - функция открывает цикл отрисовки и сохраняет пользовательские команды 
*	возращает указатель пустого типа - результат вызова функции action, который явно преобразуется в указаетль нужного типа данных
* action - указатель на функцию, которая вызывается каждый кадр (кадр меняется при польовательском вводе) для отрисовки и обработки
*	пользовательских команд
* max - значения границ для курсора по двум осям
* back_use - true=клавиша escape будет обрабатываться в функции action; false=игнорирование нажатие
* param - указатель на значение для передачи в функцию action в качестве параметра
*/
void* update_frame(update_action_function action, tuple_t max,
	bool back_use, void* param);


/*
* print_rules - отрисовка правил игры
*/
void print_rules(void);

gupdate_t set_cell_value(void* args, kaction_t action, tuple_t pos); // значения для выбранной клетки
gupdate_t set_field_size(void* args, kaction_t action, tuple_t pos); // значение размера поля
gupdate_t set_field_values(void* args, kaction_t action, tuple_t pos); // отрисовка поля для редактирования
gupdate_t dialog_box(void* args, kaction_t action, tuple_t pos); // диалоговое окно

gupdate_t load_file(void* args, kaction_t action, tuple_t pos); // доступные поля в папке data
gupdate_t mainmenu(void* args, kaction_t action, tuple_t pos);
gupdate_t settings(void* args, kaction_t action, tuple_t pos); 
gupdate_t game_loop(void* args, kaction_t action, tuple_t pos);

/*
* функция check_field - выполняет проверку поля 
*  возвращает ошибку
*  param - указатель на игровое поле 
*/

gstate_t check_field(field_t* param);

/*
* функция get_keyboard_input - обрабатывает вводимые команды от пользователя (клавиши).
*	возращает состояние ввода: тип kaction_t 
*	
*  pos - положение курсора.
*	max - границы для курсора 
*/

kaction_t get_keyboard_input(tuple_t* pos, tuple_t max);

/*
* функция set_axies - переключает (инвертирует) значение у выбранной клетки, 
*	перезаписывает видимые клетки у кажкой клетки, которая пренадлежит линий, в которой лежит выбранная клетка.
*	возвращает ошибку.
*  pos - положение курсора 
*  ptr_param - указатель на игровое поле 
*/

bool set_axies(tuple_t pos, field_t* ptr_param);

/*
* функция file_data - считывает поле из выбранного файла или записывает из field в файл
*  возвращает ошибку
*  field - указатель, который будет хранить значения клеток поля
*	readonly - true=считавание в field из файла, false=считывание из field в файл.
*/

bool file_data(field_t* field, bool readonly);

/*
* функция read_files_in_dir - считывает все доступные файлы с игровыми полями в директории data
*  возвращает ошибку
*  param - указатель, который будет хранить название файлов
*/

bool read_files_in_dir(dir_t* param);