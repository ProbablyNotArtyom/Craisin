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

#ifndef HEADER_CPU
#define HEADER_CPU

#include <string>

//-----------------------------------------------------------------------------

#define NUM_CPU_MODELS	6
typedef enum {
	CPU_INVALID = 0,
	CPU_6502,
	CPU_6502_ILLEGAL,
	CPU_65C02,
	CPU_65C02_WDC,
	CPU_65816
} cpu_model_t;
extern const std::string cpuModels_tbl[NUM_CPU_MODELS];

//-----------------------------------------------------------------------------

cpu_model_t cpu_get_model(std::string model_name);
std::string cpu_get_model_string(cpu_model_t model);

//-----------------------------------------------------------------------------

#endif

