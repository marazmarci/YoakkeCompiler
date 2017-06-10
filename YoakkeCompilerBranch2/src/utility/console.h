#pragma once

#include <Windows.h>
#include "../common.h"
#include "../lexing/position.h"

namespace yk {
	struct console {
	private:
		static HANDLE s_Handle;

	public:
		static ysize Width;
		static ysize Height;
		static ysize TabSize;

	public:
		static void initialize();
		static point get_cursor_pos();
		static void set_cursor_pos(point const& pos);

	public:
		console() = delete;
	};
}

