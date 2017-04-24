#pragma once

#include "../common.h"

namespace yk {
	struct console {
	public:
		static ysize Width;
		static ysize Height;
		static ysize TabSize;

	public:
		static void initialize();

	public:
		console() = delete;
	};
}

