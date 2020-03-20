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

#ifndef HEADER_STRLIST
#define HEADER_STRLIST

	#include <iostream>
	#include <fstream>
	#include <unistd.h>
	#include <string>
	#include <vector>
	#include <list>

	#include <stack>
	#include <list>
	#include <expr.hpp>
	#include <error.hpp>

//-----------------------------------------------------------------------------

class StringList {
public:
	StringList(void);
	~StringList(void);
	void addstring(std::string str);
	void reset(void);
	void pop(void);
	void push(std::string str);
	std::string top(void);
	std::string current(void);
	std::string next(void);
	int get_nstrings(void);
	StringList* copy(void);

public:
	std::vector<std::string>	strings;	// List of strings
	int 						nstrings;	// Number of strings
	int							cstring;	// Current stringlist index
};

#endif
