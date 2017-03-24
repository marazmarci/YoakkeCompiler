#include "fmt_in.h"

namespace yk {
	std::string& operator>>(line_at& lat, std::string& str) {
		std::size_t cnt = lat.Index;
		while (cnt--) {
			std::getline(lat.Stream, str);
		}
		return str;
	}
}
