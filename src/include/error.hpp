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

#ifndef HEADER_ERROR
#define HEADER_ERROR

#include <cstdarg>

//-----------------------------------------------------------------------------

typedef void (*craisin_error_fn)(const char *fmt, ...);

typedef enum {
	E_6309_INVALID				= 1,
	E_6809_INVALID				= 2,
	E_ALIGNMENT_INVALID			= 3,
	E_BITNUMBER_UNRESOLVED		= 4,
	E_BITNUMBER_INVALID			= 5,
	E_BYTE_OVERFLOW				= 6,
	E_CONDITION_P1				= 7,
	E_DIRECTIVE_OS9_ONLY		= 8,
	E_DIV0						= 9,
	E_EXEC_ADDRESS				= 10,
	E_FILL_INVALID				= 11,
	E_IMMEDIATE_INVALID			= 12,
	E_IMMEDIATE_UNRESOLVED		= 13,
	E_EXPRESSION_BAD			= 14,
	E_EXPRESSION_NOT_CONST		= 15,
	E_EXPRESSION_NOT_RESOLVED	= 16,
	E_FILE_OPEN					= 17,
	E_FILENAME_MISSING			= 18,
	E_INSTRUCTION_FAILED		= 19,
	E_INSTRUCTION_SECTION		= 20,
	E_LINE_ADDRESS				= 21,
	E_LINED_ADDRESS				= 22,
	E_MACRO_DUPE				= 23,
	E_MACRO_ENDM				= 24,
	E_MACRO_NONAME				= 25,
	E_MACRO_RECURSE				= 26,
	E_MODULE_IN					= 27,
	E_MODULE_NOTIN				= 28,
	E_NEGATIVE_BLOCKSIZE		= 29,
	E_NEGATIVE_RESERVATION		= 30,
	E_NW_8						= 31,
	E_OPCODE_BAD				= 32,
	E_OPERAND_BAD				= 33,
	E_OBJTARGET_ONLY			= 34,
	E_PADDING_BAD				= 35,
	E_PRAGMA_UNRECOGNIZED		= 36,
	E_REGISTER_BAD				= 37,
	E_SECTION_END				= 38,
	E_SECTION_EXTDEP			= 39,
	E_SECTION_FLAG				= 40,
	E_SECTION_NAME				= 41,
	E_SECTION_TARGET			= 42,
	E_SETDP_INVALID				= 43,
	E_SETDP_NOT_CONST			= 44,
	E_STRING_BAD				= 45,
	E_STRUCT_DUPE				= 46,
	E_STRUCT_NONAME				= 47,
	E_STRUCT_NOSYMBOL			= 48,
	E_STRUCT_RECURSE			= 49,
	E_SYMBOL_BAD				= 50,
	E_SYMBOL_DUPE				= 51,
	E_SYMBOL_MISSING			= 52,
	E_SYMBOL_UNDEFINED			= 53,
	E_SYMBOL_UNDEFINED_EXPORT	= 54,
	E_UNKNOWN_OPERATION			= 55,
	E_USER_SPECIFIED			= 56,
	E_ORG_NOT_FOUND				= 57,
	
	/* warnings must be 1000 or greater */
	
	W_DUPLICATE_SECTION			= 1000,
	W_ENDSTRUCT_WITHOUT			= 1001,
	W_NOT_SUPPORTED				= 1002,
	W_USER_SPECIFIED			= 1003
} errorcode_t;

//-----------------------------------------------------------------------------

void craisin_error(const char *fmt, ...);
void set_error_fn(void (*f)(const char *fmt, ...));

//-----------------------------------------------------------------------------

#endif

