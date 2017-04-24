#pragma once

#include "../common.h"

namespace yk {
	namespace math {
		template <class T>
		ysize digit_count(T number) {
			ysize digits = 0;
			if (number < 0) digits = 1;
			while (number) {
				number /= 10;
				digits++;
			}
			return digits;
		}
	}
}
