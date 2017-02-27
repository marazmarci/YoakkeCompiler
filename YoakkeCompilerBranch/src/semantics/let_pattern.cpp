#include "let_pattern.h"
#include "type_symbol.h"
#include "../ast/pattern.h"
#include "symbol_table.h"
#include "typed_symbol.h"
#include "../ast_meta/let_meta.h"

namespace yk {
	namespace let_pat {
		// TODO: sub-pattern-> exp as new binexp(exp . IDX)
		void define(symbol_table& table, pattern* left, type_symbol* right, expr* exp, let_meta& lm) {
			if (auto sk = dynamic_cast<skip_pattern*>(left)) {
				return;
			}
			else if (auto ip = dynamic_cast<ident_pattern*>(left)) {
				auto sym_set = table.ref(ip->Identifier);
				auto typed_set = symbol_table::filter<var_symbol>(sym_set);
				if (typed_set.size() == 0) {
					// New symbol
					table.decl(new var_symbol(ip->Identifier, right));
					lm.add(ip->Identifier, exp, right);
				}
				else if (typed_set.size() == 1) {
					// Shadowing
					// TODO
					typed_set[1]->Type = right;
				}
				else {
					throw std::exception("Let sanity!");
				}
			}
			else if (auto bp = dynamic_cast<bin_pattern*>(left)) {
				throw std::exception("UNHANDLED PATTERN (bin)");
			}
			else if (auto lp = dynamic_cast<list_pattern*>(left)) {
				if (right) {
					if (auto rtup = dynamic_cast<tuple_type_symbol*>(right)) {
						if (rtup->Types.size() == lp->List.size()) {
							for (ysize i = 0; i < lp->List.size(); i++) {
								// TODO
								define(table, lp->List[i], rtup->Types[i], exp, lm);
							}
						}
						else {
							throw std::exception("Pattern does not match (tuple size mismatch)!");
						}
					}
					else {
						throw std::exception("Pattern does not match RHS!");
					}
				}
				else {
					// Unknown typed expansion
					for (ysize i = 0; i < lp->List.size(); i++) {
						// TODO
						define(table, lp->List[i], nullptr, exp, lm);
					}
				}
			}
			else {
				throw std::exception("UNHANDLED PATTERN");
			}
		}
	}
}