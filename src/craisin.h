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

//-----------------------------------------------------------------------------

/* The maximum depth that scopes can be nested */
#define MAX_SCOPE_DEPTH		16

/* The maximum number of conditional statements that can be nested */
#define MAX_COND_DEPTH		64

//-----------------------------------------------------------------------------

enum statusCode {
	STATUS_OK,							// Normal idle status

	STATUS_ERRORS,						// Marks where statuses get treated like errors
	ERROR_INTERNAL = STATUS_ERRORS,		// Unknown internal error

	NUM_STATUSCODES
};

//-----------------------------------------------------------------------------

extern const char *statusStrings[NUM_STATUSCODES];

#endif
