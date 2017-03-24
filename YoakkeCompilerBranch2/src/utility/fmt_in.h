#pragma once

#include <fstream>
#include <string>

namespace yk {
	struct line_at {
		std::ifstream& Stream;
		std::size_t Index;

		line_at(std::ifstream& st, std::size_t idx)
			: Stream(st), Index(idx) {
		}
	};

	std::string& operator>>(line_at& lat, std::string& str);
}
