
#include "kuromasu.h"

gupdate_t game_loop(void* args, kaction_t action, tuple_t pos)
{
	field_t* field = (field_t*)args;

	// формирование копии поля.
	cell_t* buffer = (cell_t*)calloc(pow(field->size, 2), sizeof(cell_t));
	for (unsigned int i = 0; i < pow(field->size, 2); i++) *(buffer + i) = *(field->array + i); 
	
	static unsigned int life_counter = LIFE_COUNT; // статическая переменная для счета жизни, объявляется и инициализируется при первом вызове функции game_loop
	gstate_t state = STATE_RUNNING;

	//символьный массив life_line содержит символьное представление количества попыток для решения головоломки, которые остались у пользователя
	char life_line[LIFE_COUNT * 3 + 1];
	for (unsigned int i = 0; i < LIFE_COUNT; i++)
	{
		life_line[i*3] = (i < life_counter) ? '\x03' : ' ';
		life_line[i*3 + 1] = life_line[i*3 + 2] = ' ';
	}

	cell_t current_cell = *(field->array + (field->size * pos.y + pos.x));
	printf("[ %-18s ( x: %2d , y: %2d ) ]\t[ ЖИЗНИ: %8.8s ]\n", "ВИДИМОСТЬ:", current_cell.free_value.x, current_cell.free_value.y, life_line);

	if (action == INPUT_BACK) // обработка события нажатия на клавишу escape
	{
		dir_t items = (dir_t){ (name_t[]){ "ПРОДОЛЖИТЬ РЕШЕНИЕ", "ВЫЙТИ В ГЛАВНОЕ МЕНЮ" }, 2 }; // формируется объект items отрисовки диалогового окна 
		switch (*(int*)update_frame(dialog_box, (tuple_t) { 0, 2 }, FALSE, &items)) // вызов отрисовки и обработка возращаемого пользователе результата
		{
		case 0: return (gupdate_t) { FRAME_RETURN, NULL };
		case 1:
			life_counter = LIFE_COUNT;
			return (gupdate_t) { FRAME_EXIT, NULL };
		}
	}
	else if (action == INPUT_ACCEPT) // обработка события нажатия на клавишу space
	{	
		//переключение значения клетки и пересчет видимых клеток
		if (set_axies(pos, field))state = check_field(field); // проверка значений видимых клеток для каждой клетки поля
	}

	draw_field(pos, field); //  отрисовка поля 
	bool result = TRUE; // инициализация переменной для фиксации поражения или победы

	switch (state) // обработка результата проверки
	{
	case STATE_WIN:
		life_counter = LIFE_COUNT; //возврат статической переменной в начальное положение
		return (gupdate_t) { FRAME_EXIT, &result };
	case STATE_LOSE:
		printf("\n\t\t[ НАЖМИТЕ ЛЮБУЮ КНОПКУ ДЛЯ ПРОДОЛЖЕНИЯ... ]\n\n\a"); 
		getch(); // пауза, чтобы пользователь смог увидеть допущенную ошибку 
		if (--life_counter <= 0)
		{
			result = FALSE;
			life_counter = LIFE_COUNT;
			return (gupdate_t) { FRAME_EXIT, &result };// выход из отрисовки и возврат значения 
		}
		for (unsigned int i = 0; i < pow(field->size, 2); i++) *(field->array + i) = buffer[i];// возврат к прошлой версии поля.
	case STATE_RUNNING: break;
	}
	free(buffer);
	return (gupdate_t) { FRAME_CONTINUE, NULL }; // продолжение отрисовки 
}

int connection_check(tuple_t pos, field_t* field, bool *checker) 
{
	int result = 1; 
	cell_t *current_cell = (field->array + (pos.y * field->size + pos.x));

	bool * current_checker = (checker + (pos.y * field->size + pos.x));
	*current_checker = TRUE;

	if (pos.x > 0 && (current_cell - 1)->check_value >= 0 && *(current_checker - 1) != TRUE)
		result += connection_check((tuple_t) { pos.x - 1, pos.y }, field, checker);
	if (pos.x < field->size - 1 && (current_cell + 1)->check_value >= 0 && *(current_checker + 1) != TRUE)
		result += connection_check((tuple_t) { pos.x + 1, pos.y }, field, checker);
	if (pos.y > 0 && (current_cell - field->size)->check_value >= 0 && *(current_checker - field->size) != TRUE)
		result += connection_check((tuple_t) { pos.x, pos.y - 1 }, field, checker);
	if (pos.y < field->size - 1 && (current_cell + field->size)->check_value >= 0 && *(current_checker + field->size) != TRUE)
		result += connection_check((tuple_t) { pos.x, pos.y + 1 }, field, checker);

	return result;
}

