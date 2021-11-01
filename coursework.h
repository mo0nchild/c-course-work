#pragma once//ver 1.5

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

const char* WIN_LABEL = "\t=====  |   |      |===|  |   |  |  /|  |===  |===|    \\      \\ \n\t  |    |=| |      |--/   |=| |  | / |  |     |__-|   /_\\    / \\ \n\t  |    |_| |      |___|  |_| |  |/  |  |     |      /   \\  /   \\ \n";
const char* LOSE_LABEL = "\t=====  |   |      |===|  |===|  |===|  |  /|  |===  |===|    \\      \\ \n\t  |    |=| |      |   |  |__-|  |   |  | / |  |     |__-|   /_\\    / \\\n\t  |    |_| |      |   |  |      |___|  |/  |  |     |      /   \\  /   \\ \n";
const char* NAME_LABEL = "\t\t| /  \\  /   |==|  |==|  |\\ /|    \\    |==|  \\  /\n\t\t|\\     /    |__|  |  |  | \\ |   /_\\   |       /\n\t\t| \\   /     |	  |__|  |   |  /   \\  |__|   /";

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

typedef struct
{
	fupdate_t frame_update_state;
	void* return_value;
} gupdate_t;
gupdate_t gupdate_c(fupdate_t frame_update_state, void* return_value) // конструктор для структуры gupdate_t
{
	gupdate_t constructor = { frame_update_state, return_value };
	return constructor;
}

/*
* структура tuple_t хранение двух значений (координат)
*/

typedef struct { int x, y; } tuple_t;
tuple_t tuple_c(int x, int y) // конструктор для структуры tuple_t
{
	tuple_t constructor = { x, y };
	return constructor;
}

/*
* структура cell_t описывает свойства клетки игрового поля
*	free_value - кортеж для хранение видимых клеток по двум осям
*	check_value - значение клетки
*	color - цвет для отрисовки
*/

typedef struct
{
	tuple_t free_value;
	int check_value;
	color_t color;
} cell_t;
cell_t cell_c(int check, int free, color_t color)// конструктор для структуры cell_t
{
	cell_t constructor = { tuple_c(free, free), check ,color };
	return constructor;
}

/*
* структура dir_t описывает значения и размер массива строк
*	array - массив строк
*	size - размер массива
*/

typedef struct
{
	name_t* array;
	int size;
} dir_t;
dir_t dir_c(name_t* dir, int size) // конструктор для структуры dir_t
{
	dir_t constructor = { dir, size };
	return constructor;
}

/*
* структура field_t описывает игровое поле
*	array - массив клеток типа cell_t
*	size - размер поля (массива)
*	name - название поля
*/

typedef struct
{
	cell_t* array;
	name_t name;
	int size;
}field_t;
field_t field_c(cell_t* array, name_t name, int size) // конструктор для структуры field_t
{
	field_t constructor = { array, name, size };
	return constructor;
}

// определение типа функции, вызываемой при отрисовки кадра 
typedef gupdate_t(*update_action_function)(void* args[], kaction_t action, tuple_t pos);

void set_line(int dir, int* last, int i, int* cell);
void draw_list(int cursor, dir_t* param, int begin, int end);
void draw_field(tuple_t pos, field_t* param);

void* update_frame(update_action_function action, tuple_t max,
	bool back_use, void* param[]);

void print_rules(void);
void start_app(void);

gupdate_t set_cell_value(void* args[], kaction_t action, tuple_t pos);
gupdate_t set_field_size(void* args[], kaction_t action, tuple_t pos);
gupdate_t set_field_values(void* args[], kaction_t action, tuple_t pos);
gupdate_t dialog_box(void* args[], kaction_t action, tuple_t pos);

gupdate_t load_file(void* args[], kaction_t action, tuple_t pos);
gupdate_t mainmenu(void* args[], kaction_t action, tuple_t pos);
gupdate_t settings(void* args[], kaction_t action, tuple_t pos);
gupdate_t game_loop(void* args[], kaction_t action, tuple_t pos);

gstate_t check_field(field_t* param);
kaction_t get_keyboard_input(tuple_t* pos, tuple_t max);

bool set_axies(tuple_t pos, field_t* ptr_param);
bool file_data(field_t* field, bool readonly);
bool read_files_in_dir(dir_t* param);

