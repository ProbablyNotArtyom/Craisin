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

#ifndef HEADER_CRAISIN
#define HEADER_CRAISIN

	#include <stack>
	#include <list>
	#include <expr.hpp>
	
	using namespace std;
	
//-----------------------------------------------------------------------------

/* Characters that can start a valid symbol */
#define CHAR_SYMHEAD 				"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_@$"
/* Characters that can trail the start of a valid symbol */
#define CHAR_SYMTAIL CHAR_SYMHEAD 	".?0123456789"

/* The maximum depth that scopes can be nested */
#define MAX_SCOPE_DEPTH		16

/* The maximum number of conditional statements that can be nested */
#define MAX_COND_DEPTH		64

//-----------------------------------------------------------------------------

typedef enum {
	STATUS_OK,							// Normal idle status

	STATUS_ERRORS,						// Marks where statuses get treated like errors
	ERROR_INTERNAL = STATUS_ERRORS,		// Unknown internal error

	NUM_STATUSCODES
} error_code_t;

typedef list<struct craisin_error> errorlist_t;
typedef struct craisin_error craisin_error_t;
struct craisin_error {
	error_code_t	code;
	string 			message;
	int 			index;
};

//-----------------------------------------------------------------------------

enum output_format {
	OUTPUT_FMT_STD = 0,			// Output to STDOUT
	OUTPUT_FMT_BIN,				// Generic binary 
	OUTPUT_FMT_IHEX,
	OUTPUT_FMT_SREC				// Motorola SRecord
};

enum {
	PRAGMA_NONE = 0,
	PRAGMA_CPU_6502				= 1<<0,
	PRAGMA_CPU_6502_ILLEGAL		= 1<<1,
	PRAGMA_CPU_65C02			= 1<<2,
	PRAGMA_CPU_65C02_WDC		= 1<<3,
	PRAGMA_CPU_65816			= 1<<4,
	PRAGMA_EXPORT_ALL			= 1<<5,		// Export all symbols
	PRAGMA_IMPORT_UNDEF			= 1<<6,		// Import symbols that are undefined when exporting
	
	PRAGMA_NEGATED				= 1<<31		// Pragma flag test bit
} pragma_t;

typedef enum {
	section_flag_bss = 1,				// BSS section
	section_flag_constant = 2,			// constants - no base offset
	section_flag_none = 0				// no flags
} secflag_t;

typedef enum {
	symbol_flag_set = 1,				// symbol was used with "set"
	symbol_flag_nocheck = 2,			// do not check symbol characters
	symbol_flag_nolist = 4,				// no not show symbol in symbol table
	symbol_flag_nocase = 8,				// do not match case of symbol
	symbol_flag_none = 0				// no flags
} symflag_t;

typedef list<struct craisin_reloc> reloclist_t;
typedef struct craisin_reloc craisin_reloc_t;
struct craisin_reloc {
	expr_t 	offset;						// offset of relocation
	int 	size;						// size of relocation
	expr_t 	expr;						// relocation expression
};

typedef list<struct craisin_section> sectionlist_t;
typedef struct craisin_section craisin_section_t;
struct craisin_section {
	string			name;				// the name of the section
	secflag_t 		flags;				// flags for the symbol
	expr_t 			offset;				// offset for next instance
	int 			oblen;				// size of section output
	int 			obsize;				// size of output buffer
	unsigned char 	*obytes;			// output buffer
	reloclist_t 	*reloctab;			// table of relocations
};

typedef list<struct craisin_symbol> symbollist_t;
typedef struct craisin_symbol craisin_symbol_t;
struct craisin_symbol {
	string				name;			// the name of the symbol
	int 				context;		// symbol context (-1 for global)
	int 				version;		// version of the symbol (for "set")
	symflag_t 			flags;			// flags for the symbol
	craisin_section_t 	*section;		// section the symbol is defined in
	expr_t				value;			// symbol value
	craisin_symbol_t 	*left;			// left subtree pointer
	craisin_symbol_t 	*right;			// right subtree pointer
	craisin_symbol_t 	*nextver;		// next lower version
};

typedef struct craisin_line line_t;
typedef struct craisin_state craisin_state_t;
typedef list<struct sym_export> exportlist_t;
typedef list<string> stringlist_t;
struct sym_export {
	string				name;			// symbol to export
	craisin_symbol_t	*se;			// symbol table entry
	line_t 				*line;			// line the export is on
};

typedef list<struct craisin_struct> structlist_t;
typedef struct craisin_struct craisin_struct_t;
struct craisin_struct_field {
	string				name;			// structure field name - NULL for anonymous
	int 				size;			// structure field size
	craisin_struct_t  	*substruct;		// sub structure if there is one
};

typedef struct craisin_struct_field struct_field_t;
struct craisin_struct {
	string 					name;		// name of structure
	int 					size;		// number of bytes taken by struct
	list<struct_field_t> 	fields;		// fields in the structure
	line_t 					*origin;	// line where structure is defined
};

