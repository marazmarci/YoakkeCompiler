/**
 * math.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  This file contains various math functions that improve readability of the
 *  code.
 */

#pragma once

#include "str_utils.h"

namespace math {
	/**
	 * Calculates the amount of digits needed to print a number. Uses the
	 * str_utils::to_str().
	 * @see str_utils::to_str
	 * @param num The number to calculate the digit count of.
	 * @return The number of digits of the number.
	 */
	template <typename T>
	static inline ysize digit_count(T const& num) {
		return str_utils::to_str(num).length();
	}

	/**
	 * Determines if a given value is in a specified range. Inclusive at the
	 * start, exclusive at the end (start <= value < start + length).
	 * @param val The value checked in the range.
	 * @param start The start of the range.
	 * @param len The length of the range.
	 * @return True, if the value is in the range.
	 */
	template <typename T>
	inline bool in_range(T const& val, T const& start, T const& len) {
		return val >= start && val < start + len;
	}
}
