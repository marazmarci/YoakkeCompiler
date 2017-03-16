#pragma once

#include "parselet.h"
#include "parser.h"
#include "../lexing/token.h"
#include "../common.h"

namespace yk {
	template <typename T, typename P>
	class prec_parser : public parser {
	private:
		typedef prefix_parselet<T, P> pre_rule;
		typedef infix_parselet<T, P> in_rule;

	private:
		yopt_map<token::type_t, yshared_ptr<pre_rule>>	m_Prefix;
		yopt_map<token::type_t, yshared_ptr<in_rule>>	m_Infix;

	public:
		prec_parser(lexer& lex, yvec<token>& buff)
			: parser(lex, buff) {
		}

	public:
		void register_rule(token::type_t const& val, yshared_ptr<pre_rule> r) {
			if (auto p = m_Prefix.at(val)) {
				throw std::exception("Prefix parselet already defined!");
			}
			m_Prefix.insert(std::make_pair(val, r));
		}

		void register_rule(token::type_t const& val, yshared_ptr<in_rule> r) {
			if (auto p = m_Infix.at(val)) {
				throw std::exception("Infix parselet already defined!");
			}
			m_Infix.insert(std::make_pair(val, r));
		}

		yshared_ptr<T> parse(P& par, ysize prec = 0) {
			auto lookahead = peek();
			if (auto pre_parselet = m_Prefix.at(lookahead.Type)) {
				lookahead = consume();
				auto left = pre_parselet->parse(lookahead, par);
				if (left) {
					while (prec < get_precedence()) {
						lookahead = peek();
						if (auto in_parselet = m_infix.at(lookahead.Type)) {
							if (in_parselet->matches(left)) {
								lookahead = consume();
								auto newleft = 
								if (!(left = in_parselet->parse(left, lookahead, par))) {
									throw std::exception("Parse RHS returned with null");
								}
							}
							else {
								break;
							}
						}
						else {
							throw std::exception("No matching infix parselet!");
						}
					}
				}
				return left;
			}
			return nullptr;
		}

	private:
		ysize get_precedence() {
			auto parser = ext::get_value(m_Infix, peek().identifier());
			if (auto parselet = m_Infix.at(peek().Type)) {
				return parselet->precedence();
			}
			return 0;
		}
	};
}
