#include "kuromasu.h"
//#include <stdio.h>
//#include <stdarg.h>

int main(void) 
{
	setlocale(LC_ALL, "rus");
	system("mkdir data");

	field_t field = { .name = "data.txt" };
	update_frame(mainmenu, tuple_c(0, 3), FALSE, &field);


	/*for(int i = 0; i < 120; i++) 
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), i);
		printf("%d\n", i);
	}*/

	return 0;
}