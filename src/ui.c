#include "kuromasu.h"

gupdate_t set_cell_value(void* args, kaction_t action, tuple_t pos)
{
	int* field_size = (int*)args;
	if (action == INPUT_ACCEPT)
	{
		int value = pos.x;
		return (gupdate_t) { FRAME_EXIT, & value };
	}
	else
	{
		printf("\n\n\n\n\t\t[ УСТАНОВИТЬ ЧИСЛО ДЛЯ ВЫБРАННОЙ КЛЕТКИ ]\n\n\t\t\t[ ЗНАЧЕНИЕ: %c %2d %c ]", (pos.x == 0) ? ' ' : '<', pos.x,
			(pos.x == *field_size * 2 - 1) ? ' ' : '>');
	}
	return (gupdate_t) { FRAME_CONTINUE, NULL };
}

gupdate_t create_field(void* args, kaction_t action, tuple_t pos)
{
	if (action == INPUT_ACCEPT)
	{
		field_t field;

		field.size = pos.x + MIN_FIELD_SIZE;
		field.array = (cell_t*)calloc(pow(field.size, 2), sizeof(cell_t));

		for (int i = 0; i < field.size; i++)
		{
			for (int k = 0; k < field.size; k++)
				*(field.array + (i * field.size + k)) = (cell_t){ (tuple_t) {0,0}, 0, DEFAULT };
		}
		return (gupdate_t) { FRAME_EXIT, & field };
	}

	printf("\n\n\n\n\t\t[ УСТАНОВИТЬ РАЗМЕР ДЛЯ ИГРОВОГО ПОЛЯ ]\n\n\t\t\t[ ЗНАЧЕНИЕ: %c %2d %c ]", (pos.x == 0) ? ' ' : '<',
		pos.x + MIN_FIELD_SIZE, (pos.x == MAX_FIELD_SIZE - MIN_FIELD_SIZE) ? ' ' : '>');

	return (gupdate_t) { FRAME_CONTINUE, NULL };
}

gupdate_t set_field_values(void* args, kaction_t action, tuple_t pos)
{
	field_t* field = (field_t*)args;
	if (action == INPUT_ACCEPT)
	{
		(field->array + (pos.y * field->size + pos.x))->check_value =
			*(int*)update_frame(set_cell_value, (tuple_t) { field->size * 2, 0 }, FALSE, & (field->size));
	}
	else if (action == INPUT_BACK)
	{
		dir_t items = (dir_t){ (name_t[]) { "ПРОДОЛЖИТЬ СОЗДАНИЕ", "ВЫЙТИ ИЗ РЕДАКТОРА", "СОХРАНИТЬ УРОВЕНЬ" }, 3 };
		switch (*(int*)update_frame(dialog_box, (tuple_t) { 0, 3 }, FALSE, & items))
		{
		case 0: break;
		case 1: return (gupdate_t) { FRAME_EXIT, NULL };
		case 2: return (gupdate_t) { FRAME_EXIT, field };
		}
	}

	draw_field(pos, field);
	return (gupdate_t) { FRAME_CONTINUE, NULL };
}

gupdate_t load_file(void* args, kaction_t action, tuple_t pos)
{
	pos.y = pos.y + pos.x * FILES_ON_PAGE;
	dir_t* param = (dir_t*)args;
	if (action == INPUT_ACCEPT && pos.y < param->size)
	{
		name_t filename;
		strcpy(filename, param->array + pos.y);

		dir_t items = (dir_t){ (name_t[]) { "ВЫБРАТЬ ИГРОВОЕ ПОЛЕ", "РЕДАКТИРОВАТЬ ТЕКУЩЕЕ", "ВЕРНУТЬСЯ НАЗАД" }, 3};
		switch (*(unsigned int*)update_frame(dialog_box, (tuple_t) { 0, 3 }, FALSE, &items))
		{
		case 0: return (gupdate_t) { FRAME_EXIT, filename };
		case 1:;
			field_t read_field;
			strcpy(read_field.name, filename);
			if (!file_data(&read_field, TRUE)) 
			{
				print_message((char* []) { "[ СОВЕРШЕНА ОШИБКА ПРИ РЕДАКТИРОВНИИ ПОЛЯ ]", "[  НЕВОЗМОЖНО ОТКРЫТЬ  ]", "[ ФАЙЛ ДЛЯ ИГРОВОГО ПОЛЯ ]" });
				break;
			}
			field_t* save_field = (field_t*)update_frame(set_field_values, (tuple_t) { read_field.size, read_field.size }, TRUE, &read_field);
			if (!file_data(&read_field, FALSE)) 
				print_message((char* []) { "[ СОВЕРШЕНА ОШИБКА ПРИ РЕДАКТИРОВНИИ ПОЛЯ ]", "[  НЕВОЗМОЖНО ОТКРЫТЬ  ]", "[ ФАЙЛ ДЛЯ ИГРОВОГО ПОЛЯ ]" });
			
		case 2: break;
		}

	}
	else if (action == INPUT_BACK) return (gupdate_t) { FRAME_EXIT, NULL };
	else
	{
		printf("\n\t\t%34s\n\n", "[ СПИСОК ДОСТУПНЫХ ПОЛЕЙ: ]");
		draw_list(pos.y, param, FILES_ON_PAGE * pos.x, FILES_ON_PAGE * (pos.x + 1));
		printf("\n\n\t\t\t[ СТРАНИЦА: %2d/%2d ИТОГ: %d ]", pos.x + 1, (int)ceil(param->size / (double)FILES_ON_PAGE), param->size);
	}
	return (gupdate_t) { FRAME_CONTINUE, NULL };
}