void* update_frame(update_action_function action, tuple_t max,
	bool back_use, void* param[])
{
	kaction_t input = INPUT_NORMALLY;
	tuple_t pos = tuple_c(0, 0);

	while (1)
	{
		switch (input)
		{
		case INPUT_BACK: if (!back_use) break;
		case INPUT_ACCEPT:
		case INPUT_NORMALLY:

			clear_frame();
			printf("\n[ W A S D: ПРЕМЕСТИТЬ КУРСОР ] [ SPACE: ПОДТВЕРДИТЬ ] [ %18.18s ]\n",
				(back_use) ? "ESCAPE: ВЕРНУТЬСЯ" : "==================");

			gupdate_t update = action(param, input, pos);
			if (update.frame_update_state || input == INPUT_ACCEPT)
			{
				input = INPUT_NORMALLY;
				if (update.frame_update_state == FRAME_EXIT) return update.return_value;
				else continue;
			}
		case INPUT_ERROR: break;
		}
		input = get_keyboard_input(&pos, max);
	}
}

void draw_list(int cursor, dir_t* param, int begin, int end)
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	for (int i = begin; i < end; i++)
	{
		if (cursor == i)SetConsoleTextAttribute(console, SELECT);
		printf("\n\t\t\t[ %20.20s ]\n", i < param->size ? *(param->array + i) : "ПУСТО");
		SetConsoleTextAttribute(console, DEFAULT);
	}
}

gupdate_t set_cell_value(void* args[], kaction_t action, tuple_t pos)
{
	int* field_size = (int*)args;
	if (action == INPUT_ACCEPT)
	{
		int value = pos.x;
		return gupdate_c(FRAME_EXIT, &value);
	}
	else
	{
		printf("\n\n\n\n\t\t[ ЗНАЧЕНИЕ: %c %2d %c ]", (pos.x == 0) ? ' ' : '<', pos.x,
			(pos.x == *field_size * 2 - 1) ? ' ' : '>');
	}
	return gupdate_c(FRAME_CONTINUE, NULL);
}

gupdate_t set_field_size(void* args[], kaction_t action, tuple_t pos)
{
	if (action == INPUT_ACCEPT)
	{
		field_t field;

		field.size = pos.x + MIN_FIELD_SIZE;
		field.array = (cell_t*)calloc(pow(field.size, 2), sizeof(cell_t));

		for (int i = 0; i < field.size; i++)
		{
			for (int k = 0; k < field.size; k++)
				*(field.array + (i * field.size + k)) = cell_c(0, field.size, DEFAULT);
		}//------------------------------------------------------------------------------------------------------------------------------
		return gupdate_c(FRAME_EXIT, &field);
	}

	printf("\n\n\n\n\t\t[ РАЗМЕР ПОЛЯ: %c %2d %c ]", (pos.x == 0) ? ' ' : '<',
		pos.x + MIN_FIELD_SIZE, (pos.x == MAX_FIELD_SIZE - MIN_FIELD_SIZE) ? ' ' : '>');

	return gupdate_c(FRAME_CONTINUE, NULL);
}

gupdate_t dialog_box(void* args[], kaction_t action, tuple_t pos)
{
	dir_t* items = (dir_t*)args;
	if (action == INPUT_ACCEPT)
	{
		int select = (pos.y);
		return gupdate_c(FRAME_EXIT, &select);
	}

	printf("\n\n\n\t\t\t%s\n", "ЧТО ВЫ ХОТИТЕ СДЕЛАТЬ?\n\n\n");
	draw_list(pos.y, items, 0, items->size);

	return gupdate_c(FRAME_CONTINUE, NULL);
}

gupdate_t set_field_values(void* args[], kaction_t action, tuple_t pos)
{
	field_t* field = (field_t*)args;
	if (action == INPUT_ACCEPT)
	{
		(field->array + (pos.y * field->size + pos.x))->check_value =
			*(int*)update_frame(set_cell_value, tuple_c(field->size * 2, 0), FALSE, &(field->size));
	}
	else if (action == INPUT_BACK)
	{
		name_t list[] = { "ПРОДОЛЖИТЬ", "ВЫЙТИ", "СОХРАНИТЬ" };
		dir_t items = dir_c(list, 3);
		switch (*(int*)update_frame(dialog_box, tuple_c(0, 3), FALSE, &items))
		{
		case 0: break;
		case 1: return gupdate_c(FRAME_EXIT, NULL);
		case 2:
			clear_frame();
			printf("\n\n\n\t\t\t[ ВВЕДИТЕ НАЗВАНИЕ ]: ");
			scanf("%s", field->name);
			return gupdate_c(FRAME_EXIT, field);
		}
	}

	draw_field(pos, field);

	return gupdate_c(FRAME_CONTINUE, NULL);
}

