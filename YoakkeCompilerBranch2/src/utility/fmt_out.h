#pragma once

#include <cstddef>
#include <iostream>

namespace yk {
	struct cmd_pos {
		std::size_t x;
		std::size_t y;

		cmd_pos(std::size_t x, std::size_t y)
			: x(x), y(y) {
		}
	};

	struct skip {
		std::size_t Count;

		skip(std::size_t cnt)
			: Count(cnt) {
		}
	};

	struct repeat {
		std::size_t Count;
		char Ch;

		repeat(std::size_t cnt, char c)
			: Count(cnt), Ch(c) {
		}
	};

	struct part_until {
		const char* Begin;
		char What;

		part_until(const char* beg, char w)
			: Begin(beg), What(w) {
		}
	};

	std::ostream& operator<<(std::ostream& out, cmd_pos const& pos);
	std::ostream& operator<<(std::ostream& out, skip const& sk);
	std::ostream& operator<<(std::ostream& out, repeat const& rep);
	std::ostream& operator<<(std::ostream& out, part_until const& po);
}
