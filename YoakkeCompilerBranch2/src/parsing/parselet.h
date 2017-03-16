#pragma once

#include "../common.h"

namespace yk {
	template <typename T, typename P>
	class prefix_parselet {
	public:
		virtual yshared_ptr<T> parse(token const& begin, P& parser) = 0;
	};

	template <typename T, typename P>
	class infix_parselet {
	private:
		ysize m_Precedence;

	public:
		infix_parselet(ysize prec)
			: m_Precedence(prec) {
		}

	public:
		virtual yshared_ptr<T> parse(yshared_ptr<T> left, token const& begin, P& parser) = 0;
		inline ysize precedence() const { return m_Precedence; }
		virtual bool matches(T* left) { return true; }
	};
}