typedef list<struct craisin_macro> macrolist_t;
typedef struct craisin_macro craisin_macro_t;
struct craisin_macro {
	string			name;			// name of macro
	list<string>	lines;			// macro lines
	int 			numlines;		// number lines in macro
	int 			flags;			// flags for the macro
	line_t 			*definedat;		// the line where the macro definition starts
};

struct craisin_line {
	expr_t addr;						// assembly address of the line
	expr_t daddr;						// data address of the line (os9 only)
	int len;							// the "size" this line occupies (address space wise) (-1 if unknown)
	int dlen;							// the data "size" this line occupies (-1 if unknown)
	int minlen;							// minimum length
	int maxlen;							// maximum length
	int insn;							// number of insn in insn table
	int symset;							// set if the line symbol was consumed by the instruction
	string sym;							// symbol, if any, on the line
	string output;						// output bytes
	int outputl;						// size of output
	int outputbl;						// size of output buffer
	int dpval;							// direct page value
	int cycle_base;						// base instruction cycle count
	int cycle_adj;						// cycle adjustment
	int	cycle_flags;					// cycle flags
	int genmode;						// generation mode (insn_parse_gen0/8/16)
	int fcc_extras;						// fcc extra bytes
	errorlist_t *err;					// list of errors
	errorlist_t *warn;					// list of errors
	error_code_t err_testmode;			// error code in testmode
	line_t *prev;						// previous line
	line_t *next;						// next line
	int inmod;							// inside a module?
	craisin_section_t *csect;			// which section are we in?
	struct line_expr_s *exprs;			// expressions used during parsing
	string lstr;						// string passed forward
	int pb;								// pass forward post byte
	int lint;							// pass forward integer
	int lint2;							// another pass forward integer
	int conditional_return;				// for ?RTS handling (1 if RTS follows)
	craisin_state_t *as;				// assembler state data ptr
	int pragmas;						// pragmas in effect for the line
	int context;						// the symbol context number
	string ltext;						// line number
	string linespec;					// line spec
	int lineno;							// line number
	int soff;							// struct offset (for listings)
	int dshow;							// data value to show (for listings)
	int dsize;							// set to 1 for 8 bit dshow value
	int isbrpt;							// set to 1 if this line is a branch point
	craisin_symbol_t *dptr;				// symbol value to display
	
	int noexpand_start;					// start of a no-expand block
	int noexpand_end;					// end of a no-expand block
	int hideline;						// set if we're going to hide this line on output	
};

struct craisin_state {
	int output_format;					// output format
	int debug_level;					// level of debugging requested
	FILE *debug_file;					// FILE * to output debug messages to
	int flags;							// assembly flags
	int pragmas;						// pragmas currently in effect
	int errorcount;						// number of errors encountered
	int warningcount;					// number of warnings issued
	int testmode_errorcount;			// number of errors in testmode
	int inmacro;						// are we in a macro?
	int instruct;						// are w in a structure?
	int skipcond;						// skipping a condition?
	int skipcount;						// depth of "skipping"
	int skipmacro;						// are we skipping in a macro?	
	int endseen;						// have we seen an "end" pseudo?
	int execaddr;						// address from "end"
	int inmod;							// inside an os9 module?
	int undefzero;						// used for handling "condundefzero"
	int pretendmax;						// set if we need to pretend the instruction is max length
	unsigned char crc[3];				// crc accumulator
	int cycle_total;					// cycle count accumulator
	int badsymerr;						// throw error on undef sym if set
	
	line_t *line_head;					// start of lines list
	line_t *line_tail;					// tail of lines list
	
	line_t *cl;							// current line pointer
	
	craisin_section_t *csect;			// current section
	
	int context;						// the current "context"
	int nextcontext;					// the next available context
	
	symbollist_t symtab;				// meta data for the symbol table
	macrolist_t *macros;				// macro table
	sectionlist_t *sections;			// section table
	exportlist_t *exportlist;			// list of exported symbols
	stringlist_t importlist;			// list of imported symbols
	string list_file;					// name of file to list to
	string audit_file;					// name of file to output used features to
	int tabwidth;						// tab width in list file
	string map_file;					// name of map file
	string output_file;					// output file name	
	stringlist_t input_files;			// files to assemble
	void *input_data;					// opaque data used by the input system
	stringlist_t include_list;			// include paths
	stringlist_t file_dir;				// stack of the "current file" dir
	stringlist_t includelist;
	
	structlist_t structs;				// defined structures
	craisin_struct_t *cstruct;			// current structure
	expr_t savedaddr;					// old address counter before struct started	
	int exportcheck;					// set if we need to collapse out the section base to 0
	int passno;							// set to the current pass number
	int preprocess;						// set if we are prepocessing
	int fileerr;						// flags error opening file
	int exprwidth;						// the bit width of the expression being evaluated
};

typedef void (*const pass_fn_t)(craisin_state_t *as);

//-----------------------------------------------------------------------------

#endif






