#include <Windows.h>
#include "console.h"

namespace yk {
	ysize console::Width;
	ysize console::Height;
	ysize console::TabSize;

	void console::initialize() {
		CONSOLE_SCREEN_BUFFER_INFO csbi;

		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		Width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		Height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
		TabSize = 8;
	}
}
