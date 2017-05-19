#pragma once

#include "../common.h"

namespace yk {
	template <typename T, typename P>
	struct parselet {
		using return_t = T;
		using parser_t = P;
	};

	template <typename T, typename P>
	class prefix_parselet : public parselet<T, P> {
	public:
		virtual yopt<typename parselet<T, P>::return_t>
		parse(token const& begin, typename parselet<T, P>::parser_t& parser) = 0;
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
		virtual yopt<typename parselet<T, P>::return_t> 
		parse(typename parselet<T, P>::return_t const& left, token const& begin, typename parselet<T, P>::parser_t& parser) = 0;
		
		virtual bool matches(typename parselet<T, P>::return_t const& left, typename parselet<T, P>::parser_t& parser) {
			return true; 
		}
		
		ysize precedence() const {
			return m_Precedence;
		}
	};
}
