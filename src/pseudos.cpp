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
#include <error.hpp>

using namespace std;

static int cstringlen(craisin_state_t * as, line_t * ln, char **p, char delim);

//-----------------------------------------------------------------------------

// for "end"
PARSEFUNC(pseudo_parse_end) {
	expr_t addr;

	l->len = 0;

	as->endseen = 1;

	if (!**p) {
		addr = craisin_expr_build(CRAISIN_EXPR_TYPE_INT, 0);
	} else {
		addr = craisin_parse_expr(as, p);
	}
	if (!addr) {
		//set_error_fn(as, as->cl, E_EXPRESSION_BAD);
		addr = craisin_expr_build(CRAISIN_EXPR_TYPE_INT, 0);
	}
	craisin_save_expr(l, 0, addr);
}

EMITFUNC(pseudo_emit_end) {
	expr_t addr;

	addr = craisin_fetch_expr(l, 0);

	if (addr) {
		if (!craisin_expr_istype(addr, CRAISIN_EXPR_TYPE_INT));
			//set_error_fn(as, l, E_EXEC_ADDRESS);
		else
			as->execaddr = craisin_expr_intval(addr);
	}
	as->endseen = 1;
}

PARSEFUNC(pseudo_parse_int8) {
	int i = 0;
	expr_t e;

	for (;;) {
		e = craisin_parse_expr(as, p);
		if (!e) {
			//set_error_fn2(as, l, E_EXPRESSION_BAD, "(#%d)", i);
			break;
		}
		craisin_save_expr(l, i++, e);
		if (**p != ',')
			break;
		(*p)++;
	}

	l->len = i;
}

EMITFUNC(pseudo_emit_int8) {
	int i;
	expr_t e;
//      int v;

	for (i = 0; i < l->len; i++) {
		e = craisin_fetch_expr(l, i);
		craisin_emitexpr(l, e, 1);
	}
}

PARSEFUNC(pseudo_parse_int16) {
	int i = 0;
	expr_t e;

	for (;;) {
		e = craisin_parse_expr(as, p);
		if (!e) {
			//set_error_fn2(as, l, E_EXPRESSION_BAD, "(#%d)", i);
			break;
		}
		craisin_save_expr(l, i++, e);
		if (**p != ',')
			break;
		(*p)++;
	}

	l->len = i * 2;
}

EMITFUNC(pseudo_emit_int16) {
	int i;
	expr_t e;
//      int v;

	for (i = 0; i < (l->len) / 2; i++) {
		e = craisin_fetch_expr(l, i);
		craisin_emitexpr(l, e, 2);
	}
}

PARSEFUNC(pseudo_parse_int32) {
	int i = 0;
	expr_t e;

	for (;;) {
		e = craisin_parse_expr(as, p);
		if (!e) {
			//set_error_fn2(as, l, E_EXPRESSION_BAD, "(#%d)", i);
			break;
		}
		craisin_save_expr(l, i++, e);
		if (**p != ',')
			break;
		(*p)++;
	}

	l->len = i * 4;
}

EMITFUNC(pseudo_emit_int32) {
	int i;
	expr_t e;
//      int v;

	for (i = 0; i < (l->len) / 4; i++) {
		e = craisin_fetch_expr(l, i);
		craisin_emitexpr(l, e, 4);
	}
}

PARSEFUNC(pseudo_parse_ascii) {
	char delim;
	int i;

	if (!**p) {
		//set_error_fn(as, l, E_OPERAND_BAD);
		return;
	}

	delim = **p;
	(*p)++;

	i = cstringlen(as, l, p, delim);

	if (**p != delim) {
		//set_error_fn(as, l, E_OPERAND_BAD);
		return;
	}
	(*p)++;
	l->len = i;
}

EMITFUNC(pseudo_emit_ascii) {
	int i;
	expr_t e;

	for (i = 0; i < l->len - l->fcc_extras; i++)
		craisin_emit(l, l->lstr[i]);

/* PRAGMA_M80EXT */
	for (i = 0; i < l->fcc_extras; i++) {
		e = craisin_fetch_expr(l, i);
		craisin_emitexpr(l, e, 1);
	}
}

PARSEFUNC(pseudo_parse_asciis) {
	char delim;
	int i;

	if (!**p) {
		//set_error_fn(as, l, E_OPERAND_BAD);
		return;
	}

	delim = **p;
	(*p)++;

	i = cstringlen(as, l, p, delim);

	if (**p != delim) {
		//set_error_fn(as, l, E_OPERAND_BAD);
		return;
	}
	(*p)++;
	l->len = i;
}

