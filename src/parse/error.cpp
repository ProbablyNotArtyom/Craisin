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
#include <string.h>
#include <cstdlib>

#include <craisin.hpp>
#include <error.hpp>

//-----------------------------------------------------------------------------

using namespace std;
static void (*error_handler)(const char *fmt, ...) = nullptr;
void craisin_emit_error(const string error) {
	if (error_handler) (*error_handler)(error.c_str());
	else {
		cerr << error.c_str();
	}
	exit(1);
}

void set_error_fn(craisin_error_fn func) {
	error_handler = func;
}

//-----------------------------------------------------------------------------

const char* craisin_lookup_error(errorcode_t error_code) {
	switch (error_code) {
		case E_6309_INVALID:			return "Illegal use of 6309 instruction in 6809 mode";
		case E_6809_INVALID:			return "Illegal use of 6809 instruction in 6309 mode";
		case E_ALIGNMENT_INVALID:		return "Invalid alignment";
		case E_BITNUMBER_INVALID:		return "Invalid bit number";
		case E_BITNUMBER_UNRESOLVED:	return "Bit number must be fully resolved";
		case E_BYTE_OVERFLOW:			return "Byte overflow";
		case E_CONDITION_P1:			return "Conditions must be constant on pass 1";
		case E_DIRECTIVE_OS9_ONLY:		return "Directive only valid for OS9 target";
		case E_DIV0:					return "Division by zero";
		case E_EXEC_ADDRESS:			return "Exec address not constant!";
		case E_EXPRESSION_BAD:			return "Bad expression";
		case E_EXPRESSION_NOT_CONST:	return "Expression must be constant";
		case E_EXPRESSION_NOT_RESOLVED: return "Expression not fully resolved";
		case E_FILE_OPEN:				return "Cannot open file";
		case E_FILENAME_MISSING:		return "Missing filename";
		case E_FILL_INVALID:			return "Invalid fill length";
		case E_IMMEDIATE_INVALID:		return "Immediate mode not allowed";
		case E_IMMEDIATE_UNRESOLVED:	return "Immediate byte must be fully resolved";
		case E_INSTRUCTION_FAILED:		return "Instruction failed to resolve.";
		case E_INSTRUCTION_SECTION:		return "Instruction generating output outside of a section";
		case E_LINE_ADDRESS:			return "Cannot resolve line address";
		case E_LINED_ADDRESS:			return "Cannot resolve line data address";
		case E_OBJTARGET_ONLY:			return "Only supported for object target";
		case E_OPCODE_BAD:				return "Bad opcode";
		case E_OPERAND_BAD:				return "Bad operand";
		case E_PADDING_BAD:				return "Bad padding";
		case E_PRAGMA_UNRECOGNIZED:		return "Unrecognized pragma string";
		case E_REGISTER_BAD:			return "Bad register";
		case E_SETDP_INVALID:			return "SETDP not permitted for object target";
		case E_SETDP_NOT_CONST:			return "SETDP must be constant on pass 1";
		case E_STRING_BAD:				return "Bad string condition";
		case E_SYMBOL_BAD:				return "Bad symbol";
		case E_SYMBOL_MISSING:			return "Missing symbol";
		case E_SYMBOL_UNDEFINED:		return "Undefined symbol";
		case E_SYMBOL_UNDEFINED_EXPORT: return "Undefined exported symbol";
		case E_MACRO_DUPE:				return "Duplicate macro definition";
		case E_MACRO_ENDM:				return "ENDM without MACRO";
		case E_MACRO_NONAME:			return "Missing macro name";
		case E_MACRO_RECURSE:			return "Attempt to define a macro inside a macro";
		case E_MODULE_IN:				return "Already in a module!";
		case E_MODULE_NOTIN:			return "Not in a module!";
		case E_NEGATIVE_BLOCKSIZE:		return "Negative block sizes make no sense!";
		case E_NEGATIVE_RESERVATION:	return "Negative reservation sizes make no sense!";
		case E_NW_8:					return "n,W cannot be 8 bit";
		case E_SECTION_END:				return "ENDSECTION without SECTION";
		case E_SECTION_EXTDEP:			return "EXTDEP must be within a section";
		case E_SECTION_FLAG:			return "Unrecognized section flag";
		case E_SECTION_NAME:			return "Need section name";
		case E_SECTION_TARGET:			return "Cannot use sections unless using the object target";
		case E_STRUCT_DUPE:				return "Duplicate structure definition";
		case E_STRUCT_NONAME:			return "Cannot declare a structure without a symbol name.";
		case E_STRUCT_NOSYMBOL:			return "Structure definition with no effect - no symbol";
		case E_STRUCT_RECURSE:			return "Attempt to define a structure inside a structure";
		case E_SYMBOL_DUPE:				return "Multiply defined symbol";
		case E_UNKNOWN_OPERATION:		return "Unknown operation";
		case E_ORG_NOT_FOUND:			return "Previous ORG not found";
		case E_USER_SPECIFIED:			return "User Specified:";
		
		case W_ENDSTRUCT_WITHOUT:		return "ENDSTRUCT without STRUCT";
		case W_DUPLICATE_SECTION:		return "Section flags can only be specified the first time; ignoring duplicate definition";
		case W_NOT_SUPPORTED:			return "Not supported";
		
		default:						return "Error";
	}
}