void print_rules(void)
{
	clear_frame();
	printf("\n\t\tКуромасу играется на прямоугольной сетке. \n\t\tВ некоторых из этих ячеек есть числа. \n\t\tКаждая ячейка может быть черной или белой. \n\t\tЦель состоит в том, чтобы определить, \n\t\tк какому типу относится каждая ячейка.\n\t\tСледующие правила определяют, какие ячейки какие :\n\n\t\t\t1)Каждое число на доске представляет \n\t\tколичество белых клеток, которые можно увидеть из этой клетки,\n\t\t включая ее самого.Ячейку можно увидеть из другой ячейки,\n\t\t если они находятся в той же строке или столбце,\n\t\t и между ними нет черных ячеек в этой строке или столбце.\n\t\t\t2)Пронумерованные ячейки не могут быть черными.\n\t\t\t3)Никакие две черные клетки не могут быть смежными \n\t\tпо горизонтали или вертикали.\n\t\t\t4)Все белые клетки должны быть соединены горизонтально \n\t\tили вертикально.\n");
	printf("\n\t\t\t\tНАЖМИТЕ КЛАВИШУ ДЛЯ ПРОДОЛЖЕНИЯ\n\n");
	getch();
}

bool read_files_in_dir(dir_t* param)
{
	DIR* directory = opendir(".\\data");
	struct dirent* ptr_dirent_value;
	*param = dir_c((name_t*)calloc(1, sizeof(name_t)), 1);

	if (!directory) return FALSE;

	while ((ptr_dirent_value = readdir(directory)) != NULL && param->size < MAX_FILES_IN_DIR)
	{
		param->array = (name_t*)realloc(param->array, sizeof(name_t) * (param->size + 1));
		strcpy((param->array + param->size++), ptr_dirent_value->d_name);
	}
	closedir(directory);
	*param = dir_c(param->array + 3, param->size - 3);

	return TRUE;
}

gupdate_t load_file(void* args[], kaction_t action, tuple_t pos)
{
	pos.y = pos.y + pos.x * FILES_ON_PAGE;
	dir_t* param = (dir_t*)args;
	if (action == INPUT_ACCEPT && pos.y < param->size)
	{
		name_t filename;
		strcpy(filename, param->array + pos.y);
		return gupdate_c(FRAME_EXIT, filename);
	}
	else if (action == INPUT_BACK) return gupdate_c(FRAME_EXIT, NULL);
	else
	{
		draw_list(pos.y, param, FILES_ON_PAGE * pos.x, FILES_ON_PAGE * (pos.x + 1));
		printf("\n\t\t\tpage: %d/%d", pos.x + 1, (int)ceil(param->size / (double)FILES_ON_PAGE));
	}
	return gupdate_c(FRAME_CONTINUE, NULL);
}

gupdate_t settings(void* args[], kaction_t action, tuple_t pos)
{
	field_t* field = (field_t*)(args);
	if (action == INPUT_ACCEPT)
	{
		switch (pos.y)
		{
		case 0:;
			field_t edit_field = *(field_t*)update_frame(set_field_size, tuple_c(7, 0), FALSE, NULL);
			field_t* save_field = (field_t*)update_frame(set_field_values, tuple_c(edit_field.size, edit_field.size), TRUE, &edit_field);

			if (save_field != NULL)
			{
				if (!file_data(save_field, FALSE))
				{
					clear_frame();
					printf("\n\n\n\t\t\t[ НЕВОЗМОЖНО СОХРАНИТЬ ]\n");
				}
				else *field = *save_field;
			}
			break;
		case 1:;
			dir_t dir;
			if (read_files_in_dir(&dir))
			{
				name_t* filename = (name_t*)update_frame(load_file, tuple_c(ceil(dir.size / 3.f), 3), TRUE, &dir);
				if (filename != NULL)strcpy(field->name, *filename);
			}
			break;
		case 2: print_rules();  break;
		case 3: return gupdate_c(FRAME_EXIT, field);
		}
	}
	else if (action == INPUT_BACK) return gupdate_c(FRAME_EXIT, field);

	printf("\nТЕКУЩЕЕ ПОЛЕ: %s\n\n\n", field->name);

	const name_t list[] = { "СОЗДАТЬ", "ЗАГРУЗИТЬ", "ПРАВИЛА", "НАЗАД" };
	dir_t items = dir_c(list, 4);

	draw_list(pos.y, &items, 0, 4);

	return gupdate_c(FRAME_CONTINUE, NULL);
}