EMITFUNC(pseudo_emit_asciis) {
	int i;

	for (i = 0; i < l->len - 1; i++)
		craisin_emit(l, l->lstr[i]);
	craisin_emit(l, l->lstr[i] | 0x80);
}

PARSEFUNC(pseudo_parse_asciiz) {
	char delim;
	int i;

	if (!**p) {
		//set_error_fn(as, l, E_OPERAND_BAD);
		return;
	}

	delim = **p;
	(*p)++;

	i = cstringlen(as, l, p, delim);

	if (**p != delim) {
		//set_error_fn(as, l, E_OPERAND_BAD);
		return;
	}
	(*p)++;
	l->len = i + 1;
}

EMITFUNC(pseudo_emit_asciiz) {
	int i;

	for (i = 0; i < (l->len - 1); i++)
		craisin_emit(l, l->lstr[i]);
	craisin_emit(l, 0);
}

// Reserve undefined bytes on the output
PARSEFUNC(pseudo_parse_res) {
	expr_t expr;

	expr = craisin_parse_expr(as, p);
	if (!expr) {
		//set_error_fn(as, l, E_EXPRESSION_BAD);
	}

	l->lint = 0;
	if (as->instruct) {
		craisin_reduce_expr(as, expr);
		if (!craisin_expr_istype(expr, CRAISIN_EXPR_TYPE_INT)) {
			//set_error_fn(as, l, E_EXPRESSION_NOT_CONST);
		} else {
			int e;
			e = craisin_expr_intval(expr);
			register_struct_entry(as, l, e, NULL);
			l->len = 0;
			l->lint = 1;
			l->symset = 1;
		}
	} else {
		if (l->inmod) {
			l->dlen = -1;
			l->len = 0;
		}
	}
	craisin_save_expr(l, 0, expr);
}

RESOLVEFUNC(pseudo_resolve_res) {
	expr_t expr;

	if (l->lint)
		return;

	if (l->inmod) {
		if (l->dlen >= 0)
			return;
	} else {
		if (l->len >= 0)
			return;
	}

	expr = craisin_fetch_expr(l, 0);

	if (craisin_expr_istype(expr, CRAISIN_EXPR_TYPE_INT)) {
		if (craisin_expr_intval(expr) < 0) {
			//set_error_fn2(as, l, E_NEGATIVE_RESERVATION, "(%d)", craisin_expr_intval(expr));
			l->len = 0;
			l->dlen = 0;
			return;
		}
		if (l->inmod)
			l->dlen = craisin_expr_intval(expr);
		else
			l->len = craisin_expr_intval(expr);
	}
}

PARSEFUNC(pseudo_parse_zero) {
	expr_t expr;

	expr = craisin_parse_expr(as, p);
	if (!expr) {
		//set_error_fn(as, l, E_EXPRESSION_BAD);
	}

	craisin_save_expr(l, 0, expr);
}

RESOLVEFUNC(pseudo_resolve_zero) {
	expr_t expr;

	if (l->len >= 0)
		return;

	expr = craisin_fetch_expr(l, 0);

	if (craisin_expr_istype(expr, CRAISIN_EXPR_TYPE_INT)) {
		if (craisin_expr_intval(expr) < 0) {
			//set_error_fn2(as, l, E_NEGATIVE_BLOCKSIZE, "(%d)", craisin_expr_intval(expr));
			l->len = 0;
			return;
		}
		l->len = craisin_expr_intval(expr);
	}
}

EMITFUNC(pseudo_emit_zero) {
	int i;

	if (l->len < 0) {
		//set_error_fn(as, l, E_EXPRESSION_NOT_CONST);
		return;
	}

	for (i = 0; i < l->len; i++)
		craisin_emit(l, 0);
}

//-----------------------------------------------------------------------------	

PARSEFUNC(pseudo_parse_org) {
	expr_t e;

	l->len = 0;

	e = craisin_parse_expr(as, p);
	if (!e) {
		//set_error_fn(as, l, E_OPERAND_BAD);
		return;
	}

	craisin_expr_destroy(l->daddr);
	l->daddr = e;

	if (l->inmod == 0) {
		craisin_expr_destroy(l->addr);
		l->addr = e;
	}
	l->len = 0;
}

