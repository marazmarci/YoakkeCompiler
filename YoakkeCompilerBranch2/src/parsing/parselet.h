#pragma once

#include "../common.h"

namespace yk {
	template <typename T, typename P>
	struct parselet {
		using return_t = T;
		using return_ptr = ysptr<return_t>;
		using parser_t = P;
	};

	template <typename T, typename P>
	class prefix_parselet : public parselet<T, P> {
	public:
		virtual return_ptr parse(token const& begin, parser_t& parser) = 0;
	};

	template <typename T, typename P>
	class infix_parselet : public parselet<T, P> {
	private:
		ysize m_Precedence;

	public:
		infix_parselet(ysize prec)
			: m_Precedence(prec) {
		}

	public:
		virtual return_ptr parse(return_ptr left, token const& begin, parser_t& parser) = 0;
		
		virtual bool matches(return_ptr left, parser_t& parser) {
			return true; 
		}
		
		ysize precedence() const {
			return m_Precedence;
		}
	};
}