gupdate_t mainmenu(void* args[], kaction_t action, tuple_t pos)
{
	field_t* field = (field_t*)args;

	if (action == INPUT_ACCEPT)
	{
		switch (pos.y)
		{
		case 0:;
			bool check = file_data(field, TRUE);
			if (!check)
			{
				clear_frame();
				printf("\n\n\n\t\t\tНЕВОЗМОЖНО ЗАГРУЗИТЬ ПОЛЕ: %s\n", field->name);
				getch(); break;
			}
			bool* game_result = (bool*)update_frame(game_loop, tuple_c(field->size, field->size), TRUE, field);
			if (game_result != NULL)
			{
				bool decode = *game_result;
				clear_frame();
				printf("\n\n\n\n%s\n", decode ? WIN_LABEL : LOSE_LABEL);
				getch();
			}

			break;
		case 1:;
			*field = *(field_t*)update_frame(settings, tuple_c(0, 4), FALSE, field);
			break;
		case 2:;

			name_t list[] = { "ВЕРНУТЬСЯ", "ВЫЙТИ" };
			dir_t items = dir_c(list, 2);

			switch (*(int*)update_frame(dialog_box, tuple_c(0, 2), FALSE, &items))
			{
			case 0: return gupdate_c(FRAME_RETURN, NULL);
			case 1:return gupdate_c(FRAME_EXIT, NULL);
			}
		}
	}

	printf("\n%s\n\n\n", NAME_LABEL);

	name_t list[] = { "НАЧАТЬ", "РЕДАКТОР", "ВЫЙТИ" };
	dir_t items = dir_c(list, 3);
	draw_list(pos.y, &items, 0, 3);

	return gupdate_c(FRAME_CONTINUE, NULL);

}

void start_app(void)
{
	setlocale(LC_ALL, "rus");
	system("mkdir data");

	field_t field = { .name = "data.txt" };
	update_frame(mainmenu, tuple_c(0, 3), FALSE, &field);
}

bool file_data(field_t* field, bool readonly)
{
	name_t dirname;
	FILE* file;
	if (!readonly)strcat(field->name, ".txt");
	sprintf(dirname, "./data/%s", field->name);

	if (!(file = fopen(dirname, readonly ? "rt" : "w"))) return FALSE;

	if (readonly)
	{
		int index = 0;
		fscanf(file, "%d", &(field->size));
		field->array = (cell_t*)calloc(pow(field->size, 2), sizeof(cell_t));

		for (int i = 0; i < pow(field->size, 2); i++)
		{
			(field->array + i)->free_value.x = (field->array + i)->free_value.y = field->size;
			(field->array + i)->color = DEFAULT;
		}
		while (!feof(file))fscanf(file, "%d", &((field->array + (index++))->check_value));
	}
	else
	{
		fprintf(file, "%d\n", field->size);
		for (int i = 0; i < field->size; i++)
		{
			for (int k = 0; k < field->size; k++)
				fprintf(file, "%d\t", (field->array + (i * field->size + k))->check_value);
			fprintf(file, "\n");
		}
	}
	fclose(file);
	return TRUE;
}

kaction_t get_keyboard_input(tuple_t* pos, tuple_t max) // стрелочное управление WASD & SPACE
{
	kaction_t state = INPUT_NORMALLY;
	switch (getch())
	{
	case key_a: pos->x = (pos->x - 1 < 0 ? 0 : pos->x - 1); break;
	case key_d:
		pos->x = (pos->x + 1 > max.x - 1 ? max.x - 1 : pos->x + 1);
		break;
	case key_w: pos->y = (pos->y - 1 < 0 ? 0 : pos->y - 1); break;
	case key_s:
		pos->y = (pos->y + 1 > max.y - 1 ? max.y - 1 : pos->y + 1);
		break;
	case key_space: state = INPUT_ACCEPT; break;
	case key_esc: state = INPUT_BACK; break;
	default: state = INPUT_ERROR; break;
	}
	return state;
}

