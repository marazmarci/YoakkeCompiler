#include "let_pattern.h"
#include "type_symbol.h"
#include "../ast/expr.h"
#include "../ast/pattern.h"

namespace yk {
	namespace let_pat {
		static void define_internal(yvec<ypair<ystr, expr*>>& vec, pattern* left, expr* right) {
			if (auto sk = dynamic_cast<skip_pattern*>(left)) {
				return;
			}
			else if (auto ip = dynamic_cast<ident_pattern*>(left)) {
				vec.push_back(std::make_pair(ip->identifier, right));
				return;
			}
			else if (auto bp = dynamic_cast<bin_pattern*>(left)) {
				throw std::exception("UNHANDLED PATTERN (bin)");
			}
			else if (auto lp = dynamic_cast<list_pattern*>(left)) {
				if (right) {
					if (auto rtup = dynamic_cast<tuple_type_symbol*>(right->EvalType)) {
						if (rtup->Types.size() == lp->List.size()) {
							// List or tuple
							if (auto rlist = dynamic_cast<list_expr*>(right)) {
								for (ysize i = 0; i < lp->List.size(); i++) {
									define_internal(vec, lp->List[i], rlist->List[i]);
								}
							}
							else {
								for (ysize i = 0; i < lp->List.size(); i++) {
									auto right_sub = new bin_expr(right,
										new int_lit_expr(token("Integer", "")),
										token(".", "."));
									right_sub->EvalType = rtup->Types[i];
									define_internal(vec, lp->List[i],
										right_sub);
								}
							}
						}
						else {
							throw std::exception("Pattern does not match (tuple size mismatch)!");
						}
					}
					else {
						throw std::exception("Pattern does not match (RHS is not tuple)!");
					}
				}
				else {
					throw std::exception("Cannot let pattern without RHS!");
				}
			}
			else {
				throw std::exception("UNHANDLED PATTERN");
			}
		}

		yvec<ypair<ystr, expr*>> define(pattern* left, expr* exp) {
			yvec<ypair<ystr, expr*>> result;
			define_internal(result, left, exp);
			return result;
		}
	}
}