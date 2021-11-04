
#include "kuromasu.h"

gupdate_t game_loop(void* args, kaction_t action, tuple_t pos)
{
	field_t* field = (field_t*)args;

	// формирование копии поля.
	cell_t* buffer = (cell_t*)calloc(pow(field->size, 2), sizeof(cell_t));
	for (int i = 0; i < pow(field->size, 2); i++) *(buffer + i) = *(field->array + i);
	
	static int life_counter = LIFE_COUNT; // статическая переменная для счета жизни, объявляется и инициализируется при первом вызове функции game_loop

	//символьный массив life_line содержит символьное представление количества попыток для решения головоломки, которые остались у пользователя
	char life_line[LIFE_COUNT * 3 + 1];
	for (int i = 0; i < LIFE_COUNT; i++) 
	{
		life_line[i*3] = (i < life_counter) ? '\x03' : ' ';
		life_line[i*3 + 1] = life_line[i*3 + 2] = ' ';
	}
	printf("\n[ ЖИЗНИ: %8.8s ]\n", life_line);

	if (action == INPUT_BACK) // обработка события нажатия на клавишу escape
	{

		name_t list[] = { "ПРОДОЛЖИТЬ", "ВЫЙТИ" };
		dir_t items = dir_c(list, 2); // формируется объект items отрисовки диалогового окна 

		switch (*(int*)update_frame(dialog_box, tuple_c(0, 2), FALSE, &items)) // вызов отрисовки и обработка возращаемого пользователе результата
		{
		case 0: return gupdate_c(FRAME_RETURN, NULL);
		case 1:
			life_counter = LIFE_COUNT;
			return gupdate_c(FRAME_EXIT, NULL);
		}
	}
	// обработка события нажатия на клавишу space
	else if (action == INPUT_ACCEPT) set_axies(pos, field);//переключение значения клетки и пересчет видимых клеток

	
	gstate_t state = check_field(field); // проверка значений видимых клеток для каждой клетки поля
	draw_field(pos, field); //  отрисовка поля 
	bool result = TRUE; // инициализация переменной для фиксации поражения или победы

	switch (state) // обработка результата проверки
	{
	case STATE_WIN:
		life_counter = LIFE_COUNT; //возврат статической переменной в начальное положение
		return gupdate_c(FRAME_EXIT, &result); 
	case STATE_LOSE:
		printf("\n\t\t\tНАЖМИТЕ КЛАВИШУ ДЛЯ ПРОДОЛЖЕНИЯ\n\n"); 
		getch(); // пауза, чтобы пользователь смог увидеть допущенную ошибку 
		if (--life_counter <= 0)
		{
			result = FALSE;
			life_counter = LIFE_COUNT;
			return gupdate_c(FRAME_EXIT, &result);// выход из отрисовки и возврат значения 
		}
		for (int i = 0; i < pow(field->size, 2); i++) *(field->array + i) = buffer[i];// возврат к прошлой версии поля.
	case STATE_RUNNING: break;
	}
	free(buffer);
	return gupdate_c(FRAME_CONTINUE, NULL); // продолжение отрисовки 
}

void set_line(int dir, int* last, int break_point, int* cell) // перезапись значений свобоных клеток выбранной оси
{
	int value = break_point - (*last); 
	for (; (*last) < break_point; (*last)++, cell += (4 * dir)) *cell = value;
	(*last)++;
}

bool set_axies(tuple_t pos, field_t* ptr_param) 
{
	// указатель на клетку с курсором 
	cell_t* cell = (ptr_param->array + (pos.y * ptr_param->size + pos.x)); 

	// проверка  - 1.Никакие две черных ячейки не могу быть связаны по горизонтали иливертикали.
	//			   2.У клетки отсутствует значение.
	if (cell->check_value > 0 ||
		((pos.x != 0) && (cell - 1)->check_value == BLACK_CELL ||
			(pos.x != ptr_param->size - 1) && (cell + 1)->check_value == BLACK_CELL ||
			(cell + ptr_param->size)->check_value == BLACK_CELL ||
			(cell - ptr_param->size)->check_value == BLACK_CELL))
		return FALSE; 

	// переключение значения клетки 
	cell->check_value = (cell->check_value == BLACK_CELL ? WHITE_CELL : BLACK_CELL);

	// хранение индексов последних просчитанных клеток двух линий
	tuple_t last = tuple_c(0, 0);

	/*
	* пересчет видимый клеток в каждой клетки двух линий, которые пересекаются в выбранной клетке
	* проверяют две линий Х и У
	* если при проходе по линии встречается черная клетка или конец поля, значения free_value.(выбранная ось) 
	* от клетки с индексом last.(выбранная ось) до клетки где сработало условие пересчитывается: 
	* присваивается значение разности межку началом и концом просчета
	*/

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
		// если клетка не содержит цифру то пропуск итерации
		if (cell->check_value <= 0) continue;

		// если сумма свободных клеток по двум осям проверяемой клетки меньше чем значение клетки
		if (cell->free_value.x + cell->free_value.y < cell->check_value + 1)
		{
			result = STATE_LOSE;
			cell->color = RED;
		}
		else
		{
			// если сумма свободных клеток по двум осям проверяемой клетки не одинаковы со значение клетки до переключаем тригер победы
			if (cell->free_value.x + cell->free_value.y != cell->check_value + 1)trigger = FALSE;
			else cell->color = GREEN;
		}

	}
	if (trigger) result = STATE_WIN;
	return result;
}