gupdate_t game_loop(void* args[], kaction_t action, tuple_t pos)
{
	static int life_counter = LIFE_COUNT;
	char* life_line = (char*)calloc(life_counter * 3 + 1, sizeof(char));
	for (int i = 0; i < life_counter; i++) strcat(life_line, "\x03  ");

	field_t* field = (field_t*)args;

	printf("\n[ ЖИЗНИ: %8.8s ]\n", life_line);
	if (action == INPUT_BACK)
	{
		name_t list[] = { "ПРОДОЛЖИТЬ", "ВЫЙТИ" };
		dir_t items = dir_c(list, 2);

		switch (*(int*)update_frame(dialog_box, tuple_c(0, 2), FALSE, &items))
		{
		case 0: return gupdate_c(FRAME_RETURN, NULL);
		case 1:
			life_counter = LIFE_COUNT;
			return gupdate_c(FRAME_EXIT, NULL);
		}
	}
	else if (action == INPUT_ACCEPT) set_axies(pos, args);

	gstate_t state = check_field(field);
	draw_field(pos, field);
	bool result = TRUE;

	switch (state)
	{
	case STATE_WIN:
		life_counter = LIFE_COUNT;
		return gupdate_c(FRAME_EXIT, &result);
	case STATE_LOSE:
		printf("\n\t\t\tНАЖМИТЕ КЛАВИШУ ДЛЯ ПРОДОЛЖЕНИЯ\n\n");
		getch();
		if (--life_counter <= 0)
		{
			result = FALSE;
			life_counter = LIFE_COUNT;
			return gupdate_c(FRAME_EXIT, &result);
		}
		set_axies(pos, field);
	case STATE_RUNNING: break;
	}
	return gupdate_c(FRAME_CONTINUE, NULL);
}

void set_line(int dir, int* last, int i, int* cell) // перезапись значений свобоных клеток выбранной оси
{
	int value = i - (*last);
	for (; (*last) < i; (*last)++, cell += (4 * dir)) *cell = value;
	(*last)++;
}

bool set_axies(tuple_t pos, field_t* ptr_param) //проверка по двум осям на пересечении x and y
{
	cell_t* cell = (ptr_param->array + (pos.y * ptr_param->size + pos.x));

	if (cell->check_value > 0 ||
		((pos.x != 0) && (cell - 1)->check_value == BLACK_CELL ||
			(pos.x != ptr_param->size - 1) && (cell + 1)->check_value == BLACK_CELL ||
			(cell + ptr_param->size)->check_value == BLACK_CELL ||
			(cell - ptr_param->size)->check_value == BLACK_CELL))
		return FALSE;

	cell->check_value = (cell->check_value == BLACK_CELL ? WHITE_CELL : BLACK_CELL);

	tuple_t last = tuple_c(0, 0);
	for (int i = 0; i <= ptr_param->size; i++)
	{
		if ((ptr_param->array + (pos.y * ptr_param->size + i))->check_value == BLACK_CELL || i == ptr_param->size)
			set_line(1, &last.x, i, &((ptr_param->array + (pos.y * ptr_param->size + last.x))->free_value.x));

		if ((ptr_param->array + (i * ptr_param->size + pos.x))->check_value == BLACK_CELL || i == ptr_param->size)
			set_line(ptr_param->size, &last.y, i, &((ptr_param->array + (last.y * ptr_param->size + pos.x))->free_value.y));
	}

	return TRUE;
}

gstate_t check_field(field_t* param)
{
	gstate_t result = STATE_RUNNING;
	bool trigger = TRUE;

	for (int i = 0; i < pow(param->size, 2); i++)
	{
		cell_t* cell = (param->array + i);
		if (cell->check_value <= 0) continue;

		if (cell->free_value.x + cell->free_value.y < cell->check_value + 1)
		{
			result = STATE_LOSE;
			cell->color = RED;
		}
		else
		{
			if (cell->free_value.x + cell->free_value.y != cell->check_value + 1)trigger = FALSE;
			else cell->color = GREEN;
		}

	}
	if (trigger) result = STATE_WIN;
	return result;
}

void draw_field(tuple_t pos, field_t* param)
{
	cell_t* cell;
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	printf("\n\n");
	for (int y = 0; y < param->size; y++)
	{
		for (int x = 0; x < param->size; x++)
		{

			cell = (param->array + (y * param->size + x));
			if (cell->color != DEFAULT) cell->color = (pos.x == x && pos.y == y) ? MIX : cell->color;
			else cell->color = (pos.x == x && pos.y == y) ? SELECT : DEFAULT;

			SetConsoleTextAttribute(console, cell->color);
			if (cell->check_value > 0)printf("|%5d|", cell->check_value);
			else printf("|%5c|", cell->check_value == WHITE_CELL ? ' ' : 'X');

			cell->color = DEFAULT;

		}
		printf("\n\n");
	}
}
