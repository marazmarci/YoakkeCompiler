#pragma once

#include "rules.h"
#include "yparser.h"
#include "../ast/type_desc.h"

namespace yk {
	namespace type_rules {
		using type_pre_parselet = prefix_parselet<type_desc, yparser>;
		using type_in_parselet = infix_parselet<type_desc, yparser>;

		template <typename Return_T>
		class pass : public type_pre_parselet {
		public:
			yshared_ptr<type_desc> parse(token const& begin, yparser& par) override {
				return std::make_shared<Return_T>(begin);
			}
		};

		using ident = pass<ident_type_desc>;

		class enclose : public type_pre_parselet {
		public:
			yshared_ptr<type_desc> parse(token const& begin, yparser& par) override {
				if (auto sub = par.parse_type_desc()) {
					if (par.match(ytoken_t::Rpar)) {
						return sub;
					}
					else {
						token const& tok = par.peek();
						throw_expect("')'", par);
					}
				}
				else {
					if (auto end = par.match(ytoken_t::Rpar)) {
						return std::make_shared<unit_type_desc>(begin, end.value());
					}
					else {
						token const& tok = par.peek();
						throw_expect("type", par);
					}
				}
			}
		};

		class list : public type_in_parselet {
		public:
			list(ysize prec)
				: type_in_parselet(prec) {
			}

		public:
			yshared_ptr<type_desc> parse(
				yshared_ptr<type_desc> left, token const& begin, yparser& par) override {
				auto ls = std::make_shared<list_type_desc>(left);
				do {
					if (auto rhs = par.parse_type_desc(precedence() - 1)) {
						ls->add(rhs);
					}
					else {
						throw_expect("type", par);
					}
				} while (par.match(ytoken_t::Comma));
				return ls;
			}
		};

		template <bool Right>
		class bin : public type_in_parselet {
		public:
			bin(ysize prec)
				: type_in_parselet(prec) {
			}

		public:
			yshared_ptr<type_desc> parse(
				yshared_ptr<type_desc> left, token const& begin, yparser& par) override {
				if (auto rhs = par.parse_type_desc(precedence() - (Right ? 1 : 0))) {
					return std::make_shared<bin_type_desc>(begin, left, rhs);
				}
				else {
					throw_expect("type", par);
				}
			}
		};

		using lbinop = bin<false>;
		using rbinop = bin<true>;
	}
}
