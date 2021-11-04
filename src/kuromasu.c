#include "kuromasu.h"

void* update_frame(update_action_function action, tuple_t max,
	bool back_use, void* param)
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
			printf("\n[ W A S D: ѕ–≈ћ≈—“»“№  ”–—ќ– ] [ SPACE: ѕќƒ“¬≈–ƒ»“№ ] [ %18.18s ]\n",
				(back_use) ? "ESCAPE: ¬≈–Ќ”“№—я" : "==================");

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
		printf("\n\t\t\t[ %20.20s ]\n", i < param->size ? *(param->array + i) : "ѕ”—“ќ");
		SetConsoleTextAttribute(console, DEFAULT);
	}
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

			SetConsoleTextAttribute(console, cell->color = DEFAULT);
		}
		printf("\n\n");
	}
}

void print_rules(void)
{
	clear_frame();
	printf("\n\t\t уромасу играетс€ на пр€моугольной сетке. \n\t\t¬ некоторых из этих €чеек есть числа. \n\t\t ажда€ €чейка может быть черной или белой. \n\t\t÷ель состоит в том, чтобы определить, \n\t\tк какому типу относитс€ кажда€ €чейка.\n\t\t—ледующие правила определ€ют, какие €чейки какие :\n\n\t\t\t1) аждое число на доске представл€ет \n\t\tколичество белых клеток, которые можно увидеть из этой клетки,\n\t\t включа€ ее самого.ячейку можно увидеть из другой €чейки,\n\t\t если они наход€тс€ в той же строке или столбце,\n\t\t и между ними нет черных €чеек в этой строке или столбце.\n\t\t\t2)ѕронумерованные €чейки не могут быть черными.\n\t\t\t3)Ќикакие две черные клетки не могут быть смежными \n\t\tпо горизонтали или вертикали.\n\t\t\t4)¬се белые клетки должны быть соединены горизонтально \n\t\tили вертикально.\n");
	printf("\n\t\t\t\tЌј∆ћ»“≈  Ћј¬»Ў” ƒЋя ѕ–ќƒќЋ∆≈Ќ»я\n\n");
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

bool file_data(field_t* field, bool readonly)
{
	name_t dirname;
	FILE* file;
	if (!readonly) 
	{
		bool checker = FALSE;
		for (int i = 0; i < pow(field->size, 2); i++)
		{
			if ((field->array + i)->check_value > 0)
			{
				checker = TRUE;
				break;
			}
		}
		if (!checker) return FALSE;
	}
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