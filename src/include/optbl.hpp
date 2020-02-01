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

#ifndef HEADER_OPTBL
#define HEADER_OPTBL

#include <craisin.hpp>
#include <cpu.hpp>

typedef struct {
	char *opcode;		/* the mneumonic */
	int ops[4];		/* opcode values for up to four addr modes */
	void (*parse) (asmstate_t * as, line_t * l, char **optr);	/* parse operand for insn */
	void (*resolve) (asmstate_t * as, line_t * l, int force);	/* resolve instruction to code */
	void (*emit) (asmstate_t * as, line_t * l);	/* resolve instruction to code */
	int flags;		/* flag for this instruction */
} instab_t;

enum {
	lwasm_insn_cond = 1,	/* conditional instruction */
	lwasm_insn_endm = 1 << 1,	/* end of macro */
	lwasm_insn_setsym = 1 << 2,	/* insn sets symbol address */
	lwasm_insn_is6309 = 1 << 3,	/* insn is 6309 only */
	lwasm_insn_struct = 1 << 4,	/* insn allowed in a struct */
	lwasm_insn_setdata = 1 << 5,	/* insn uses the data address for symbols */
	lwasm_insn_is6800 = 1 << 6,	/* insn is a 6800 compatibility operation */
	lwasm_insn_is6809 = 1 << 7,	/* insn is 6809 only */
	lwasm_insn_is6809conv = 1 << 8,	/* insn is 6809 convenience only */
	lwasm_insn_is6309conv = 1 << 9,	/* insn is 6309 convenience only */
	lwasm_insn_isemuext = 1 << 10,	/* insn is an emulator extension */
	lwasm_insn_used = 1 << 11,	/* insn has been used at least once during assembly */
	lwasm_insn_normal = 0
};

#define PARSEFUNC(fn)	void (fn)(asmstate_t *as, line_t *l, char **p)
#define RESOLVEFUNC(fn)	void (fn)(asmstate_t *as, line_t *l, int force)
#define EMITFUNC(fn)	void (fn)(asmstate_t *as, line_t *l)

extern instab_t instab[];

#endif