PARSEFUNC(pseudo_parse_equ) {
	expr_t e;

	l->len = 0;

	if (!(l->sym)) {
		//set_error_fn(as, l, E_SYMBOL_MISSING);
		return;
	}

	e = craisin_parse_expr(as, p);
	if (!e) {
		//set_error_fn(as, l, E_OPERAND_BAD);
		return;
	}

	register_symbol(as, l, l->sym, e, symbol_flag_none);
	l->symset = 1;
	l->dptr = lookup_symbol(as, l, l->sym);
	craisin_expr_destroy(e);
}

PARSEFUNC(pseudo_parse_set) {
	expr_t e;

	l->len = 0;

	if (!(l->sym)) {
		//set_error_fn(as, l, E_SYMBOL_MISSING);
		return;
	}

	e = craisin_parse_expr(as, p);
	if (!e) {
		//set_error_fn(as, l, E_OPERAND_BAD);
		return;
	}

	register_symbol(as, l, l->sym, e, symbol_flag_set);
	l->symset = 1;
	l->dptr = lookup_symbol(as, l, l->sym);
	craisin_expr_destroy(e);
}

PARSEFUNC(pseudo_parse_ifeq) {
	expr_t e;

	l->len = 0;
	l->hideline = 1;

	if (as->skipcond && !(as->skipmacro)) {
		as->skipcount++;
		skip_operand(p);
		return;
	}

	e = craisin_parse_cond(as, p);
	if (e)
		craisin_reduce_expr(as, e);
	if (e && craisin_expr_intval(e) != 0) {
		as->skipcond = 1;
		as->skipcount = 1;
	}
}

PARSEFUNC(pseudo_parse_ifne) {
	expr_t e;

	l->len = 0;
	l->hideline = 1;

	if (as->skipcond && !(as->skipmacro)) {
		as->skipcount++;
		skip_operand(p);
		return;
	}

	e = craisin_parse_cond(as, p);
	if (e)
		craisin_reduce_expr(as, e);
	if (e && craisin_expr_intval(e) == 0) {
		as->skipcond = 1;
		as->skipcount = 1;
	}
}


PARSEFUNC(pseudo_parse_ifgt) {
	expr_t e;

	l->len = 0;
	l->hideline = 1;

	if (as->skipcond && !(as->skipmacro)) {
		as->skipcount++;
		skip_operand(p);
		return;
	}

	e = craisin_parse_cond(as, p);
	if (e)
		craisin_reduce_expr(as, e);
	if (e && craisin_expr_intval(e) <= 0) {
		as->skipcond = 1;
		as->skipcount = 1;
	}
}

PARSEFUNC(pseudo_parse_ifge) {
	expr_t e;

	l->len = 0;
	l->hideline = 1;

	if (as->skipcond && !(as->skipmacro)) {
		as->skipcount++;
		skip_operand(p);
		return;
	}

	e = craisin_parse_cond(as, p);
	if (e)
		craisin_reduce_expr(as, e);
	if (e && craisin_expr_intval(e) < 0) {
		as->skipcond = 1;
		as->skipcount = 1;
	}
}

PARSEFUNC(pseudo_parse_iflt) {
	expr_t e;

	l->len = 0;
	l->hideline = 1;

	if (as->skipcond && !(as->skipmacro)) {
		as->skipcount++;
		skip_operand(p);
		return;
	}

	e = craisin_parse_cond(as, p);
	if (e)
		craisin_reduce_expr(as, e);
	if (e && craisin_expr_intval(e) >= 0) {
		as->skipcond = 1;
		as->skipcount = 1;
	}
}

PARSEFUNC(pseudo_parse_ifle) {
	expr_t e;

	l->hideline = 1;
	l->len = 0;

	if (as->skipcond && !(as->skipmacro)) {
		as->skipcount++;
		skip_operand(p);
		return;
	}

	e = craisin_parse_cond(as, p);
	if (e)
		craisin_reduce_expr(as, e);
	if (e && craisin_expr_intval(e) > 0) {
		as->skipcond = 1;
		as->skipcount = 1;
	}
}

PARSEFUNC(pseudo_parse_endif) {
	l->hideline = 1;
	l->len = 0;
	skip_operand(p);
	if (as->skipcond && !(as->skipmacro)) {
		as->skipcount--;
		if (as->skipcount <= 0)
			as->skipcond = 0;
	}
}

PARSEFUNC(pseudo_parse_else) {
	l->len = 0;
	l->hideline = 1;
	skip_operand(p);

	if (as->skipmacro)
		return;

	if (as->skipcond) {
		if (as->skipcount == 1) {
			as->skipcount = 0;
			as->skipcond = 0;
		}
		return;
	}
	as->skipcond = 1;
	as->skipcount = 1;
}

