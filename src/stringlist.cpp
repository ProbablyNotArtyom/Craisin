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

#ifndef HEADER_STRINGLIST
#define HEADER_STRINGLIST

	#include <iostream>
	#include <fstream>
	#include <unistd.h>
	#include <string>
	#include <vector>
	#include <list>

	#include <cpu.hpp>
	#include <craisin.hpp>
	#include <error.hpp>

	#include <expr.hpp>
	#include <token.hpp>
	#include <stringlist.hpp>

//-----------------------------------------------------------------------------

StringList::StringList(void) {
	strings.clear();
	nstrings = 0;
	cstring = 0;
}

StringList::~StringList() {}

void StringList::addstring(std::string str) {
	strings.push_back(str);
	nstrings++;
}

void StringList::reset(void) {
	cstring = 0;
}

void StringList::pop(void) {
	strings.pop_back();
	nstrings--;
}

std::string StringList::top(void) {
	strings.back();
}

void StringList::push(std::string str) {
	strings.push_back(str);
	nstrings++;
}

std::string StringList::current(void) {
	if (cstring >= nstrings) return nullptr;
	return strings[cstring];
}

std::string StringList::next(void) {
	cstring++;
	return current();
}

int StringList::get_nstrings(void) {
	return nstrings;
}

StringList* StringList::copy(void) {
	StringList *r = new StringList();

	if (nstrings) {
		for (int i = 0; i < nstrings; i++) r->addstring(strings[i]);
	}
	return r;
}

#endif
