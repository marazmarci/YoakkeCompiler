#pragma once

#include "token.h"

namespace yk {
	template <typename T, typename P>
	class prefix_parselet {
	public:
		virtual T* parse(token const& begin, P* parser) = 0;
	};
}