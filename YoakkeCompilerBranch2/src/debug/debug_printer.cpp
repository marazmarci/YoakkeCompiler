#include <cassert>
#include <mach7/match.hpp>
#include <mach7/patterns/constructor.hpp>
#include "debug_printer.h"
#include "../ast/expr.h"

namespace mch {
	using namespace yk;

	template <> struct bindings<ident_expr>		{ Members(ident_expr::Identifier); };
	template <> struct bindings<int_lit_expr>	{ Members(int_lit_expr::Value); };
	template <> struct bindings<real_lit_expr>	{ Members(real_lit_expr::Value); };
	template <> struct bindings<preury_expr>	{ Members(preury_expr::Operator, preury_expr::Sub); };
	template <> struct bindings<postury_expr>	{ Members(postury_expr::Operator, postury_expr::Sub); };
	template <> struct bindings<binop_expr>		{ Members(binop_expr::Operator, binop_expr::LHS, binop_expr::RHS); };
	template <> struct bindings<asgn_expr>		{ Members(asgn_expr::Operator, asgn_expr::LHS, asgn_expr::RHS); };
	template <> struct bindings<const_asgn_expr>{ Members(const_asgn_expr::Operator, const_asgn_expr::LHS, const_asgn_expr::RHS); };
	template <> struct bindings<list_expr>		{ Members(list_expr::Expressions); };
	template <> struct bindings<call_expr>		{ Members(call_expr::Function, call_expr::Args); };
}

namespace yk {
	void dbg_printer::print(std::ostream& os, yshared_ptr<expr> const& what) {
		mch::var<ystr> str;
		mch::var<long long> ival;
		mch::var<long double> dval;
		mch::var<token> tok;
		mch::var<yshared_ptr<expr>> exp, exp2;
		mch::var<yvec<yshared_ptr<expr>>> list;

		Match (*what.get())
		{
			Case(ident_expr, str) {
				os << str;
				return;
			}
			Case(int_lit_expr, ival) {
				os << ival;
				return;
			}
			Case(real_lit_expr, dval) {
				os << dval;
				return;
			}
			Case(preury_expr, tok, exp) {
				os << tok.Value << '(';
				print(os, exp);
				os << ')';
				return;
			}
			Case(postury_expr, tok, exp) {
				os << '(';
				print(os, exp);
				os << ')' << tok.Value;
				return;
			}
			Case(binop_expr, tok, exp, exp2) {
				os << '(';
				print(os, exp);
				os << tok.Value;
				print(os, exp2);
				os << ')';
				return;
			}
			Case(asgn_expr, tok, exp, exp2) {
				os << '(';
				print(os, exp);
				os << tok.Value;
				print(os, exp2);
				os << ')';
				return;
			}
			Case(const_asgn_expr, tok, exp, exp2) {
				print(os, exp);
				os << tok.Value;
				print(os, exp2);
				return;
			}
			Case(list_expr, list) {
				if (list.size()) {
					print(os, list[0]);
				}
				for (ysize i = 1; i < list.size(); i++) {
					os << ", "; 
					print(os, list[i]);
				}
				return;
			}
			Case(call_expr, exp, exp2) {
				print(os, exp);
				os << '(';
				if (exp2) {
					print(os, exp2);
				}
				os << ')';
				return;
			}
			Otherwise() {
				throw std::exception("Unhandled visit for debug print!");
			}
		}
		EndMatch
	}
}
