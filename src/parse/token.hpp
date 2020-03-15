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

typedef enum {
	TOKEN_VOID = -2,
	TOKEN_EOF  = -1,
	TOKEN_ERROR = 0,
	TOKEN_BLOCK_END,
	TOKEN_OUTDENT,
	TOKEN_ID,
	TOKEN_INT,
	TOKEN_STRING,
	
	// Single character tokens (any ascii character is token)
	
	TOKEN_EOL = 10,
	TOKEN_ADR = '@',
	TOKEN_EQUAL = '=',
	TOKEN_HIGHER = '>',
	TOKEN_LOWER = '<',
	TOKEN_COMMA = ',',
	TOKEN_COLON = ':',
	TOKEN_PERCENT = '%',
	TOKEN_OPEN_P  = '(',
	TOKEN_CLOSE_P = ')',
	TOKEN_PLUS = '+',
	TOKEN_MINUS = '-',
	TOKEN_MUL = '*',
	TOKEN_DIV = '/',
	TOKEN_DOT = '.',
	TOKEN_HASH = '#',
	TOKEN_DOLLAR = '$',
	TOKEN_BYTE_INDEX = TOKEN_DOLLAR,
	
	// Keyword tokens
	
	TOKEN_KEYWORD = 128,
	TOKEN_GOTO    = 128,
	TOKEN_IF,
	TOKEN_UNLESS,
	TOKEN_THEN,
	TOKEN_ELSE,
	TOKEN_PROC,
	TOKEN_RULE,
	TOKEN_MACRO,
	TOKEN_AND,
	TOKEN_OR,
	TOKEN_NOT,
	TOKEN_SQRT,
	TOKEN_WHILE,
	TOKEN_UNTIL,
	TOKEN_WHERE,
	TOKEN_CONST,
	TOKEN_ENUM,
	TOKEN_ARRAY,
	TOKEN_TYPE2,			// this should be TOKEN_TYPE, but there was conflict with windows.h include on Windows
	TOKEN_FILE,
	TOKEN_LO,
	TOKEN_HI,
	TOKEN_OF,
	TOKEN_FOR,
	TOKEN_IN,
	TOKEN_OUT,
	TOKEN_PARAM,
	TOKEN_INSTR,
	TOKEN_TIMES,
	TOKEN_ADR2,
	TOKEN_DEBUG,
	TOKEN_MOD,
	TOKEN_BITNOT,
	TOKEN_BITAND,
	TOKEN_BITOR,
	TOKEN_BITXOR,
	TOKEN_STRUCT,
	TOKEN_USE,
	TOKEN_REF,
	TOKEN_STEP,
	TOKEN_RETURN,
	TOKEN_SCOPE,
	TOKEN_SEQUENCE,
	TOKEN_ASSERT,
	TOKEN_EITHER,
	TOKEN_STRING_TYPE,
	TOKEN_LAST_KEYWORD = TOKEN_STRING_TYPE,
	
	// two character tokens
	TOKEN_LOWER_EQUAL,
	TOKEN_HIGHER_EQUAL,
	TOKEN_NOT_EQUAL,
	TOKEN_DOTDOT,
	TOKEN_RIGHT_ARROW,
	TOKEN_HORIZ_RULE
} token_t;

//-----------------------------------------------------------------------------

#endif
