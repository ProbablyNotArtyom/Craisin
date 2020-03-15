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
	#include <error.hpp>
	
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

struct craisin_error {
	errorcode_t		code;
	std::string 	message;
	int 			index;
};

typedef std::list<struct craisin_error> errorlist_t;
typedef struct craisin_error craisin_error_t;

enum output_format {
	OUTPUT_FMT_STD = 0,			// Output to STDOUT
	OUTPUT_FMT_BIN,				// Generic binary 
	OUTPUT_FMT_IHEX,
	OUTPUT_FMT_SREC				// Motorola SRecord
};

//-----------------------------------------------------------------------------

enum {
	PRAGMA_NONE = 0,
	PRAGMA_CPU_6502				= 1<<0,
	PRAGMA_CPU_6502_ILLEGAL		= 1<<1,
	PRAGMA_CPU_65C02			= 1<<2,
	PRAGMA_CPU_65C02_WDC		= 1<<3,
	PRAGMA_CPU_65816			= 1<<4,
	PRAGMA_EXPORT_ALL			= 1<<5,		// Export all symbols
	PRAGMA_IMPORT_UNDEF			= 1<<6,		// Import symbols that are undefined when exporting
	
	PRAGMA_CESCAPES				= 1<<7,		// Interpret C style escape sequences in strings
	PRAGMA_TESTMODE				= 1<<8,		// Enable testmode
	PRAGMA_NEWSOURCE			= 1<<9,		
	PRAGMA_CLEARBIT				= 1<<10,
	
	PRAGMA_NEGATED				= 1<<31		// Pragma flag test bit
} pragma_t;

typedef struct craisin_line line_t;
typedef struct craisin_state craisin_state_t;

typedef enum {
	section_flag_bss = 1,				// BSS section
	section_flag_constant = 2,			// constants - no base offset
	section_flag_none = 0				// no flags
} secflag_t;

typedef enum {
	TF_NONE = 0,
	TF_EMIT = 1,
	TF_ERROR = 2
} craisin_testflags_t;

typedef std::list<struct craisin_section> sectionlist_t;
typedef struct craisin_section craisin_section_t;
typedef std::list<struct craisin_reloc> reloclist_t;
typedef struct craisin_reloc craisin_reloc_t;
struct craisin_reloc {
	expr_t 	offset;						// offset of relocation
	int 	size;						// size of relocation
	expr_t 	expr;						// relocation expression
};

struct craisin_section {
	std::string			name;			// the name of the section
	secflag_t 			flags;			// flags for the symbol
	expr_t 				offset;			// offset for next instance
	int 				oblen;			// size of section output
	int 				obsize;			// size of output buffer
	unsigned char 		*obytes;		// output buffer
	reloclist_t 		*reloctab;		// table of relocations
};

typedef enum {
	symbol_flag_set = 1,				// symbol was used with "set"
	symbol_flag_nocheck = 2,			// do not check symbol characters
	symbol_flag_nolist = 4,				// no not show symbol in symbol table
	symbol_flag_nocase = 8,				// do not match case of symbol
	symbol_flag_none = 0				// no flags
} symflag_t;

typedef std::list<struct craisin_symbol> symbollist_t;
typedef struct craisin_symbol craisin_symbol_t;
struct craisin_symbol {
	std::string			name;			// the name of the symbol
	int 				context;		// symbol context (-1 for global)
	int 				version;		// version of the symbol (for "set")
	symflag_t 			flags;			// flags for the symbol
	craisin_section_t 	*section;		// section the symbol is defined in
	expr_t				value;			// symbol value
	craisin_symbol_t 	*left;			// left subtree pointer
	craisin_symbol_t 	*right;			// right subtree pointer
	craisin_symbol_t 	*nextver;		// next lower version
};

struct sym_export {
	std::string	name;					// symbol to export
	struct craisin_symbol *se;					// symbol table entry
	line_t *line;						// line the export is on
};

typedef std::list<struct sym_export> exportlist_t;
typedef std::list<std::string> stringlist_t;