PARSEFUNC(pseudo_parse_ifdef) {
	char *sym;
	int i;
	craisin_symbol_t *s;

	l->len = 0;
	l->hideline = 1;

	if (as->skipcond && !(as->skipmacro)) {
		as->skipcount++;
		skip_operand(p);
		return;
	}

      again:
	for (i = 0; (*p)[i] && !isspace((*p)[i]) && (*p)[i] != '|' && (*p)[i] != '&'; i++)
	/* do nothing */ ;

	sym = strndup(*p, i);
	(*p) += i;

	s = lookup_symbol(as, l, sym);

	free(sym);

	if (!s) {
		if (**p == '|') {
			(*p)++;
			goto again;
		}
		as->skipcond = 1;
		as->skipcount = 1;
	}
	skip_operand(p);
}

PARSEFUNC(pseudo_parse_ifndef) {
	char *sym;
	int i;
	craisin_symbol_t *s;

	l->len = 0;
	l->hideline = 1;

	if (as->skipcond && !(as->skipmacro)) {
		as->skipcount++;
		skip_operand(p);
		return;
	}
	for (i = 0; (*p)[i] && !isspace((*p)[i]) && (*p)[i] != '&' && (*p)[i] != '|'; i++)
	/* do nothing */ ;

	sym = strndup(*p, i);
	(*p) += i;

	s = lookup_symbol(as, l, sym);

	free(sym);

	if (s) {
		as->skipcond = 1;
		as->skipcount = 1;
		return;
	}
}

PARSEFUNC(pseudo_parse_ifpragma) {
	char *pstr;
	int i;
	int pragma;
	int compare;

	l->len = 0;
	l->hideline = 1;

	if (as->skipcond && !(as->skipmacro)) {
		as->skipcount++;
		skip_operand(p);
		return;
	}

      again:
	for (i = 0; (*p)[i] && !isspace((*p)[i]) && (*p)[i] != '|' && (*p)[i] != '&'; i++)
	/* do nothing */ ;

	pstr = strndup(*p, i);
	(*p) += i;

	pragma = parse_pragma_helper(pstr);
	if (!pragma)
		//set_error_fn(as, l, E_PRAGMA_UNRECOGNIZED);

	free(pstr);

	if (pragma & PRAGMA_CLEARBIT) {
		pragma &= ~PRAGMA_CLEARBIT;	/* strip off flag bit */
		compare = l->pragmas & pragma ? 0 : 1;
	} else {
		compare = l->pragmas & pragma;
	}

	if (!compare) {
		if (**p == '|') {
			(*p)++;
			goto again;
		}
		as->skipcond = 1;
		as->skipcount = 1;
	}
	skip_operand(p);
}

PARSEFUNC(pseudo_parse_error) {
	//set_error_fn2(as, l, E_USER_SPECIFIED, "%s", *p);
	skip_operand(p);
}

PARSEFUNC(pseudo_parse_warning) {
	//set_error_fn2(as, l, W_USER_SPECIFIED, "%s", *p);
	l->len = 0;
	skip_operand(p);
}

PARSEFUNC(pseudo_parse_incbin) {
	char *fn, *p2;
	int delim = 0;
	FILE *fp;
	long flen;
	char *rfn;

	if (!**p) {
		//set_error_fn(as, l, E_FILENAME_MISSING);
		return;
	}

	if (**p == '"' || **p == '\'') {
		delim = **p;
		(*p)++;

		for (p2 = *p; *p2 && (*p2 != delim); p2++)
		/* do nothing */ ;
	} else {
		for (p2 = *p; *p2 && !isspace(*p2); p2++)
		/* do nothing */ ;
	}
	fn = strndup(*p, p2 - *p);
	*p = p2;
	if (delim && **p)
		(*p)++;

	fp = input_open_standalone(as, fn, &rfn);
	if (!fp) {
		//set_error_fn(as, l, E_FILE_OPEN);
		free(fn);
		return;
	}

	l->lstr = rfn;

	fseek(fp, 0, SEEK_END);
	flen = ftell(fp);
	fclose(fp);

	l->len = flen;
}

EMITFUNC(pseudo_emit_incbin) {
	FILE *fp;
	int c;

	fp = fopen(l->lstr, "rb");
	if (!fp) {
		//set_error_fn2(as, l, E_FILE_OPEN, "%s", "(emit)!");
		return;
	}

	for (;;) {
		c = fgetc(fp);
		if (c == EOF) {
			fclose(fp);
			return;
		}
		craisin_emit(l, c);
	}
}

