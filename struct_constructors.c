#include "kuromasu.h"

gupdate_t gupdate_c(fupdate_t frame_update_state, void* return_value) 
{
	gupdate_t constructor = { frame_update_state, return_value };
	return constructor;
}

tuple_t tuple_c(int x, int y)
{
	tuple_t constructor = { x, y };
	return constructor;
}

cell_t cell_c(int check, int free, color_t color)
{
	cell_t constructor = { tuple_c(free, free), check ,color };
	return constructor;
}

dir_t dir_c(name_t* dir, int size)
{
	dir_t constructor = { dir, size };
	return constructor;
}

field_t field_c(cell_t* array, name_t name, int size)
{
	field_t constructor = { array, name, size };
	return constructor;
}