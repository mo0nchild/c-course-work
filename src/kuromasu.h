#pragma once//ver 1.5
#define _CRT_SECURE_NO_WARNINGS
/*
* ������������ ������������ �����
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
* ��������� ��� ��������� ������ ����������
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
* ��������� ��� ��������� ��������
*/

#define WIN_LABEL "\t=====  |   |      |===|  |   |  |  /|  |===  |===|    \\      \\ \n\t  |    |=| |      |--/   |=| |  | / |  |     |__-|   /_\\    / \\ \n\t  |    |_| |      |___|  |_| |  |/  |  |     |      /   \\  /   \\ \n"
#define LOSE_LABEL  "\t=====  |   |      |===|  |===|  |===|  |  /|  |===  |===|    \\      \\ \n\t  |    |=| |      |   |  |__-|  |   |  | / |  |     |__-|   /_\\    / \\\n\t  |    |_| |      |   |  |      |___|  |/  |  |     |      /   \\  /   \\ \n"
#define NAME_LABEL  "\t\t| /  \\  /   |==|  |==|  |\\ /|    \\    |==|  \\  /\n\t\t|\\     /    |__|  |  |  | \\ |   /_\\   |       /\n\t\t| \\   /     |	  |__|  |   |  /   \\  |__|   /"

/*
* �������� ���� KEY_CODE ��� ������ � ���������
* �������� ������������� � ������������ � ������ ������������ ��������
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
* ����������� ���� color_t ��� ���������� ������, ������� ������������ ��� ��������� ����������
* �������� ������������� ����� ������.
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
* ��� gstate_t ��������� ��� ������ � ������� ������������� ����������:
*	STATE_RUNNING - ������� ���������
*	STATE_LOSE - ������������ �������� ������ �� ����� ����
*	STATE_WIN - ������������ �������� ���� ���������
*/
typedef enum { STATE_RUNNING, STATE_WIN, STATE_LOSE } gstate_t;

/*
* ��� kaction_t ��������� ��� ��������� ��������� ������� ������� �������������:
*	INPUT_ERROR - ������������ ����� �� ������ �������
*	INPUT_NORMALLY - ������������ ����� ������� ��� ����� ��������� �������
*	INPUT_ACCEPT - ������������ ����� ������� ��� ������������
*	INPUT_BACK - ������������ ����� ������� "��������"
*/
typedef enum { INPUT_ERROR, INPUT_NORMALLY, INPUT_ACCEPT, INPUT_BACK } kaction_t;
/*
* ��� fupdate_t ������������ ��� ��������� ��������� ��������� ����������
*	FRAME_CONTINUE - ��������� ������������
*	FRAME_RETURN - ��������� � ������ ���������
*	FRAME_EXIT - ����� �� ��������� ������� ����� � ������� ��������
*/
typedef enum { FRAME_CONTINUE, FRAME_RETURN, FRAME_EXIT } fupdate_t;

typedef char name_t[261]; //��������� ��� ������������ ��� ������ � ����������
typedef _Bool bool; //��������� �������� ���� 

/*
* ��������� gupdate_t ���������� ��������� � �������� ��� �������� �����
*	frame_update_state - ��� �������� ���������
*	return_value - ��������� �� ������������ ��������
*/

typedef struct gupdate_t
{
	fupdate_t frame_update_state;
	void* return_value;
} gupdate_t;
gupdate_t gupdate_c(fupdate_t frame_update_state, void* return_value); // ����������� ��� ��������� gupdate_t


/*
* ��������� tuple_t �������� ���� �������� (���������)
*/

typedef struct tuple_t { int x, y; } tuple_t;
tuple_t tuple_c(int x, int y); // ����������� ��� ��������� tuple_t

/*
* ��������� cell_t ��������� �������� ������ �������� ����
*	free_value - ������ ��� �������� ������� ������ �� ���� ����
*	check_value - �������� ������
*	color - ���� ��� ���������
*/

typedef struct cell_t
{
	tuple_t free_value;
	int check_value;
	color_t color;
} cell_t;
cell_t cell_c(int check, int free, color_t color);// ����������� ��� ��������� cell_t

/*
* ��������� dir_t ��������� �������� � ������ ������� �����
*	array - ������ �����
*	size - ������ �������
*/

typedef struct dir_t
{
	name_t* array;
	int size;
} dir_t;
dir_t dir_c(name_t* dir, int size); // ����������� ��� ��������� dir_t

