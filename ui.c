#include "kuromasu.h"

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
		printf("\n\n\n\n\t\t[ «Õ¿◊≈Õ»≈: %c %2d %c ]", (pos.x == 0) ? ' ' : '<', pos.x,
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

	printf("\n\n\n\n\t\t[ –¿«Ã≈– œŒÀﬂ: %c %2d %c ]", (pos.x == 0) ? ' ' : '<',
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

	printf("\n\n\n\t\t\t%s\n", "◊“Œ ¬€ ’Œ“»“≈ —ƒ≈À¿“‹?\n\n\n");
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
		name_t list[] = { "œ–ŒƒŒÀ∆»“‹", "¬€…“»", "—Œ’–¿Õ»“‹" };
		dir_t items = dir_c(list, 3);
		switch (*(int*)update_frame(dialog_box, tuple_c(0, 3), FALSE, &items))
		{
		case 0: break;
		case 1: return gupdate_c(FRAME_EXIT, NULL);
		case 2:
			clear_frame();
			printf("\n\n\n\t\t\t[ ¬¬≈ƒ»“≈ Õ¿«¬¿Õ»≈ ]: ");
			scanf("%s", field->name);
			return gupdate_c(FRAME_EXIT, field);
		}
	}

	draw_field(pos, field);

	return gupdate_c(FRAME_CONTINUE, NULL);
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
		printf("\n\n\n");
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
					printf("\n\n\n\t\t\t[ Õ≈¬Œ«ÃŒ∆ÕŒ —Œ’–¿Õ»“‹ ]\n");
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

	printf("\n“≈ ”Ÿ≈≈ œŒÀ≈: %s\n\n\n", field->name);

	const name_t list[] = { "—Œ«ƒ¿“‹", "«¿√–”«»“‹", "œ–¿¬»À¿", "Õ¿«¿ƒ" };
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
				printf("\n\n\n\t\t\tÕ≈¬Œ«ÃŒ∆ÕŒ «¿√–”«»“‹ œŒÀ≈: %s\n", field->name);
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

			name_t list[] = { "¬≈–Õ”“‹—ﬂ", "¬€…“»" };
			dir_t items = dir_c(list, 2);

			switch (*(int*)update_frame(dialog_box, tuple_c(0, 2), FALSE, &items))
			{
			case 0: return gupdate_c(FRAME_RETURN, NULL);
			case 1:return gupdate_c(FRAME_EXIT, NULL);
			}
		}
	}

	printf("\n%s\n\n\n", NAME_LABEL);

	name_t list[] = { "Õ¿◊¿“‹", "–≈ƒ¿ “Œ–", "¬€…“»" };
	dir_t items = dir_c(list, 3);
	draw_list(pos.y, &items, 0, 3);

	return gupdate_c(FRAME_CONTINUE, NULL);

}