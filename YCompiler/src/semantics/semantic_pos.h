#pragma once

#include "../common.h"
#include "../io/file_hnd.h"
#include "../syntax/position.h"

struct semantic_pos {
	file_hnd const* File;
	interval		Pos;

	semantic_pos(file_hnd const& f, interval const& pos)
		: File(&f), Pos(pos) {
	}

	static bool same_file(semantic_pos const& a, semantic_pos const& b) {
		return a.File == b.File;
	}
};
