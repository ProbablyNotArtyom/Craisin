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

expr_t lwexpr_create(void);
void craisin_expr_destroy(expr_t E);
expr_t craisin_expr_copy(expr_t E);
void craisin_expr_add_operand(expr_t E, expr_t O);
expr_t craisin_expr_build(int exprtype, ...);
char *craisin_expr_print(expr_t E);
int craisin_expr_compare(expr_t E1, expr_t E2);
void craisin_expr_simplify(expr_t E, void *priv);

void craisin_expr_set_special_handler(expr_handler1_t *fn);
void craisin_expr_set_var_handler(expr_handler2_t *fn);
void craisin_expr_set_term_parser(expr_handler3_t *fn);

expr_t craisin_expr_parse(char **p, void *priv);
expr_t craisin_expr_parse_compact(char **p, void *priv);
int craisin_expr_istype(expr_t e, int t);
int craisin_expr_intval(expr_t e);
int craisin_expr_specint(expr_t e);
void *craisin_expr_specptr(expr_t e);
int craisin_expr_whichop(expr_t e);

int expr_type(expr_t e);

int craisin_expr_operandcount(expr_t e);

void craisin_expr_setwidth(int w);

// run a function on all terms in an expression; if the function
// returns non-zero for any term, return non-zero, else return
// zero
int expr_testterms(expr_t e, expr_tester_t *fn, void *priv);

void craisin_expr_setdivzero(void (*fn)(void *priv));

//-----------------------------------------------------------------------------

#endif