PARSEFUNC(pseudo_parse_include) {
	char *fn, *p2;
	char *p3;
	int delim = 0;
	int len;
	char buf[110];

	if (!**p) {
		//set_error_fn(as, l, E_FILENAME_MISSING);
		return;
	}

	if (**p == '"' || **p == '\'') {
		delim = **p;
		(*p)++;

		for (p2 = *p; *p2 && *p2 != delim; p2++)
		/* do nothing */ ;
	} else {
		for (p2 = *p; *p2 && !isspace(*p2); p2++)
		/* do nothing */ ;
	}
	fn = strndup(*p, p2 - *p);
	(*p) = p2;
	if (delim && **p)
		(*p)++;

/* add a book-keeping entry for line numbers */
	snprintf(buf, 100, "\001\001SETLINENO %d\n", l->lineno + 1);
	input_openstring(as, "INTERNAL", buf);

	len = strlen(fn) + 8;
	p3 = (char*)malloc(len + 1);
	sprintf(p3, "include:%s", fn);
	as->fileerr = 0;
	input_open(as, p3);
	free(p3);

	if (as->fileerr == 0)
		l->hideline = 1;
	l->len = 0;
	free(fn);
}

PARSEFUNC(pseudo_parse_align) {
	expr_t e;
	if (!**p) {
		//set_error_fn(as, l, E_OPERAND_BAD);
		return;
	}

	e = craisin_parse_expr(as, p);

	if (!e) {
		//set_error_fn(as, l, E_OPERAND_BAD);
		return;
	}

	craisin_save_expr(l, 0, e);

	if (**p == ',') {
		(*p)++;
		e = craisin_parse_expr(as, p);
	} else {
		e = craisin_expr_build(CRAISIN_EXPR_TYPE_INT, 0);
	}
	if (!e) {
		//set_error_fn(as, l, E_PADDING_BAD);
		return;
	}

	craisin_save_expr(l, 1, e);
}

RESOLVEFUNC(pseudo_resolve_align) {
	expr_t e;
	int align = 1;
	expr_t te;
	int a;

	e = craisin_fetch_expr(l, 0);

	if (craisin_expr_istype(e, CRAISIN_EXPR_TYPE_INT)) {
		align = craisin_expr_intval(e);
		if (align < 1) {
			//set_error_fn(as, l, E_ALIGNMENT_INVALID);
			return;
		}
	} else {
		return;
	}


	te = craisin_expr_copy(l->addr);
	as->exportcheck = 1;
	craisin_reduce_expr(as, te);
	as->exportcheck = 0;

	if (craisin_expr_istype(te, CRAISIN_EXPR_TYPE_INT)) {
		a = craisin_expr_intval(te);
	} else {
		a = -1;
	}
	craisin_expr_destroy(te);

	if (a >= 0) {
		if (a % align == 0) {
			l->len = 0;
			return;
		}
		l->len = align - (a % align);
		return;
	}
}

EMITFUNC(pseudo_emit_align) {
	expr_t e;
	int i;

	if (l->csect && (l->csect->flags & (section_flag_bss | section_flag_constant)))
		return;
	e = craisin_fetch_expr(l, 1);
	for (i = 0; i < l->len; i++) {
		craisin_emitexpr(l, e, 1);
	}
}

PARSEFUNC(pseudo_parse_fill) {
	expr_t e, e1;
	if (!**p) {
		//set_error_fn(as, l, E_OPERAND_BAD);
		return;
	}

	e1 = craisin_parse_expr(as, p);
	if (**p != ',') {
		//set_error_fn(as, l, E_OPERAND_BAD);
		return;
	}
	(*p)++;
	e = craisin_parse_expr(as, p);

	if (!e) {
		//set_error_fn(as, l, E_OPERAND_BAD);
		return;
	}

	craisin_save_expr(l, 0, e);
	craisin_save_expr(l, 1, e1);

	if (!e1) {
		//set_error_fn(as, l, E_PADDING_BAD);
		return;
	}
}

