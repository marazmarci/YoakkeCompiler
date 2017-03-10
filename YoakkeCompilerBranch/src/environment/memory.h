#pragma once

#include "../common.h"

namespace yk {
	namespace mem {
		template <typename T>
		T swap_endian(T u) {
			union {
				T u;
				ybyte u8[sizeof(T)];
			} source, dest;

			source.u = u;
			for (ysize k = 0; k < sizeof(T); k++) {
				dest.u8[k] = source.u8[sizeof(T) - k - 1];
			}
			return dest.u;
		}
	}
}