cell_t* set_line(cell_t* ptr_param, int size, int step, int shift)
{
	unsigned int last = 0;
	tuple_t* cell;
	for (unsigned int i = 0; i <= size; i++)
	{
		if ((ptr_param + (i * step) + shift)->check_value != BLACK_CELL && i != size) continue;
		for (int delta = i - last; last < i; last++)
		{
			cell = &(ptr_param + (last * step) + shift)->free_value;
			if (step == 1) cell->x = delta;
			else cell->y = delta;
		}
		last++;
	}
	return ptr_param;
}

bool set_axies(tuple_t pos, field_t* ptr_param)
{
	// указатель на клетку с курсором 
	cell_t* cell = (ptr_param->array + (pos.y * ptr_param->size + pos.x)); 

	// проверка  - 1.Никакие две черных ячейки не могу быть связаны по горизонтали иливертикали.
	//			   2.У клетки отсутствует значение.
	if (cell->check_value > 0 ||
		(pos.x != 0 && cell[-1].check_value == BLACK_CELL ||
			(pos.x != ptr_param->size - 1) && (cell + 1)->check_value == BLACK_CELL ||
			(cell + ptr_param->size)->check_value == BLACK_CELL ||
			(cell - ptr_param->size)->check_value == BLACK_CELL))
		return FALSE; 

	// переключение значения клетки 
	cell->check_value = (cell->check_value == BLACK_CELL ? WHITE_CELL : BLACK_CELL);
	/*
	* пересчет видимый клеток в каждой клетки двух линий, которые пересекаются в выбранной клетке
	* проверяют две линий Х и У
	* если при проходе по линии встречается черная клетка или конец поля, значения free_value.(выбранная ось) 
	* от клетки с индексом last.(выбранная ось) до клетки где сработало условие пересчитывается: 
	* присваивается значение разности межку началом и концом просчета
	*/
	ptr_param->array = set_line(ptr_param->array, ptr_param->size, 1, pos.y * ptr_param->size);
	ptr_param->array = set_line(ptr_param->array, ptr_param->size, ptr_param->size, pos.x);

	/*for (unsigned int i = 0; i <= ptr_param->size; i++)
	{
		if ((ptr_param->array + (pos.y * ptr_param->size + i))->check_value == BLACK_CELL || i == ptr_param->size)
			set_line(1, &last.x, i, &((ptr_param->array + (pos.y * ptr_param->size + last.x))->free_value.x));

		if ((ptr_param->array + (i * ptr_param->size + pos.x))->check_value == BLACK_CELL || i == ptr_param->size)
			set_line(ptr_param->size, &last.y, i, &((ptr_param->array + (last.y * ptr_param->size + pos.x))->free_value.y));
	}*/

	return TRUE;
}

gstate_t check_field(field_t* param)
{
	gstate_t result = STATE_RUNNING;
	bool *checker = (bool*)calloc(pow(param->size, 2), sizeof(bool)), trigger = TRUE;

	int white_cell_counter = 0, connections = 0;
	for (unsigned int i = 0; i < pow(param->size, 2); i++)
	{
		if ((param->array + i)->check_value >= 0) 
		{
			if (!connections) connections = connection_check((tuple_t) { i% param->size, i / param->size }, param, checker);
			white_cell_counter++;
		}
	}
	free(checker);

	for (unsigned int i = 0; i < pow(param->size, 2); i++)
	{
		cell_t* cell = (param->array + i);
		// если клетка не содержит цифру то пропуск итерации
		if (cell->check_value <= 0) continue;
		
		// если сумма свободных клеток по двум осям проверяемой клетки меньше чем значение клетки
		if ((cell->free_value.x + cell->free_value.y < cell->check_value + 1) || (connections != white_cell_counter))
		{
			result = STATE_LOSE;
			cell->color = RED;
			continue;
		}

		// если сумма свободных клеток по двум осям проверяемой клетки не одинаковы со значение клетки до переключаем тригер победы
		if (cell->free_value.x + cell->free_value.y != cell->check_value + 1)
		{
			trigger = FALSE;
			cell->color = DEFAULT;
		}
		else cell->color = GREEN;
			
	
	}
	if (trigger && result != STATE_LOSE) result = STATE_WIN;
	return result;
}