#pragma once

#include "parser.h"
#include "prefix_parselet.h"
#include "infix_parselet.h"
#include "../utility/map_ext.h"

namespace yk {
	template <typename T>
	class prec_parser : public parser {
	private:
		typedef prefix_parselet<T, prec_parser<T>> pre_rule;
		typedef infix_parselet<T, prec_parser<T>> in_rule;

	private:
		ymap<ystr, pre_rule*>	m_Prefix;
		ymap<ystr, in_rule*>	m_Infix;

	public:
		prec_parser(token_buffer* buff)
			: parser(buff) {
		}

	private:
		template <typename T>
		void register_generic(ystr const& val, T* r, ymap<ystr, T*>& map) {
			auto it = map.find(val);
			if (it == map.end()) {
				map.insert(std::make_pair(val, r));
			}
			else {
				throw std::exception("Parselet already registered!");
			}
		}

	public:
		void register_rule(ystr const& val, pre_rule* r) {
			register_generic(val, r, m_Prefix);
		}

		void register_rule(ystr const& val, in_rule* r) {
			register_generic(val, r, m_Infix);
		}

		ysize get_precedence() {
			auto parser = ext::get_value(m_Infix, peek().identifier());
			if (parser) {
				return parser->precedence();
			}
			return 0;
		}

		T* parse() {
			return parse(0);
		}

		T* parse(ysize prec) {
			auto lookahead = peek();
			pre_rule* pre_parselet = ext::get_value(m_Prefix, lookahead.identifier());
			if (!pre_parselet) {
				return nullptr;
			}
			lookahead = consume();
			auto left = pre_parselet->parse(lookahead, this);
			if (left) {
				while (prec < get_precedence()) {
					lookahead = peek();
					in_rule* in_parselet = ext::get_value(m_Infix, lookahead.identifier());
					if (!in_parselet) {
						throw std::exception("No matching infix parselet!");
					}
					if (in_parselet->matches(left)) {
						lookahead = consume();
						left = in_parselet->parse(left, lookahead, this);
					}
					else {
						break;
					}
				}
			}
			return left;
		}
	};
}