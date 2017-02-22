#pragma once

#include "token.h"

namespace yk {
	template <typename T, typename P>
	class infix_parselet {
	private:
		const ysize m_Precedence;

	public:
		infix_parselet(ysize prec)
			: m_Precedence(prec) {
		}

	public:
		virtual T* parse(T* left, token const& begin, P* parser) = 0;
		inline ysize precedence() const { return m_Precedence; }
		virtual bool matches(T* left) { return true; }
	};
}