RESOLVEFUNC(pseudo_resolve_fill) {
	expr_t e, te;
	int align = 1;

	e = craisin_fetch_expr(l, 0);

	te = craisin_expr_copy(e);
	as->exportcheck = 1;
	craisin_reduce_expr(as, te);
	as->exportcheck = 0;

	if (craisin_expr_istype(te, CRAISIN_EXPR_TYPE_INT)) {
		align = craisin_expr_intval(te);
		if (align < 0) {
			craisin_expr_destroy(te);
			//set_error_fn(as, l, E_FILL_INVALID);
			return;
		}
	} else {
		craisin_expr_destroy(te);
		return;
	}
	craisin_expr_destroy(te);

	l->len = align;
}

EMITFUNC(pseudo_emit_fill) {
	expr_t e;
	int i;

/* don't emit anything in bss */
	if (l->csect && (l->csect->flags & (section_flag_bss | section_flag_constant)))
		return;

	e = craisin_fetch_expr(l, 1);
	for (i = 0; i < l->len; i++) {
		craisin_emitexpr(l, e, 1);
	}
}

//-----------------------------------------------------------------------------

/* string conditional argument parser */
/*
argument syntax:

a bare word ended by whitespace, comma, or NUL
a double quote delimited string containing arbitrary printable characters
a single quote delimited string containing arbitrary printable characters

In a double quoted string, a double quote cannot be represented.
In a single quoted string, a single quote cannot be represented.

*/
char *strcond_parsearg(char **p) {
	char *arg;
	char *tstr;
	int i;
	tstr = *p;

	if (!**p || isspace(**p)) {
		return strdup("");
	}

	if (*tstr == '"') {
	// double quote delim
		tstr++;
		for (i = 0; tstr[i] && tstr[i] != '"'; i++)
		/* do nothing */ ;

		arg = (char*)malloc(i + 1);
		strncpy(arg, tstr, i);
		arg[i] = 0;

		if (tstr[i])
			i++;

		*p += i;
		return arg;
	} else if (*tstr == '\'') {
	// single quote delim
		tstr++;
		for (i = 0; tstr[i] && tstr[i] != '\''; i++)
		/* do nothing */ ;

		arg = (char*)malloc(i + 1);
		strncpy(arg, tstr, i);
		arg[i] = 0;

		if (tstr[i])
			i++;

		*p += i;
		return arg;
	} else {
	// bare word - whitespace or comma delim
		for (i = 0; tstr[i] && !isspace(tstr[i]) && tstr[i] != ','; i++)
		/* do nothing */ ;

		arg = (char*)malloc(i + 1);
		strncpy(arg, tstr, i);
		arg[i] = 0;
		if (tstr[i] == ',')
			i++;

		*p += i;
		return arg;
	}
}

/* string conditional helpers */
/* return "1" for true, "0" for false */
int strcond_eq(char **p) {
	char *arg1;
	char *arg2;
	int c = 0;

	arg1 = strcond_parsearg(p);
	arg2 = strcond_parsearg(p);

	if (strcmp(arg1, arg2) == 0)
		c = 1;
	free(arg1);
	free(arg2);
	return c;
}

int strcond_ieq(char **p) {
	char *arg1;
	char *arg2;
	int c = 0;

	arg1 = strcond_parsearg(p);
	arg2 = strcond_parsearg(p);

	if (strcasecmp(arg1, arg2) == 0)
		c = 1;
	free(arg1);
	free(arg2);
	return c;
}

int strcond_ne(char **p) {
	char *arg1;
	char *arg2;
	int c = 0;

	arg1 = strcond_parsearg(p);
	arg2 = strcond_parsearg(p);

	if (strcmp(arg1, arg2) != 0)
		c = 1;
	free(arg1);
	free(arg2);
	return c;
}

int strcond_ine(char **p) {
	char *arg1;
	char *arg2;
	int c = 0;

	arg1 = strcond_parsearg(p);
	arg2 = strcond_parsearg(p);

	if (strcasecmp(arg1, arg2) != 0)
		c = 1;
	free(arg1);
	free(arg2);
	return c;
}

int strcond_peq(char **p) {
	char *arg0;
	char *arg1;
	char *arg2;
	size_t plen;
	int c = 0;

	arg0 = strcond_parsearg(p);
	arg1 = strcond_parsearg(p);
	arg2 = strcond_parsearg(p);

	plen = strtol(arg0, NULL, 10);
	if (strlen(arg1) > plen)
		arg1[plen] = 0;
	if (strlen(arg2) > plen)
		arg2[plen] = 0;

	if (strcmp(arg1, arg2) == 0)
		c = 1;
	free(arg0);
	free(arg1);
	free(arg2);
	return c;
}

