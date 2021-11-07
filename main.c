#include "src/kuromasu.h"

int main(void) 
{
	setlocale(LC_ALL, "rus");

	const char* setup_path = (char*)calloc(261, sizeof(char));
	sprintf(setup_path, "mkdir %s", path_name);
	system(setup_path);

	field_t field = { .name = "data.txt" };
	update_frame(mainmenu, tuple_c(0, 3), FALSE, &field);

	return 0;
}