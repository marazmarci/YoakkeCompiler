#pragma once

#include "parselet.h"
#include "parser.h"
#include "../lexing/token.h"
#include "../common.h"

namespace yk {
	template <typename T, typename P>
	class prec_parser : public parser {
	private:
		using pre_rule	= prefix_parselet<T, P>;
		using in_rule	= infix_parselet<T, P>;

	private:
		yopt_map<ytoken_t, ysptr<pre_rule>>	m_Prefix;
		yopt_map<ytoken_t, ysptr<in_rule>>	m_Infix;

	public:
		prec_parser(lexer& lex, yvec<token>& buff)
			: parser(lex, buff) {
		}

	public:
		void register_rule(ytoken_t val, ysptr<pre_rule> r) {
			if (auto p = m_Prefix.at(val)) {
				throw std::exception("Prefix parselet already defined!");
			}
			m_Prefix.insert({ val, r });
		}

		void register_rule(ytoken_t val, ysptr<in_rule> r) {
			if (auto p = m_Infix.at(val)) {
				throw std::exception("Infix parselet already defined!");
			}
			m_Infix.insert({ val, r });
		}

		ysptr<T> parse(P& par, ysize prec = 0) {
			auto lookahead = peek();
			if (auto pre_parselet = m_Prefix.at(lookahead.Type)) {
				lookahead = consume();
				auto left = pre_parselet->get()->parse(lookahead, par);
				if (left) {
					while (prec < get_precedence()) {
						lookahead = peek();
						if (auto in_parselet = m_Infix.at(lookahead.Type)) {
							if (in_parselet->get()->matches(left, par)) {
								lookahead = consume();
								if (!(left = in_parselet->get()->parse(left, lookahead, par))) {
									//throw std::exception("Parse RHS returned with null");
									return nullptr;
								}
							}
							else {
								break;
							}
						}
						else {
							//throw std::exception("No matching infix parselet!");
							return nullptr;
						}
					}
				}
				return left;
			}
			return nullptr;
		}

	private:
		ysize get_precedence() {
			if (auto parselet = m_Infix.at(peek().Type)) {
				return parselet.value().get()->precedence();
			}
			return 0;
		}
	};
}