gupdate_t settings(void* args, kaction_t action, tuple_t pos)
{
	field_t* field = (field_t*)(args);
	if (action == INPUT_ACCEPT)
	{
		switch (pos.y)
		{
		case 0:;
			field_t edit_field = *(field_t*)update_frame(create_field, (tuple_t) { fabs(MAX_FIELD_SIZE-MIN_FIELD_SIZE)+1, 0 }, FALSE, NULL);
			field_t* save_field = (field_t*)update_frame(set_field_values, (tuple_t) { edit_field.size, edit_field.size }, TRUE, &edit_field);
			
			if (save_field != NULL)
			{
				clear_frame();
				printf("\n\n\n\t\t%40s\n\t\t%30s\n\n\t\t%10s  ", "[ ВВЕДИТЕ НАЗВАНИЕ ДЛЯ ИГРОВОГО ПОЛЯ ]",
					"[ РАСПОЛОЖЕНИЕ ФАЙЛА: .\\data\\название.txt ]", "[ ---> ]");
				if (!scanf("%20s", save_field->name)) break;
				
				if (!file_data(save_field, FALSE))
					print_message((char* []) { "[ СОВЕРШЕНА ОШИБКА ПРИ СОЗДАНИИ ПОЛЯ ]", "[  НЕВОЗМОЖНО СОХРАНИТЬ  ]", "[ ЗНАЧЕНИЯ ИГРОВОГО ПОЛЯ ]" });
				else *field = *save_field;
			}
			break;
		case 1:;
			dir_t dir;
			if (read_path(&dir))
			{
				unsigned int border = (unsigned int)(ceil(dir.size / 3.f) == 0) ? 1 : ceil(dir.size / 3.f);
				name_t* filename = (name_t*)update_frame(load_file, (tuple_t) { border, FILES_ON_PAGE }, TRUE, & dir);
				if (filename != NULL)strcpy(field->name, *filename);
			}
			break;
		case 2: print_rules();  return (gupdate_t) { FRAME_CONTINUE, NULL };
		case 3: return (gupdate_t) { FRAME_EXIT, field };
		}
	}
	else if (action == INPUT_BACK) return (gupdate_t) { FRAME_EXIT, field };

	printf("[ ТЕКУЩЕЕ ПОЛЕ: %10s ]\n\n\t\t%38s\n\n", field->name, "[ СТРАНИЦА РЕДАКТОРА ИГРОВЫХ ПОЛЕЙ ]");
	dir_t items = (dir_t){ (name_t[]) { "СОЗДАТЬ УРОВЕНЬ", "ЗАГРУЗИТЬ СЕТКУ", "ПРАВИЛА ГОЛОВОЛОМКИ", " ВЕРНУТЬСЯ НАЗАД" }, 4 };
	draw_list(pos.y, &items, 0, 4);

	return (gupdate_t) { FRAME_CONTINUE, NULL };
}

gupdate_t mainmenu(void* args, kaction_t action, tuple_t pos)
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
				printf("\n\n\n\t\t\t[ НЕВОЗМОЖНО ЗАГРУЗИТЬ ПОЛЕ: %s ]\n\a\n\t\t[ НАЖМИТЕ ЛЮБУЮ КНОПКУ ДЛЯ ПРОДОЛЖЕНИЯ... ]", field->name);
				getch(); break;
			}
			bool* game_result = (bool*)update_frame(game_loop, (tuple_t) { field->size, field->size }, TRUE, field);
			if (game_result != NULL)
			{
				bool decode = *game_result;
				clear_frame();
				printf("\n\n\n\n%s\n\a\n\n\n\t\t[ НАЖМИТЕ ЛЮБУЮ КНОПКУ ДЛЯ ПРОДОЛЖЕНИЯ... ]", decode ? WIN_LABEL : LOSE_LABEL);
				getch();
			}

			break;
		case 1:;
			*field = *(field_t*)update_frame(settings, (tuple_t) { 0, 4 }, FALSE, field);
			break;
		case 2:;
			dir_t items = (dir_t){ (name_t[]) { "ВЕРНУТЬСЯ В МЕНЮ", "ВЫЙТИ ИЗ ПРОГРАММЫ" }, 2 };

			switch (*(unsigned int*)update_frame(dialog_box, (tuple_t) { 0, 2 }, FALSE, &items))
			{
			case 0: return (gupdate_t) { FRAME_RETURN, NULL };
			case 1: return (gupdate_t) { FRAME_EXIT, NULL };				
			}
		}
	}

	printf("\n%s\n\n\n", NAME_LABEL);

	dir_t items = (dir_t){ (name_t[]) { "НАЧАТЬ РЕШЕНИЕ", "РЕДАКТОР УРОВНЕЙ", "ВЫЙТИ ИЗ ИГРЫ" }, 3 };
	draw_list(pos.y, &items, 0, 3);

	return (gupdate_t) { FRAME_CONTINUE, NULL };

}