int strcond_ipeq(char **p) {
	char *arg0;
	char *arg1;
	char *arg2;
	size_t plen;
	int c = 0;

	arg0 = strcond_parsearg(p);
	arg1 = strcond_parsearg(p);
	arg2 = strcond_parsearg(p);

	plen = strtol(arg0, NULL, 10);
	if (strlen(arg1) > plen)
		arg1[plen] = 0;
	if (strlen(arg2) > plen)
		arg2[plen] = 0;

	if (strcasecmp(arg1, arg2) == 0)
		c = 1;
	free(arg0);
	free(arg1);
	free(arg2);
	return c;
}

int strcond_pne(char **p) {
	char *arg0;
	char *arg1;
	char *arg2;
	size_t plen;
	int c = 0;

	arg0 = strcond_parsearg(p);
	arg1 = strcond_parsearg(p);
	arg2 = strcond_parsearg(p);

	plen = strtol(arg0, NULL, 10);
	if (strlen(arg1) > plen)
		arg1[plen] = 0;
	if (strlen(arg2) > plen)
		arg2[plen] = 0;

	if (strcmp(arg1, arg2) != 0)
		c = 1;
	free(arg0);
	free(arg1);
	free(arg2);
	return c;
}

int strcond_ipne(char **p) {
	char *arg0;
	char *arg1;
	char *arg2;
	size_t plen;
	int c = 0;

	arg0 = strcond_parsearg(p);
	arg1 = strcond_parsearg(p);
	arg2 = strcond_parsearg(p);

	plen = strtol(arg0, NULL, 10);
	if (strlen(arg1) > plen)
		arg1[plen] = 0;
	if (strlen(arg2) > plen)
		arg2[plen] = 0;

	if (strcasecmp(arg1, arg2) != 0)
		c = 1;
	free(arg0);
	free(arg1);
	free(arg2);
	return c;
}

int strcond_seq(char **p) {
	char *arg0;
	char *arg1;
	char *arg2;
	char *rarg1;
	char *rarg2;

	size_t plen;
	int c = 0;

	arg0 = strcond_parsearg(p);
	arg1 = strcond_parsearg(p);
	arg2 = strcond_parsearg(p);

	rarg1 = arg1;
	rarg2 = arg2;

	plen = strtol(arg0, NULL, 10);
	if (strlen(arg1) > plen) {
		rarg1 += strlen(arg1) - plen;
	}
	if (strlen(arg2) > plen) {
		rarg2 += strlen(arg2) - plen;
	}
	if (strcmp(rarg1, rarg2) == 0)
		c = 1;
	free(arg0);
	free(arg1);
	free(arg2);
	return c;
}

int strcond_iseq(char **p) {
	char *arg0;
	char *arg1;
	char *arg2;
	char *rarg1;
	char *rarg2;

	size_t plen;
	int c = 0;

	arg0 = strcond_parsearg(p);
	arg1 = strcond_parsearg(p);
	arg2 = strcond_parsearg(p);

	rarg1 = arg1;
	rarg2 = arg2;

	plen = strtol(arg0, NULL, 10);
	if (strlen(arg1) > plen) {
		rarg1 += strlen(arg1) - plen;
	}
	if (strlen(arg2) > plen) {
		rarg2 += strlen(arg2) - plen;
	}

	if (strcasecmp(rarg1, rarg2) == 0)
		c = 1;
	free(arg0);
	free(arg1);
	free(arg2);
	return c;
}


int strcond_sne(char **p) {
	char *arg0;
	char *arg1;
	char *arg2;
	char *rarg1;
	char *rarg2;

	size_t plen;
	int c = 0;

	arg0 = strcond_parsearg(p);
	arg1 = strcond_parsearg(p);
	arg2 = strcond_parsearg(p);

	rarg1 = arg1;
	rarg2 = arg2;

	plen = strtol(arg0, NULL, 10);
	if (strlen(arg1) > plen) {
		rarg1 += strlen(arg1) - plen;
	}
	if (strlen(arg2) > plen) {
		rarg2 += strlen(arg2) - plen;
	}

	if (strcmp(rarg1, rarg2) != 0)
		c = 1;
	free(arg0);
	free(arg1);
	free(arg2);
	return c;
}

