#include "yerror.h"

namespace yk {
	std::ostream& operator<<(std::ostream& os, yerror const& err) {
		err.print(os);
		return os;
	}
}
