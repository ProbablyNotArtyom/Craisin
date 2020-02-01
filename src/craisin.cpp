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
	#include <getopt.h>

	#include "cpu.h"
	#include "craisin.h"

	using namespace std;
	
//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
	bool debug = false;
	bool verbose = false;
	int opt;
	int digit_optind = 0;
	// Parse the command line options and handle and flags
	while (1) {
		int this_option_optind = optind ? optind : 1;
	    int option_index = 0;
	    static struct option long_options[] = {
	        {"help", no_argument, 0, 'h'},
	        {"debug", no_argument, 0, 'd'},
	        {"verbose", no_argument, 0, 'v'},
	        {"cpu", required_argument, 0, 'c'},
	        {"vice", required_argument, 0, 0},
	        {0, 0, 0, 0}
	    };

		opt = getopt_long(argc, argv, "hdvc:", long_options, &option_index);
	    if (opt == -1) break;

		switch (opt) {
			default:
			case 'h':
				fprintf(stderr, "%s", helptxt.c_str());
				exit(0);
			case 'd':
				debug = true;
				break;
			case 'v':
			    verbose = true;
			    break;
			case 'c':
				printf("CPU model: %s\n", optarg);
			    break;
			case 0:
				/* Parse arguments that only have long forms */
				if (long_options[option_index].name == "vice") {
					printf("VICE output file: %s\n", optarg);
				} else {

				}
				break;
		}
	}
}

