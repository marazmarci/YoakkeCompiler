#include <Windows.h>
#include "fmt_out.h"

namespace yk {
	std::ostream& operator<<(std::ostream& out, cmd_pos const& pos) {
		COORD pos_p = { pos.x, pos.y };
		HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleCursorPosition(output, pos_p);
		return out;
	}

	std::ostream& operator<<(std::ostream& out, skip const& sk) {
		out << repeat(sk.Count, ' ');
		return out;
	}

	std::ostream& operator<<(std::ostream& out, repeat const& rep) {
		int cnt = rep.Count;
		char c = rep.Ch;
		while (cnt-- >= 0) {
			out << c;
		}
		return out;
	}
}