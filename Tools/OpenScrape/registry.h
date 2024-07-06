//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose:
//
//******************************************************************************



class Registry 
{
public:
	Registry(void);

	void read_reg(void);
	void write_reg(void);

	// Window locations and sizes
	int main_x, main_y, main_dx, main_dy;
	int tablemap_x, tablemap_y, tablemap_dx, tablemap_dy;
	int grhash_x, grhash_y, grhash_dx, grhash_dy;

	// Region grouping
	int region_grouping;

};

