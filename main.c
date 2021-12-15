#include "src/kuromasu.h"

int main(void) 
{
	setlocale(LC_ALL, "rus");

	char* setup_path = (char*)calloc(261, sizeof(char));
	sprintf(setup_path, "mkdir %s", PATH_NAME);
	system(setup_path);

	update_frame(mainmenu, (tuple_t) { 0, 3 }, FALSE, &(field_t){ .name = "data" });
	return 0;
}