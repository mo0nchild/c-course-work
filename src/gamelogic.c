
#include "kuromasu.h"

gupdate_t game_loop(void* args, kaction_t action, tuple_t pos)
{
	field_t* field = (field_t*)args;

	// ������������ ����� ����.
	cell_t* buffer = (cell_t*)calloc(pow(field->size, 2), sizeof(cell_t));
	for (int i = 0; i < pow(field->size, 2); i++) *(buffer + i) = *(field->array + i);
	
	static int life_counter = LIFE_COUNT; // ����������� ���������� ��� ����� �����, ����������� � ���������������� ��� ������ ������ ������� game_loop

	//���������� ������ life_line �������� ���������� ������������� ���������� ������� ��� ������� �����������, ������� �������� � ������������
	char life_line[LIFE_COUNT * 3 + 1];
	for (int i = 0; i < LIFE_COUNT; i++) 
	{
		life_line[i*3] = (i < life_counter) ? '\x03' : ' ';
		life_line[i*3 + 1] = life_line[i*3 + 2] = ' ';
	}
	printf("\n[ �����: %8.8s ]\n", life_line);

	if (action == INPUT_BACK) // ��������� ������� ������� �� ������� escape
	{

		name_t list[] = { "����������", "�����" };
		dir_t items = dir_c(list, 2); // ����������� ������ items ��������� ����������� ���� 

		switch (*(int*)update_frame(dialog_box, tuple_c(0, 2), FALSE, &items)) // ����� ��������� � ��������� ������������ ������������ ����������
		{
		case 0: return gupdate_c(FRAME_RETURN, NULL);
		case 1:
			life_counter = LIFE_COUNT;
			return gupdate_c(FRAME_EXIT, NULL);
		}
	}
	// ��������� ������� ������� �� ������� space
	else if (action == INPUT_ACCEPT) set_axies(pos, field);//������������ �������� ������ � �������� ������� ������

	
	gstate_t state = check_field(field); // �������� �������� ������� ������ ��� ������ ������ ����
	draw_field(pos, field); //  ��������� ���� 
	bool result = TRUE; // ������������� ���������� ��� �������� ��������� ��� ������

	switch (state) // ��������� ���������� ��������
	{
	case STATE_WIN:
		life_counter = LIFE_COUNT; //������� ����������� ���������� � ��������� ���������
		return gupdate_c(FRAME_EXIT, &result); 
	case STATE_LOSE:
		printf("\n\t\t\t������� ������� ��� �����������\n\n"); 
		getch(); // �����, ����� ������������ ���� ������� ���������� ������ 
		if (--life_counter <= 0)
		{
			result = FALSE;
			life_counter = LIFE_COUNT;
			return gupdate_c(FRAME_EXIT, &result);// ����� �� ��������� � ������� �������� 
		}
		for (int i = 0; i < pow(field->size, 2); i++) *(field->array + i) = buffer[i];// ������� � ������� ������ ����.
	case STATE_RUNNING: break;
	}
	free(buffer);
	return gupdate_c(FRAME_CONTINUE, NULL); // ����������� ��������� 
}

void set_line(int dir, int* last, int break_point, int* cell) // ���������� �������� �������� ������ ��������� ���
{
	int value = break_point - (*last); 
	for (; (*last) < break_point; (*last)++, cell += (4 * dir)) *cell = value;
	(*last)++;
}

bool set_axies(tuple_t pos, field_t* ptr_param) 
{
	// ��������� �� ������ � �������� 
	cell_t* cell = (ptr_param->array + (pos.y * ptr_param->size + pos.x)); 

	// ��������  - 1.������� ��� ������ ������ �� ���� ���� ������� �� ����������� ������������.
	//			   2.� ������ ����������� ��������.
	if (cell->check_value > 0 ||
		((pos.x != 0) && (cell - 1)->check_value == BLACK_CELL ||
			(pos.x != ptr_param->size - 1) && (cell + 1)->check_value == BLACK_CELL ||
			(cell + ptr_param->size)->check_value == BLACK_CELL ||
			(cell - ptr_param->size)->check_value == BLACK_CELL))
		return FALSE; 

	// ������������ �������� ������ 
	cell->check_value = (cell->check_value == BLACK_CELL ? WHITE_CELL : BLACK_CELL);

	// �������� �������� ��������� ������������ ������ ���� �����
	tuple_t last = tuple_c(0, 0);

	/*
	* �������� ������� ������ � ������ ������ ���� �����, ������� ������������ � ��������� ������
	* ��������� ��� ����� � � �
	* ���� ��� ������� �� ����� ����������� ������ ������ ��� ����� ����, �������� free_value.(��������� ���) 
	* �� ������ � �������� last.(��������� ���) �� ������ ��� ��������� ������� ���������������: 
	* ������������� �������� �������� ����� ������� � ������ ��������
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
		// ���� ������ �� �������� ����� �� ������� ��������
		if (cell->check_value <= 0) continue;

		// ���� ����� ��������� ������ �� ���� ���� ����������� ������ ������ ��� �������� ������
		if (cell->free_value.x + cell->free_value.y < cell->check_value + 1)
		{
			result = STATE_LOSE;
			cell->color = RED;
		}
		else
		{
			// ���� ����� ��������� ������ �� ���� ���� ����������� ������ �� ��������� �� �������� ������ �� ����������� ������ ������
			if (cell->free_value.x + cell->free_value.y != cell->check_value + 1)trigger = FALSE;
			else cell->color = GREEN;
		}

	}
	if (trigger) result = STATE_WIN;
	return result;
}