typedef std::list<struct craisin_struct> structlist_t;
typedef struct craisin_struct_field struct_field_t;
typedef struct craisin_struct craisin_struct_t;

struct craisin_struct_field {
	std::string			name;			// structure field name - NULL for anonymous
	int 				size;			// structure field size
	craisin_struct_t  	*substruct;		// sub structure if there is one
};

struct craisin_struct {
	std::string 	name;				// name of structure
	int 			size;				// number of bytes taken by struct
	std::list<struct_field_t> fields;	// fields in the structure
	line_t 			*origin;			// line where structure is defined
};

typedef std::list<struct craisin_macro> macrolist_t;
typedef struct craisin_macro craisin_macro_t;
struct craisin_macro {
	std::string				name;			// name of macro
	std::list<std::string>	lines;			// macro lines
	int 					numlines;		// number lines in macro
	int 					flags;			// flags for the macro
	line_t 					*definedat;		// the line where the macro definition starts
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
	char *sym;							// symbol, if any, on the line
	unsigned char *output;				// output bytes
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
	errorcode_t err_testmode;			// error code in testmode
	line_t *prev;						// previous line
	line_t *next;						// next line
	int inmod;							// inside a module?
	craisin_section_t *csect;			// which section are we in?
	struct line_expr_s *exprs;			// expressions used during parsing
	char *lstr;							// string passed forward
	int pb;								// pass forward post byte
	int lint;							// pass forward integer
	int lint2;							// another pass forward integer
	int conditional_return;				// for ?RTS handling (1 if RTS follows)
	craisin_state_t *as;				// assembler state data ptr
	int pragmas;						// pragmas in effect for the line
	int context;						// the symbol context number
	char *ltext;						// line number
	char *linespec;						// line spec
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
	char *list_file;					// name of file to list to
	char *audit_file;					// name of file to output used features to
	int tabwidth;						// tab width in list file
	char *map_file;						// name of map file
	char *output_file;					// output file name	
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

#define PARSEFUNC(fn)	void (fn)(craisin_state_t *as, line_t *l, char **p)
#define RESOLVEFUNC(fn)	void (fn)(craisin_state_t *as, line_t *l, int force)
#define EMITFUNC(fn)	void (fn)(craisin_state_t *as, line_t *l)

#define CURPRAGMA(l,p)	(((l) && ((l)->pragmas & (p))) ? 1 : 0)
/* this macro can only be used where "l" is the current line pointer */
void skip_operand_real(line_t *cl, char **p);
#define skip_operand(p) skip_operand_real(l, p)

//-----------------------------------------------------------------------------

craisin_symbol_t *register_symbol(craisin_state_t *as, line_t *cl, char *sym, expr_t value, int flags);
craisin_symbol_t *lookup_symbol(craisin_state_t *as, line_t *cl, char *sym);
void register_struct_entry(craisin_state_t *as, line_t *l, int size, craisin_struct_t *ss);

int parse_pragma_helper(char *p);

void input_init(craisin_state_t *as);
void input_openstring(craisin_state_t *as, char *s, char *str);
void input_open(craisin_state_t *as, char *s);
char *input_readline(craisin_state_t *as);
char *input_curspec(craisin_state_t *as);
FILE *input_open_standalone(craisin_state_t *as, char *s, char **rfn);
int input_isinclude(craisin_state_t *as);

int craisin_next_context(craisin_state_t *as);
void craisin_emit(line_t *cl, int byte);
void craisin_emitop(line_t *cl, int opc);

void craisin_save_expr(line_t *cl, int id, expr_t expr);
expr_t craisin_fetch_expr(line_t *cl, int id);
expr_t craisin_parse_expr(craisin_state_t *as, char **p);
int craisin_emitexpr(line_t *cl, expr_t expr, int s);
int craisin_reduce_expr(craisin_state_t *as, expr_t expr);
expr_t craisin_parse_cond(craisin_state_t *as, char **p);


//-----------------------------------------------------------------------------

#endif





