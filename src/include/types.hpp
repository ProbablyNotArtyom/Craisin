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

#ifndef HEADER_TYPES
#define HEADER_TYPES

#include <stdint.h>
#include <string.h>
#include <limits>

//-----------------------------------------------------------------------------

typedef uint16_t address_t;		// 16bit absolute address
typedef uint8_t ubyte_t;		// 8bit byte
typedef int8_t byte_t;			// 8bit byte, signed

inline ubyte_t get_lobyte(address_t addr) {
	return static_cast<ubyte_t>(addr & 0xFF);
}

inline ubyte_t get_hibyte(address_t addr) {
	return static_cast<ubyte_t>(addr >> 8);
}

//-----------------------------------------------------------------------------

#endif

