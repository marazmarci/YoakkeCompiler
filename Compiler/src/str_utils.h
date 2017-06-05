/**
 * common.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  Here are various functions used for string formatting used mainly by error
 *  and warning reports.
 */

#pragma once

#include <cassert>
#include <cstring>
#include "common.h"

namespace str_utils {
	/**
	 * Converts a C-style string to C++ string.
	 * @param val The C string to convert.
	 * @return A C++ string containing the C string.
	 */
	inline ystr to_str(const char* val) {
		return ystr(val);
	}

	/**
	 * Converts an int to a C++ string.
	 * @param val The int to convert.
	 * @return A C++ string representing the number.
	 */
	inline ystr to_str(int val) {
		return std::to_string(val);
	}

	/**
	 * Converts a long int to a C++ string.
	 * @param val The long int to convert.
	 * @return A C++ string representing the number.
	 */
	inline ystr to_str(long val) {
		return std::to_string(val);
	}

	/**
	 * Converts a long long int to a C++ string.
	 * @param val The long long int to convert.
	 * @return A C++ string representing the number.
	 */
	inline ystr to_str(long long val) {
		return std::to_string(val);
	}

	/**
	 * Converts an unsigned int to a C++ string.
	 * @param val The unsigned int to convert.
	 * @return A C++ string representing the number.
	 */
	inline ystr to_str(unsigned val) {
		return std::to_string(val);
	}

	/**
	 * Converts an unsigned long int to a C++ string.
	 * @param val The unsigned long int to convert.
	 * @return A C++ string representing the number.
	 */
	inline ystr to_str(unsigned long val) {
		return std::to_string(val);
	}

	/**
	 * Converts an unsigned long long int to a C++ string.
	 * @param val The unsigned long long int to convert.
	 * @return A C++ string representing the number.
	 */
	inline ystr to_str(unsigned long long val) {
		return std::to_string(val);
	}

	/**
	 * Converts a float to a C++ string.
	 * @param val The float to convert.
	 * @return A C++ string representing the number.
	 */
	inline ystr to_str(float val) {
		return std::to_string(val);
	}

	/**
	 * Converts a double to a C++ string.
	 * @param val The double to convert.
	 * @return A C++ string representing the number.
	 */
	inline ystr to_str(double val) {
		return std::to_string(val);
	}

	/**
	 * Converts a long double to a C++ string.
	 * @param val The long double to convert.
	 * @return A C++ string representing the number.
	 */
	inline ystr to_str(long double val) {
		return std::to_string(val);
	}

	namespace {
		/**
		 * Helper function for fmt().
		 * @see fmt
		 */
		template <typename... Ts>
		inline void fmt_impl(ystr& buf, const char* pos, ysize rem, Ts const&... back) {
			fmt_impl_1(buf, pos, rem, back...);
		}

		/**
		 * Helper function for fmt().
		 * @see fmt
		 */
		template <typename Tf, typename... Ts>
		inline void fmt_impl_1(ystr& buf, const char* pos, ysize rem, Tf const& front, Ts const&... back) {
			for (; rem && *pos; ++pos, --rem) {
				if (*pos == '%') {
					if (*++pos && *pos == '%') {
						buf += '%';
					}
					else {
						buf += to_str(front);
						fmt_impl(buf, pos, rem, back...);
						return;
					}
				}
				else {
					buf += *pos;
				}
			}
		}

		/**
		 * Helper function for fmt().
		 * @see fmt
		 */
		inline void fmt_impl_1(ystr& buf, const char* pos, ysize rem) {
			for (; rem && *pos; ++pos, --rem) {
				if (*pos == '%') {
					if (*++pos && *pos == '%') {
						buf += '%';
					}
					else {
						assert(false && "str_utils::fmt was called with a wrong number of arguments!");
					}
				}
				else {
					buf += *pos;
				}
			}
		}
	}

	/**
	 * Formats a C string with a certain length by replacing all occurrences 
	 * of '%' characters with the parameters supported, converted to strings
	 * using the to_str() functions. A double '%' results in a single '%',
	 * similar to printf().
	 * @see to_str
	 * @param orig The string to format.
	 * @param others... The values to insert into the string.
	 * @return A formatted C++ string.
	 */
	template <typename... Ts>
	ystr fmt(const char* orig, ysize len, Ts const&... values) {
		ystr result;
		fmt_impl(result, orig, len, values...);
		return result;
	}

	/**
	 * Formats a C string by replacing all occurrences of '%' characters with
	 * the parameters supported, converted to strings using the to_str()
	 * functions. A double '%' results in a single '%', similar to printf().
	 * The implementation just calls the C string version with explicit length.
	 * @see fmt
	 * @see to_str
	 * @param orig The string to format.
	 * @param others... The values to insert into the string.
	 * @return A formatted C++ string.
	 */
	template <typename... Ts>
	ystr fmt(const char* orig, Ts const&... values) {
		return fmt(orig, std::strlen(orig), values...);
	}

	/**
	 * Formats a string by replacing all occurrences of '%' characters with
	 * the parameters supported, converted to strings using the to_str()
	 * functions. A double '%' results in a single '%'. Similar to printf().
	 * The implementation just calls the C string version with explicit length.
	 * @see fmt
	 * @see to_str
	 * @param orig The string to format.
	 * @param others... The values to insert into the string.
	 * @return A formatted C++ string.
	 */
	template <typename... Ts>
	ystr fmt(ystr const& orig, Ts const&... values) {
		return fmt(orig.c_str(), orig.length(), values...);
	}
}
