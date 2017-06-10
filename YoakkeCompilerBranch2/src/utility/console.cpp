#include "console.h"

namespace yk {
	HANDLE console::s_Handle;
	ysize console::Width;
	ysize console::Height;
	ysize console::TabSize;

	void console::initialize() {
		s_Handle = GetStdHandle(STD_OUTPUT_HANDLE);

		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(s_Handle, &csbi);
		Width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		Height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
		TabSize = 8;
	}

	point console::get_cursor_pos() {
		static CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(s_Handle, &csbi);
		return point(csbi.dwCursorPosition.Y, csbi.dwCursorPosition.X);
	}

	void console::set_cursor_pos(point const& pos) {
		COORD pos_p = { pos.Col, pos.Row };
		SetConsoleCursorPosition(s_Handle, pos_p);
	}
}
