#pragma once

namespace yk {
	template <typename T>
	struct iterator {
		virtual T next() = 0;
		virtual bool has_next() = 0;
	};
}