/* keeping this as a separate error output for stability in unit test scripts */
void craisin_error_testmode(line_t *cl, const char* msg, int fatal)
{
	cl -> as -> testmode_errorcount++;
	fprintf(stderr, "line %d: %s : %s\n", cl->lineno, msg, cl->ltext);
	if (fatal == 1) craisin_emit_error("aborting\n");
}

/* parse unit test input data from comment field */
void craisin_parse_testmode_comment(line_t *l, craisin_testflags_t *flags, errorcode_t *err, int *len, char **buf)
{
	*flags = TF_NONE;
	
	if (!l)
		return;
	
	char* s = strstr(l -> ltext, ";.");
	if (s == NULL) return;
	
	char* t = strstr(s, ":");
	if (t == NULL)
	{
		/* parse: ;.8E0FCE (emitted code) */
		
		if (buf == NULL) return;
		
		int i;
		*flags = TF_EMIT;
		
		s = s + 2;	/* skip ;. prefix */
		t = s;
		while (*t > 32) t++;
		
		if ((t - s) & 1)
		{
			craisin_error_testmode(l, "bad test data (wrong length of hex chars)", 1);
			return;
		}
		
		*len = (t - s) / 2;
		
		t = (char*)malloc(*len);
		*buf = t;
		
		for (i = 0; i < *len; i++)
		{
			int val;
			sscanf(s, "%2x", &val);
			*t++ = (char) val;
			s += 2;
		}
	}
	else
	{
		/* parse: ;.E:1000 or ;.E:7 (warnings or errors) */
		*flags = TF_ERROR;
		
		char ch = toupper(*(t - 1));
		if (ch != 'E') craisin_error_testmode(l, "bad test data (expected E: flag)", 1);
		sscanf(t + 1, "%d", (int*) err);
	}
}

void craisin_register_error_real(craisin_state_t * as, line_t * l, errorcode_t error_code, const char *msg) {

}

void craisin_register_error(craisin_state_t * as, line_t * l, errorcode_t err) {
	craisin_register_error_real(as, l, err, craisin_lookup_error(err));
}

void craisin_register_error2(craisin_state_t * as, line_t * l, errorcode_t err, const char *fmt, ...) {
	char errbuff[1024];
	char f[128];

	sprintf(f, "%s %s", craisin_lookup_error(err), fmt);

	va_list args;

	va_start(args, fmt);

	(void) vsnprintf(errbuff, 1024, f, args);

	craisin_register_error_real(as, l, err, errbuff);

	va_end(args);
}
