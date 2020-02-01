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

#ifndef HEADER_EXPRESSION
#define HEADER_EXPRESSION

//-----------------------------------------------------------------------------

enum {
	CRAISIN_EXPR_TYPE_OPER,		// operator term
	CRAISIN_EXPR_TYPE_INT,		// integer
	CRAISIN_EXPR_TYPE_VAR,		// a "variable" (string for the name)
	CRAISIN_EXPR_TYPE_SPECIAL	// a "special" reference (user defined)
};

enum {
	CRAISIN_EXPR_OP_PLUS = 1,	// addition
	CRAISIN_EXPR_OP_MINUS,		// subtraction
	CRAISIN_EXPR_OP_TIMES,		// multiplication
	CRAISIN_EXPR_OP_DIVIDE,		// division
	CRAISIN_EXPR_OP_MOD,		// modulus
	CRAISIN_EXPR_OP_INTDIV,		// integer division
	CRAISIN_EXPR_OP_BWAND,		// bitwise and
	CRAISIN_EXPR_OP_BWOR,		// bitwise or
	CRAISIN_EXPR_OP_BWXOR,		// bitwise xor
	CRAISIN_EXPR_OP_AND,		// boolean and
	CRAISIN_EXPR_OP_OR,			// boolean or
	CRAISIN_EXPR_OP_NEG,		// unary negation, 2's complement
	CRAISIN_EXPR_OP_COM,		// unary 1's complement
	CRAISIN_EXPR_OP_COM8,		// 8 bit complement
	CRAISIN_EXPR_OP_NONE = 0
};

//-----------------------------------------------------------------------------

typedef struct expr_internal *expr_t;

struct expr_operands {
	expr_t 					ptr;		// Pointer to expression
	struct expr_operands 	*next;
};

struct expr_internal {
	int 					type;		// Term type
	int 					value;		// Value as Integer
	void 					*value2;	// Value as Pointer
	struct expr_operands 	*operands;	// Pointer to operand list, if applicable
};

typedef expr_t expr_handler1_t(int type, void *ptr, void *expr);
typedef expr_t expr_handler2_t(char *var, void *expr);
typedef expr_t expr_handler3_t(char **parse, void *expr);
typedef int expr_tester_t(expr_t ptr, void *expr);

//-----------------------------------------------------------------------------

#endif

