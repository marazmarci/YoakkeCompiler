/**
 * fnl_utils.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  These are functional utility functions used by the compiler in various
 *  places.
 */

#pragma once

#include "common.h"

namespace fnl {
	template <typename T>
	inline auto& unwrap(T& el) {
		return el;
	}

	template <typename T>
	inline auto& unwrap(yopt<T>& el) {
		return el.value();
	}
}
