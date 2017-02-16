#pragma once

#include "token.h"

namespace yk {
	template <typename T, typename P>
	class infix_parselet {
	public:
		const ysize Precedence;

	public:
		infix_parselet(ysize prec)
			: Precedence(prec) {
		}

	public:
		virtual T* parse(T* left, token const& begin, P* parser) = 0;
	};
}