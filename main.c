#include "src/kuromasu.h"

int main(void) 
{
	setlocale(LC_ALL, "rus");
	system("mkdir data");

	field_t field = { .name = "data.txt" };
	update_frame(mainmenu, tuple_c(0, 3), FALSE, &field);

	return 0;
}