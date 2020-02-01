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

#ifndef HEADER_TOKEN
#define HEADER_TOKEN

#include <string>
#include <deque>
#include "types.hpp"
#include "expr.hpp"

//-----------------------------------------------------------------------------

class Token {
public:
	Token();
	Token(expr_t type);
	Token(int val);
	Token(expr_t type, const std::string str);
	
	const expr_t 				get_type();
	const std::string 			get_str();
	const int					get_val();
	const struct expr_operands	get_operands();
private:
	expr_t 					type;
	std::string 			str;
	int						val;
	struct expr_operands 	*operators;
};

//-----------------------------------------------------------------------------

#endif