/*
* ��������� field_t ��������� ������� ����
*	array - ������ ������ ���� cell_t
*	size - ������ ���� (�������)
*	name - �������� ����
*/

typedef struct field_t
{
	cell_t* array;
	name_t name;
	int size;
}field_t;
field_t field_c(cell_t* array, name_t name, int size); // ����������� ��� ��������� field_t

/*
* ����������� ���� ��������� �� �������, ���������� ��� ��������� �����  
* ������� - ���������� ��������� fupdate_t � ��������� �� ����������
* args - ��������� ������� ��������
* action - ���������������� �������
* pos - ��������� ������� 
*/
typedef gupdate_t(*update_action_function)(void* args, kaction_t action, tuple_t pos);

/*
* ������� set_line - �������� �������� ��������� ���
* dir - ����������, ��������������� ����������� �������� �� ����
* last - ��������� �� ��������� ������
* break_point - ������ ������ ����� ��� ��������� �����������
* cell - ��������� �� ��������� ������
*/
void set_line(int dir, int* last, int i, int* cell); 

/*
* ������� draw_list - ������������ ������
* cursor - ������ ���������� ������
* param - ������ ��� ���������
* begin - ������ ������� ������� ��� ���������param[]
* end - ������ ���������� ������� ��� ���������
*/
void draw_list(int cursor, dir_t* param, int begin, int end);

/*
* ������� draw_list - ������������ ������� ����
* pos - ���������� �������
* param - ��������� �� ���������, ��������������� ����
*/
void draw_field(tuple_t pos, field_t* param);

/*
* update_frame - ������� ��������� ���� ��������� � ��������� ���������������� ������� 
*	��������� ��������� ������� ���� - ��������� ������ ������� action, ������� ���� ������������� � ��������� ������� ���� ������
* action - ��������� �� �������, ������� ���������� ������ ���� (���� �������� ��� ��������������� �����) ��� ��������� � ���������
*	���������������� ������
* max - �������� ������ ��� ������� �� ���� ����
* back_use - true=������� escape ����� �������������� � ������� action; false=������������� �������
* param - ��������� �� �������� ��� �������� � ������� action � �������� ���������
*/
void* update_frame(update_action_function action, tuple_t max,
	bool back_use, void* param);


/*
* print_rules - ��������� ������ ����
*/
void print_rules(void);

gupdate_t set_cell_value(void* args, kaction_t action, tuple_t pos); // �������� ��� ��������� ������
gupdate_t set_field_size(void* args, kaction_t action, tuple_t pos); // �������� ������� ����
gupdate_t set_field_values(void* args, kaction_t action, tuple_t pos); // ��������� ���� ��� ��������������
gupdate_t dialog_box(void* args, kaction_t action, tuple_t pos); // ���������� ����

gupdate_t load_file(void* args, kaction_t action, tuple_t pos); // ��������� ���� � ����� data
gupdate_t mainmenu(void* args, kaction_t action, tuple_t pos);
gupdate_t settings(void* args, kaction_t action, tuple_t pos); 
gupdate_t game_loop(void* args, kaction_t action, tuple_t pos);

/*
* ������� check_field - ��������� �������� ���� 
*  ���������� ������
*  param - ��������� �� ������� ���� 
*/

gstate_t check_field(field_t* param);

/*
* ������� get_keyboard_input - ������������ �������� ������� �� ������������ (�������).
*	��������� ��������� �����: ��� kaction_t 
*	
*  pos - ��������� �������.
*	max - ������� ��� ������� 
*/

kaction_t get_keyboard_input(tuple_t* pos, tuple_t max);

/*
* ������� set_axies - ����������� (�����������) �������� � ��������� ������, 
*	�������������� ������� ������ � ������ ������, ������� ����������� �����, � ������� ����� ��������� ������.
*	���������� ������.
*  pos - ��������� ������� 
*  ptr_param - ��������� �� ������� ���� 
*/

bool set_axies(tuple_t pos, field_t* ptr_param);

/*
* ������� file_data - ��������� ���� �� ���������� ����� ��� ���������� �� field � ����
*  ���������� ������
*  field - ���������, ������� ����� ������� �������� ������ ����
*	readonly - true=���������� � field �� �����, false=���������� �� field � ����.
*/

bool file_data(field_t* field, bool readonly);

/*
* ������� read_files_in_dir - ��������� ��� ��������� ����� � �������� ������ � ���������� data
*  ���������� ������
*  param - ���������, ������� ����� ������� �������� ������
*/

bool read_files_in_dir(dir_t* param);