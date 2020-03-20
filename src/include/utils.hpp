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

#ifndef HEADER_UTILS
#define HEADER_UTILS

	#include <string>
	#include <sstream>
	#include <iostream>
	#include <type_traits>
	#include <craisin.hpp>

//-----------------------------------------------------------------------------

class Utils {
public:
	inline static bool flag_debug = false;
	static void set_debug_state(bool state) {
		flag_debug = state;
	}

	// base case of recursion, no more arguments
	static void format_impl(std::stringstream& ss, const char* format) {
	    while (*format) {
	        if (*format == '%' && *++format != '%') // %% == % (not a format directive)
	            throw std::invalid_argument("not enough arguments !\n");
	        ss << *format++;
	    }
	}

	template <typename Arg, typename... Args>
	static void format_impl(std::stringstream& ss, const char* format, Arg arg, Args... args) {
	    while (*format) {
	        if (*format == '%' && *++format != '%') {
	            auto current_format_qualifier = *format;
	            switch(current_format_qualifier) {
	                case 'd' : if (!std::is_integral<Arg>()) throw std::invalid_argument("%d introduces integral argument");
	                // etc.
	            }
	            // it's true you'd have to handle many more format qualifiers, but on a safer basis
	            ss << arg; // arg type is deduced
	            return format_impl(ss, ++format, args...); // one arg less
	        }
	        ss << *format++;
	        } // the format string is exhausted and we still have args : throw
	    throw std::invalid_argument("Too many arguments\n");
	}

	template <typename... Args>
	static std::string formats(const char* fmt, Args... args) {
	    std::stringstream ss;
	    format_impl(ss, fmt, args...);
	    return ss.str();
	}

	template <typename... Args>
	static void debug_printf(const char* fmt, Args... args) {
		if (flag_debug) {
			std::stringstream ss;
	    	format_impl(ss, fmt, args...);
			printf(ss.str().c_str());
		}
	}
};

#endif
