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
	#include <sstream>
	#include <fstream>
	#include <unistd.h>
	#include <string>
	#include <memory>
	#include <filesystem>
	#include <list>
	#include <cstdarg>

	#include <argparse.hpp>
	#include <cpu.hpp>
	#include <craisin.hpp>
	#include <utils.hpp>
	#include <error.hpp>

	#include <expr.hpp>
	#include <token.hpp>

//-----------------------------------------------------------------------------

void craisin_pass_1(craisin_state_t *as);
void craisin_pass_2(craisin_state_t *as);
void craisin_pass_3(craisin_state_t *as);
void craisin_pass_4(craisin_state_t *as);
void craisin_pass_5(craisin_state_t *as);
void craisin_pass_6(craisin_state_t *as);
void craisin_pass_7(craisin_state_t *as);

struct assembler_pass {
	const std::string	name;
	const pass_fn_t		fn;
	const int 			fordep;
} passlist[] = {
	{ std::string("parse"), craisin_pass_1, 1 },
	{ std::string("symcheck"), craisin_pass_2, 0 },
	{ std::string("resolve1"), craisin_pass_3, 0 },
	{ std::string("resolve2"), craisin_pass_4, 0 },
	{ std::string("addressresolve"), craisin_pass_5, 0 },
	{ std::string("finalize"), craisin_pass_6, 0 },
	{ std::string("emit"), craisin_pass_7, 0 },
	{ std::string(), nullptr, 0 }
};

bool flag_debug = false;
bool flag_verbose = false;
cpu_model_t craisin_cpu_model;

//-----------------------------------------------------------------------------

int main(int argc, const char *argv[]) {
	// Parse the command line options and handle flags
	using namespace argparse;
	ArgumentParser parser("Craisin");
	parser.add_description("\n6502/65816 assembler supporting variable syntax");
	parser.add_argument("-h", "--help")
		.help("shows this help text")
		.default_value(false)
		.implicit_value(true);
	parser.add_argument("-d", "--debug")
		.help("enables debug mode")
		.default_value(false)
		.implicit_value(true);
	parser.add_argument("-v", "--verbose")
		.help("enables verbose logging")
		.default_value(false)
		.implicit_value(true);
	parser.add_argument("-c", "--cpu")
		.help("uses CPU as the target processor");
	parser.add_argument("--vice")
		.help("outputs a VICE symbol file to FILE");
	parser.add_argument("files")
		.help("input files")
		.remaining();

	try { parser.parse_args(argc, argv);}
	catch (const std::runtime_error& err) {
    	std::cout << err.what() << std::endl;
    	std::cout << parser;
    	exit(0);
  	}

	if (parser["--help"] == true) {
    	std::cout << parser << std::endl;
		exit(0);
	}
	if (parser["--debug"] == true) {
    	printf("[.] debug enabled\n");
		flag_debug = true;
		Utils::set_debug_state(true);
	}
	if (parser["--verbose"] == true) {
    	printf("[.] verbosity increased\n");
		flag_verbose = true;
		Utils::set_verbose_state(true);
	}
	if (parser.present<std::string>("--cpu")) {
		craisin_cpu_model = cpu_get_model(parser.get<std::string>("--cpu"));
		if (craisin_cpu_model == CPU_INVALID) {
			printf("[!] Invalid CPU model\n");
			exit(0);
		} else Utils::debug_printf("[.] CPU model: %s\n", cpu_get_model_string(craisin_cpu_model).c_str());
	}
	if (parser.present<std::string>("--vice")) {
    	Utils::debug_printf("[.] VICE output file: %s\n", parser.get<std::string>("--vice").c_str());
	}

	// Crasin now begins
	craisin_state_t assembler_state;
	// Create various string lists
	if (!assembler_state.file_dir) delete assembler_state.file_dir;
	if (!assembler_state.includelist) delete assembler_state.includelist;
	if (!assembler_state.input_files) delete assembler_state.input_files;
	assembler_state.file_dir = new StringList();
	assembler_state.includelist = new StringList();
	assembler_state.input_files = new StringList();

	try {
		auto files = parser.get<std::vector<std::string>>("files");
		// Print out all the filenames passed in
		Utils::debug_printf("[.] files provided: ");
		for (auto& file : files) { Utils::debug_printf("%s ", file.c_str()); }
		Utils::debug_printf("\n");
		// Now check that each file exists before inserting it into the input list
	    for (auto& file : files) {
			if (std::filesystem::exists(file.c_str()))
				assembler_state.input_files->push(file);
			else
				printf("[!] %s : %s\n", strerror(errno), file.c_str());
		}
	} catch (std::logic_error& e) { printf("No files provided\n"); exit(0); }

	for (int pass = 0; !passlist[pass].name.empty(); pass++) {
		Utils::verbose_printf("[pass %d] %s\n", pass+1, assembler_state.input_files->current());
		(passlist[pass].fn)(&assembler_state);
	}
}

void skip_operand_real(line_t *cl, char **p) {
	if (CURPRAGMA(cl, PRAGMA_NEWSOURCE)) return;
	for (; **p && !isspace(**p); (*p)++);
}

int craisin_next_context(craisin_state_t *as) { }
void craisin_emit(line_t *cl, int byte) { }
void craisin_emitop(line_t *cl, int opc) { }
