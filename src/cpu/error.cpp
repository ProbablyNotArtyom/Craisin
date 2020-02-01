/*
 *	Craisin - 6502/65816 assembler with flexible syntax
 *	Copyright (C) 2019  Carson Herrington
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>

#include <craisin.hpp>
#include <error.hpp>

//-----------------------------------------------------------------------------

using namespace std;
static craisin_error_fn error_handler = nullptr;
void craisin_error(const string error) {
	if (error_handler) (*error_handler)(error);
	else {
		cerr << error.c_str();
	}
	exit(1);
}

void set_error_fn(craisin_error_fn func) {
	error_handler = func;
}

