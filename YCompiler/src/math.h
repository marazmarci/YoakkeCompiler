#pragma once

#include <cassert>
#include "common.h"

namespace math {
	/**
	 * Calculates the amount of digits needed to print a number.
	 * @param num The number to calculate the digit count of.
	 * @return The number of digits of the number.
	 */
	template <typename T>
	static ysize digit_count(T const& num) {
		return std::to_string(num).length();
	}

	/**
	 * Clamps a value between two other values using a custom comparator.
	 * @param v The value to clamp.
	 * @param lo The lower bound of the clamp.
	 * @param hi The upper bound of the clamp.
	 * @param comp The comparator to use.
	 * @return The clamped value.
	 */
	template <class T, class Compare>
	constexpr T const& clamp(T const& v, T const& lo, T const& hi, Compare comp) {
		assert(!comp(hi, lo) && "Illegal arguments for clamp!");
		return comp(v, lo) ? lo : comp(hi, v) ? hi : v;
	}

	/**
	 * Clamps a value between two other values.
	 * @param v The value to clamp.
	 * @param lo The lower bound of the clamp.
	 * @param hi The upper bound of the clamp.
	 * @return The clamped value.
	 */
	template <class T>
	constexpr T const& clamp(T const& v, T const& lo, T const& hi) {
		return clamp(v, lo, hi, std::less<>());
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
	bool in_range(T const& val, T const& start, T const& len) {
		return val >= start && val < start + len;
	}
}
