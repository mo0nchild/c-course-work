#include "src/kuromasu.h"

int main(void) 
{
	setlocale(LC_ALL, "rus");

	char* setup_path = (char*)calloc(261, sizeof(char));
	sprintf(setup_path, "mkdir %s", PATH_NAME);
	system(setup_path);

	field_t init_field = (field_t){
		.name = "data",
		.array = (cell_t*)malloc(sizeof(cell_t)),
		.size = 1
	};

	update_frame(mainmenu, (tuple_t) { 0, 3 }, FALSE, &init_field);
	return 0;
}
