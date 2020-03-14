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
	#include <string>
	#include <list>

	#include <argparse.hpp>
	#include <cpu.hpp>
	#include <craisin.hpp>
	
	#include "parse/expr.hpp"
	#include "parse/token.hpp"
	
	using namespace std;
	
//-----------------------------------------------------------------------------

struct assembler_pass {
	const string	name;
	const pass_fn_t	fn;
	const int 		fordep;
} passlist[] = {
	{ string("parse"), nullptr, 1 },
	{ string("symcheck"), nullptr, 0 },
	{ string("resolve1"), nullptr, 0 },
	{ string("resolve2"), nullptr, 0 },
	{ string("addressresolve"), nullptr, 0 },
	{ string("finalize"), nullptr, 0 },
	{ string("emit"), nullptr, 0 },
	{ string(), nullptr, 0 }
};

static string helptxt = {
	"Usage: craisin [options] [sources] \r\n"
	"6502/65816 assembler supporting variable syntax\r\n"
	"\r\n"
	"  -h, --help           shows this help text\r\n"
	"  -d, --debug          enables debug mode\r\n"
	"  -v, --verbose        enables verbose logging\r\n"
	"  -c, --cpu=CPU        uses CPU as the target processor\r\n"
	"      --vice=FILE      outputs a VICE symbol file to FILE\r\n"
};

static struct option craisin_options[] = {
	{"help", no_argument, 0, 'h'},
	{"debug", no_argument, 0, 'd'},
	{"verbose", no_argument, 0, 'v'},
	{"cpu", required_argument, 0, 'c'},
	{"vice", required_argument, 0, 0},
	{0, 0, 0, 0}
};

bool flag_debug = false;
bool flag_verbose = false;
cpu_model_t craisin_cpu_model;

//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
	// Parse the command line options and handle flags
	while (1) {
	    int option_index = 0;
		int opt = argparse_long(argc, argv, "hdvc:", craisin_options, &option_index);
	    if (opt == -1) break;

		switch (opt) {
			default:
			case 'h':
				cerr << helptxt;
				exit(0);
			case 'd':
				flag_debug = true;
				break;
			case 'v':
				flag_verbose = true;
			    break;
			case 'c':
				craisin_cpu_model = cpu_get_model(optarg);
				if (craisin_cpu_model == CPU_INVALID) {
					cerr << "[!] Invalid CPU model";
					exit(0);
				} else if (flag_debug) {
					cout << "CPU model: " << cpu_get_model_string(craisin_cpu_model) << "\n";
				}
				break;
			case 0:
				/* Parse arguments that only have long forms */
				if (craisin_options[option_index].name == string("vice")) {
					if (flag_debug) cout << "VICE output file: "<< optarg << "\n";
				}
				break;
		}
	}
	
	// Crasin now begins
	for (int pass = 0; !passlist[pass].name.empty(); pass++) {
		if (flag_debug) cout << passlist[pass].name << '\n';
	}
}




