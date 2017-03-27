#pragma once

#include <iostream>

namespace yk {
	class yerror {
	public:
		virtual void print(std::ostream& os) const = 0;
	};

	std::ostream& operator<<(std::ostream& os, yerror const& err);
}
