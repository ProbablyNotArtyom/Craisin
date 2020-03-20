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
	#include <list>

	#include <cpu.hpp>
	#include <craisin.hpp>
	#include <error.hpp>
	#include <utils.hpp>

	#include <expr.hpp>
	#include <token.hpp>

	using namespace std;

//-----------------------------------------------------------------------------

void craisin_pass_1(craisin_state_t *as) {
	std::ifstream file(as->input_files->current());
	char *data;
	std::list<std::string> lines;

	if (file.is_open()) {
		file.seekg(0, file.end);
	    int size = file.tellg();
	    file.seekg(0, file.beg);

		// Read in all lines of file
		while (file.tellg() < size) {
			std::string tmpline;
			std::getline(file, tmpline);
			lines.push_back(tmpline);
		}
	}

	// Iterate over each line
	for (std::string line : lines) {
		size_t index = line.find(":");
		if (index != std::string::npos) {
			// Found a possible match
			std::string token = line.substr(0, index);
			// Check if the substring has any whitespace
			if (token.find(" ") == std::string::npos) {
				Utils::debug_printf("[LABEL] %s\n", token.c_str());
			}
		}
	}
}

//-----------------------------------------------------------------------------
