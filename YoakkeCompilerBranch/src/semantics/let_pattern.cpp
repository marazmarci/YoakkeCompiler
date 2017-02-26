#include "let_pattern.h"
#include "type_symbol.h"
#include "../ast/pattern.h"
#include "symbol_table.h"
#include "algorithms.h"
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
					lm.add(ip->Identifier, exp);
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
				if (bp->OP.identifier() == ",") {
					auto flat = alg::flatten<pattern, bin_pattern>(bp, ",");
					if (right) {
						if (auto rtup = dynamic_cast<tuple_type_symbol*>(right)) {
							if (rtup->Types.size() == flat.size()) {
								for (ysize i = 0; i < flat.size(); i++) {
									// TODO
									define(table, flat[i], rtup->Types[i], exp, lm);
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
						for (ysize i = 0; i < flat.size(); i++) {
							// TODO
							define(table, flat[i], nullptr, exp, lm);
						}
					}
				}
				else {
					throw std::exception("UNHANDLED PATTERN (bin)");
				}
			}
			else if (auto ep = dynamic_cast<enclose_pattern*>(left)) {
				// TODO
				return define(table, ep->Sub, right, exp, lm);
			}
			else {
				throw std::exception("UNHANDLED PATTERN");
			}
		}
	}
}