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

#include <language.hpp>
#include <ctype.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string.h>
#include <list>

#include <argparse.hpp>
#include <cpu.hpp>
#include <craisin.hpp>

using namespace std;

//-----------------------------------------------------------------------------	

expr_t craisin_expr_create(void) { }
void craisin_expr_destroy(expr_t E) { }
expr_t craisin_expr_copy(expr_t E) { }
void craisin_expr_add_operand(expr_t E, expr_t O) { }
expr_t craisin_expr_build(int exprtype, ...) { }
char *craisin_expr_print(expr_t E) { }
int craisin_expr_compare(expr_t E1, expr_t E2) { }
void craisin_expr_simplify(expr_t E, void *priv) { }

void craisin_save_expr(line_t *cl, int id, expr_t expr) { }
expr_t craisin_fetch_expr(line_t *cl, int id) { }
expr_t craisin_parse_expr(craisin_state_t *as, char **p) { }
int craisin_emitexpr(line_t *cl, expr_t expr, int s) { }
int craisin_reduce_expr(craisin_state_t *as, expr_t expr) { }
expr_t craisin_parse_cond(craisin_state_t *as, char **p) { }

void craisin_expr_set_special_handler(expr_handler1_t *fn) { }
void craisin_expr_set_var_handler(expr_handler2_t *fn) { }
void craisin_expr_set_term_parser(expr_handler3_t *fn) { }

expr_t craisin_expr_parse(char **p, void *priv) { }
expr_t craisin_expr_parse_compact(char **p, void *priv) { }
int craisin_expr_istype(expr_t e, int t) { }
int craisin_expr_intval(expr_t e) { }
int craisin_expr_specint(expr_t e) { }
void *craisin_expr_specptr(expr_t e) { }
int craisin_expr_whichop(expr_t e) { }

int expr_type(expr_t e) { }
int craisin_expr_operandcount(expr_t e) { }
void craisin_expr_setwidth(int w) { }

// run a function on all terms in an expression; if the function
// returns non-zero for any term, return non-zero, else return
// zero
int expr_testterms(expr_t e, expr_tester_t *fn, void *priv) { }
void craisin_expr_setdivzero(void (*fn)(void *priv)) { }