int strcond_isne(char **p) {
	char *arg0;
	char *arg1;
	char *arg2;
	char *rarg1;
	char *rarg2;

	size_t plen;
	int c = 0;

	arg0 = strcond_parsearg(p);
	arg1 = strcond_parsearg(p);
	arg2 = strcond_parsearg(p);

	rarg1 = arg1;
	rarg2 = arg2;

	plen = strtol(arg0, NULL, 10);
	if (strlen(arg1) > plen) {
		rarg1 += strlen(arg1) - plen;
	}
	if (strlen(arg2) > plen) {
		rarg2 += strlen(arg2) - plen;
	}

	if (strcasecmp(rarg1, rarg2) != 0)
		c = 1;
	free(arg0);
	free(arg1);
	free(arg2);
	return c;
}

/* string conditionals */
PARSEFUNC(pseudo_parse_ifstr) {
	static struct strconds {
		std::string str;
		int (*fn)(char **ptr);
	} strops[] = {
		{"eq", strcond_eq},
		{"ieq", strcond_ieq},
		{"ne", strcond_ne},
		{"ine", strcond_ine},
		{"peq", strcond_peq},
		{"ipeq", strcond_ipeq},
		{"pne", strcond_pne},
		{"ipne", strcond_ipne},
		{"seq", strcond_seq},
		{"iseq", strcond_iseq},
		{"sne", strcond_sne},
		{"isne", strcond_isne},
		{nullptr, 0}
	};
	int tv = 0;
	char *tstr;
	int strop;

	l->len = 0;

	if (as->skipcond && !(as->skipmacro)) {
		as->skipcount++;
		skip_operand(p);
		return;
	}

	tstr = strcond_parsearg(p);
	if (!**p || isspace(**p)) {
		//set_error_fn(as, l, E_STRING_BAD);
		return;
	}

	for (strop = 0; strops[strop].str != (std::string)NULL; strop++)
		if (strcasecmp(strops[strop].str.c_str(), tstr) == 0)
			break;

	free(tstr);

	if (strops[strop].str == (std::string)NULL) {
		//set_error_fn(as, l, E_STRING_BAD);
	}

	tv = (*(strops[strop].fn)) (p);

	if (!tv) {
		as->skipcond = 1;
		as->skipcount = 1;
	}
}

static int cstringlen(craisin_state_t * as, line_t * ln, char **p, char delim) {
	int l = 0;
	char *str = NULL;
	int blen = 0;
	int bsize = 0;
	
	if (!(as->pragmas & PRAGMA_CESCAPES)) {
		for (; **p && **p != delim; (*p)++) {
			l++;
			if (blen >= bsize) {
				str = (char*)realloc(str, bsize + 32);
				bsize += 32;
			}
			str[blen++] = **p;
		}
	} else {
		while (**p && **p != delim) {
			int wch = **p;
			if (**p == '\\') {
				/* escape sequence */
				
				(*p)++;
				if (!**p)
					break;
				
				switch (**p) {
					/* octal sequence or NUL */
					/* skip the "0", then skip up to two more digits */
					case '0':
					case '1':
					case '2':
					case '3':
						wch = **p;
						wch -= 0x30;
						if ((*p)[1] >= '0' && (*p)[1] < '8') {
							(*p)++;
							wch *= 8;
							wch += **p - 0x30;
						}
						if ((*p)[1] >= '0' && (*p)[1] < '8') {
							(*p)++;
							wch *= 8;
							wch += **p - 0x30;
						}
						break;
						
						/* hexadecimal value */
						case 'x':
							(*p)++;	// ignore "x"
							wch = 0;
							if (**p)	// skip digit 1
							{
								wch = **p - 0x30;
								if (wch > 9)
									wch -= 7;
								if (wch > 9)
									wch -= 32;
								(*p)++;
							}
							if (**p) {
								int i;
								i = **p - 0x30;
								if (i > 9)
									i -= 7;
								if (i > 9)
									i -= 32;
								wch = wch * 16 + i;
							}
							break;
							
						case 'a':
							wch = 7;
							break;
							
						case 'b':
							wch = 8;
							break;
							
						case 't':
							wch = 9;
							break;
							
						case 'n':
							wch = 10;
							break;
							
						case 'v':
							wch = 11;
							break;
							
						case 'f':
							wch = 12;
							break;
							
						case 'r':
							wch = 13;
							
							/* everything else represents itself */
							default:
								break;
				}
			}
			/* now "wch" is the character to write out */
			l++;
			(*p)++;
			if (blen >= bsize) {
				str = (char*)realloc(str, bsize + 32);
				bsize += 32;
			}
			str[blen++] = wch;
		}
	}
	/* do something with the string here */
	/* l is the string length, str is the string itself */
	ln->lstr = str;
	return l;
}

//-----------------------------------------------------------------------------
