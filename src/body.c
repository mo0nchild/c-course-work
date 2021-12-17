#include "kuromasu.h"

void* update_frame(update_action_function action, tuple_t max,
	bool back_use, void* param)
{
	kaction_t input = INPUT_NORMALLY;
	tuple_t pos = (tuple_t){ 0, 0 };
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	while (1)
	{
		switch (input)
		{
		case INPUT_BACK: if (!back_use) break;
		case INPUT_ACCEPT:
		case INPUT_NORMALLY:
			clear_frame();
			SetConsoleTextAttribute(console, SELECT);
			printf("\n[  СТРЕЛКИ: КУРСОР   ] [ SPACE: ПОДТВЕРДИТЬ ] [ %18.18s ]\n\n",
				(back_use) ? "ESCAPE: ВЕРНУТЬСЯ" : "КНОПКА ЗАБЛОКИРОВАНА");
			SetConsoleTextAttribute(console, DEFAULT);

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

void draw_list(unsigned int cursor, dir_t* param, int begin, int end)
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	for (unsigned int i = begin; i < end; i++)
	{
		if (cursor == i)SetConsoleTextAttribute(console, SELECT);
		printf("\n\t\t\t[ %20.20s ]\n", i < param->size ? *(param->array + i) : "ПУСТО");
		SetConsoleTextAttribute(console, DEFAULT);
	}
}

gupdate_t dialog_box(void* args, kaction_t action, tuple_t pos)
{
	dir_t* items = (dir_t*)args;
	if (action == INPUT_ACCEPT)
	{
		unsigned int select = (pos.y);
		return (gupdate_t) { FRAME_EXIT, & select };
	}

	printf("\n\n\t\t%38s\n\t\t%34s\n\n\n", "[ КАКОЕ ИЗ ПРЕДЛОЖЕННЫХ ДЕЙСТВИЙ ]", "[ ВЫ ХОТИТЕ СОВЕРШИТЬ ? ]");
	draw_list(pos.y, items, 0, items->size);
	return (gupdate_t) { FRAME_CONTINUE, NULL };
}

void draw_field(tuple_t pos, field_t* param)
{
	cell_t* cell;
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	printf("[ %-18s ( x: %2d , y: %2d ) ]\n\n", "ПОЗИЦИЯ КУРСОРА:", pos.x, pos.y);
	for (unsigned int y = 0; y < param->size; y++)
	{
		for (unsigned int x = 0; x < param->size; x++)
		{
			cell = (param->array + (y * param->size + x));
			if (cell->color != DEFAULT) SetConsoleTextAttribute(console, (pos.x == x && pos.y == y) ? MIX : cell->color);
			else SetConsoleTextAttribute(console, (pos.x == x && pos.y == y) ? SELECT : DEFAULT);

			char shift = (x == 0) ? '\t' : ':';
			if (cell->check_value > 0)printf("%c|%4d|", shift, cell->check_value);
			else printf("%c|%4c|", shift, cell->check_value == WHITE_CELL ? ' ' : 'X');

			SetConsoleTextAttribute(console, DEFAULT);
		}
		printf("\n\n");
	}
}

void print_rules(void)
{
	clear_frame();
	printf("\n\t\t[  ПРАВИЛА ИГРЫ-ГОЛОВОЛОМКИ \"КУРОМАСУ\"  ]\n\n"); 
	printf("\t\tКуромасу играется на прямоугольной сетке. \n\t\tВ некоторых из этих ячеек есть числа. \n\t\tКаждая ячейка может быть черной или белой. \n\t\tЦель состоит в том, чтобы определить, \n\t\tк какому типу относится каждая ячейка.\n\t\tСледующие правила определяют, какие ячейки какие :\n\n\t\t1)Каждое число на доске представляет \n\t\tколичество белых клеток, которые можно увидеть из этой клетки,\n\t\t включая ее самого.Ячейку можно увидеть из другой ячейки,\n\t\t если они находятся в той же строке или столбце,\n\t\t и между ними нет черных ячеек в этой строке или столбце.\n\t\t2)Пронумерованные ячейки не могут быть черными.\n\t\t3)Никакие две черные клетки не могут быть смежными \n\t\tпо горизонтали или вертикали.\n\t\t4)Все белые клетки должны быть соединены горизонтально \n\t\tили вертикально.\n");
	printf("\n\t\t[ НАЖМИТЕ ЛЮБУЮ КНОПКУ ДЛЯ ПРОДОЛЖЕНИЯ... ]\n\n");
	getch();
}

void print_message(char* str[3])
{
	clear_frame();
	printf("\n\n\t\t%40s\n\n\n", str[0]);
	printf("\t\t\t%20s\n\t\t\t%20s\n\a\n\n\t\t[ НАЖМИТЕ ЛЮБУЮ КНОПКУ ДЛЯ ПРОДОЛЖЕНИЯ... ]", str[1], str[2]);
	getch();
}

bool read_path(dir_t* param)
{
	DIR* directory = opendir(".\\data");
	struct dirent* ptr_dirent_value;
	*param = (dir_t){ (name_t*)calloc(1, sizeof(name_t)), 1 };

	if (!directory) return FALSE;
	int file_name_size = 0;
	while ((ptr_dirent_value = readdir(directory)) != NULL && param->size < MAX_FILES_IN_DIR)
	{
		if (strstr(ptr_dirent_value->d_name, ".txt") == NULL) continue;

		file_name_size = abs(ptr_dirent_value->d_name - strstr(ptr_dirent_value->d_name, ".txt"));
		ptr_dirent_value->d_name[file_name_size] = '\0';

		param->array = (name_t*)realloc(param->array, sizeof(name_t) * (param->size + 1));
		strncpy(param->array[param->size++], ptr_dirent_value->d_name, file_name_size + 1);

	}
	closedir(directory);
	*param = (dir_t){ param->array+1, param->size -1};

	return TRUE;
}

bool file_data(field_t* field, bool readonly)
{
	if (!readonly) 
	{
		bool checker = FALSE;
		for (unsigned int i = 0; i < pow(field->size, 2); i++)
		{
			if ((field->array + i)->check_value > 0)
			{
				checker = TRUE;
				break;
			}
		}
		if (!checker) return FALSE;
	}

	name_t dirname;
	FILE* file;
	sprintf(dirname, "./data/%s.txt", field->name);
	if ((file = fopen(dirname, readonly ? "rt" : "wt")) == NULL) return FALSE;

	if (readonly)
	{
		unsigned int index = 0;
		if (!fscanf(file, "%d", &field->size)) return FALSE;
		//field->array = (cell_t*)calloc(pow(field->size, 2), sizeof(cell_t));
		field->array = (cell_t*)realloc(field->array, pow(field->size, 2) * sizeof(cell_t));

		for (unsigned int i = 0; i < pow(field->size, 2); i++)
		{
			field->array[i].free_value.x = field->array[i].free_value.y = field->size;
			field->array[i].color = DEFAULT;
		}
		while (!feof(file))
		{
			if (!fscanf(file, "%d", &((field->array + (index++))->check_value))) continue;
		}
	}
	else
	{
		fprintf(file, "%d\n", field->size);
		for (unsigned int i = 0; i < field->size; i++)
		{
			for (unsigned int k = 0; k < field->size; k++)
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
	case key_left: pos->x = (pos->x <= 0 ? 0 : pos->x - 1); break;
	case key_right:
		pos->x = (pos->x + 1 > max.x - 1 ? max.x - 1 : pos->x + 1);
		break;
	case key_up: pos->y = (pos->y <= 0 ? 0 : pos->y - 1); break;
	case key_down:
		pos->y = (pos->y + 1 > max.y - 1 ? max.y - 1 : pos->y + 1);
		break;
	case key_space: state = INPUT_ACCEPT; break;
	case key_esc: state = INPUT_BACK; break;
	default: state = INPUT_ERROR; break;
	}
	return state;
}
