#include "checker.h"
#include "../utility/match.h"
#include "../ast/ast.h"
#include "type.h"
#include "var_sym.h"
#include "../reporting/err_stream.h"
#include "../reporting/err_msg.h"
#include "scope.h"

namespace yk {
	checker::checker(file_handle const& f)
		: m_File(f) {
	}

	void checker::check_stmt(stmt& st) {
		match(st.Data) (
			[&](ysptr<expr_stmt> exp) { 
			bind(exp->as(), [&](ysptr<expr>& Sub, bool& Semicol, bool& DoesReturn) {
				if (bool* ret_dst = get_braced(*Sub)) {
					// Braced statements are not return destinations
					*ret_dst = false;
					check_expr(*Sub);
				}
				else if (
					!Semicol
				 && !std::get_if<ysptr<const_asgn_expr>>(&Sub->Data)) {
					// Not a constant assignment and does not end with semicolon
					DoesReturn = true;
					auto ret_ty = check_expr(*Sub);
					auto ret_dest = m_Table.enclosing_return();
					if (!ret_dest) {
						throw std::exception("Sanity error: no enclosing return destination!");
					}
					if (auto o_ret_ty = ret_dest->ReturnType) {
						unify(ret_ty, *o_ret_ty);
					}
					else {
						ret_dest->ReturnType = ret_ty;
						ret_dest->ReturnPos = st.Position;
					}
				}
				else {
					check_expr(*Sub);
				}
			}); }
		);
	}

	type checker::check_expr(expr& ex) {
		return match(ex.Data) (
			[&](ysptr<unit_expr>) -> type {
				return symbol_table::UNIT_T;
			},
			[&](ysptr<ident_expr> ie) -> type {
			return bind(ie->as(), [&](ystr& Identifier) {
				ysptr<var_sym> sym	= nullptr;
				bool hint_change	= false;
				if (ex.HintType) {
					std::tie(sym, hint_change) = m_Table.ref(Identifier, *ex.HintType);
				}
				else {
					sym = m_Table.ref(Identifier);
				}

				if (!sym) {
					if (hint_change) {
						rep::err_stream::report(
							rep::no_such_symbol(m_File,
								Identifier, ex.Position,
								"The hint type supported is wrong!",
								ex.HintPosition
							)
						);
					}
					else {
						rep::err_stream::report(
							rep::no_such_symbol(m_File,
								Identifier, ex.Position
							)
						);
					}
				}
				return sym->Type;
			}); },
			[&](ysptr<int_lit_expr>) -> type {
				return symbol_table::I32_T;
			},
			[&](ysptr<real_lit_expr>) -> type {
				return symbol_table::F32_T;
			},
			[&](ysptr<preury_expr>) -> type {
				// TODO
				return symbol_table::UNIT_T;
			},
			[&](ysptr<postury_expr>) -> type {
				// TODO
				return symbol_table::UNIT_T;
			},
			[&](ysptr<binop_expr>) -> type {
				// TODO
				return symbol_table::UNIT_T;
			},
			[&](ysptr<asgn_expr> ae) -> type {
			return bind(ae->as(), [&](token& Op, expr& LHS, expr& RHS) {
				auto left	= check_expr(LHS);
				auto right	= check_expr(RHS);
				unify(left, right);
				return symbol_table::UNIT_T;
			}); },
			[&](ysptr<const_asgn_expr> ce) -> type {
			return bind(ce->as(), [&](token& Op, expr& LHS, expr& RHS) {
				auto& left = std::get<0>(*std::get<ysptr<ident_expr>>(LHS.Data));
				auto right = check_expr(RHS);

				auto sym_d = m_Table.ref(left, right).first;
				if (sym_d) {
					std::string err = "TODO: cannot shadow constant assignment: " + left;
					throw std::exception(err.c_str());
				}
				auto sym = std::make_shared<constant>(left, right);
				m_Table.decl(sym);
				return symbol_table::UNIT_T;
			}); },
			[&](ysptr<list_expr> le) -> type {
			return bind(le->as(), [&](yvec<expr>& Elements) {
				yvec<type> types;
				for (auto e : Elements) {
					types.push_back(check_expr(e));
				}
				return type::create_tup(types);
			}); },
			[&](ysptr<call_expr> ce) -> type {
			return bind(ce->as(), [&](expr& Function, yopt<expr>& Args) {
				type args_ty = Args ?
					check_expr(*Args)
					: symbol_table::UNIT_T;

				// Help call with filtering
				Function.HintType = type::create_fn(args_ty, type::create_var());;
				auto fn_ty = check_expr(Function);
				return match(fn_ty.Data) (
					[&](ysptr<cons_type> t) -> type {
					return bind(t->as(), [&](ystr& name, yvec<type>& types) -> type {
						if (name != "@fn") {
							throw std::exception("TODO: cannot call non-function expression!");
						}
						return *types.rbegin();
					}); },
					[&](auto&) -> type {
						throw std::exception("TODO: cannot call non-function expression!");
					}
				);
			}); },
			[&](ysptr<block_expr> be) -> type {
			return bind(be->as(), [&](yvec<stmt>& Statements, bool& ReturnDest) {
				auto sc = m_Table.push(ReturnDest);
				for (auto st : Statements) {
					check_stmt(st);
				}
				m_Table.pop();
				return sc->ReturnType.value_or(symbol_table::UNIT_T);
			}); },
			[&](ysptr<fnproto_expr>) -> type {
				// TODO
				return symbol_table::UNIT_T;
			},
			[&](ysptr<fn_expr> fe) -> type {
			return bind(fe->as(), [&](yvec<param_t>& Parameters, yopt<ty_expr>& ReturnType, expr& Body) {
				// TODO: warn if no parameter name
				// TODO: error if same parameter name
				auto sc = m_Table.push(true);

				yvec<type> params;
				for (auto par : Parameters) {
					auto par_ty = check_type(par.second);
					params.push_back(par_ty);
					if (auto name = par.first) {
						auto par_sym = std::make_shared
							<variable>(name.value().Value, par_ty);
						m_Table.decl(par_sym);
					}
				}

				// Return type
				type ret_t = ReturnType ?
					check_type(*ReturnType)
					: symbol_table::UNIT_T;

				// Create the type
				type fn_ty = params.empty() ?
					type::create_fn(symbol_table::UNIT_T, ret_t)
					: (params.size() > 1 ?
						type::create_fn(type::create_tup(params), ret_t)
						: type::create_fn(params[0], ret_t));

				auto body_t = check_expr(Body);
				m_Table.pop();

				// Check if body returned what return type needs
				unify(ret_t, body_t);
				return fn_ty;
			});},
			[&](ysptr<let_expr> le) -> type {
			return bind(le->as(), [&](pat_expr& Pattern, yopt<ty_expr>& Type, yopt<expr>& Value) {
				yopt<type> fin_sym = {};
				yopt<type> ty_sym = {};
				yopt<type> val_sym = {};
				if (Type) {
					ty_sym = check_type(*Type);
					if (Value) {
						Value->HintType = ty_sym;
						Value->HintPosition = Type->Position;
					}
					fin_sym = ty_sym;
				}
				if (Value) {
					val_sym = check_expr(*Value);
					fin_sym = val_sym;
				}
				if (ty_sym && val_sym) {
					unify(*ty_sym, *val_sym);
				}
				auto entries = match_pat(Pattern, fin_sym);
				for (auto e : entries) {
					m_Table.decl(
						std::make_shared<variable>(e.first,
							e.second ? *e.second : type::create_var()));
				}
				return symbol_table::UNIT_T;
			}); }
		);
	}

	type checker::check_type(ty_expr& ty) {
		return match(ty.Data) (
			[&](ysptr<unit_ty_expr>) -> type {
				return symbol_table::UNIT_T;
			},
			[&](ysptr<ident_ty_expr> ie) -> type {
			return bind(ie->as(), [&](ystr& Identifier) {
				// TODO: real type lookup
				if (Identifier == "i32") return symbol_table::I32_T;
				if (Identifier == "f32") return symbol_table::F32_T;
				return symbol_table::UNIT_T;
			}); },
			[&](ysptr<bin_ty_expr> be) -> type {
			return bind(be->as(), [&](token& Operator, ty_expr& LHS, ty_expr& RHS) {
				if (Operator.Type == ytoken_t::Arrow) {
					auto left = check_type(LHS);
					auto right = check_type(RHS);
					return type::create_fn(left, right);
				}
				throw std::exception("TODO: no such type operator");
			}); },
			[&](ysptr<list_ty_expr> le) -> type {
			return bind(le->as(), [&](yvec<ty_expr>& Elements) {
				yvec<type> syms;
				for (auto& el : Elements) {
					syms.push_back(check_type(el));
				}
				return type::create_tup(syms);
			}); }
		);
	}
	
	void checker::unify(type& t1, type& t2) {
		t1 = t1.prune();
		t2 = t2.prune();
		if (t1.same(t2)) {
			return;
		}

		match(t1.Data, t2.Data) (
			[&](ysptr<cons_type> tt1, ysptr<cons_type> tt2) {
			bind(tt1->as(), tt2->as(), [&](ystr& name1, yvec<type>& types1, ystr& name2, yvec<type>& types2) {
				if (name1 != name2) {
					// TODO
					throw std::exception(("TODO: " + t1.to_str() + " is not " + t2.to_str()).c_str());
				}
				if (types1.size() != types2.size()) {
					// TODO
					throw std::exception(("TODO: " + t1.to_str() + " has not as many types as " + t2.to_str()).c_str());
				}
				for (ysize i = 0; i < types1.size(); i++) {
					unify(types1[i], types2[i]);
				}
			}); },
			[&](ysptr<var_type> tt1, ysptr<var_type> tt2) {
			bind(tt2->as(), [&](ysize& id, yopt<type>& instance) {
				instance = t1;
			}); },
			[&](ysptr<cons_type> tt1, ysptr<var_type> tt2) {
			bind(tt2->as(), [&](ysize& id, yopt<type>& instance) {
				if (t1.contains(tt2)) {
					// TODO
					throw std::exception(("TODO: " + t1.to_str() + " contains " + t2.to_str() + " so it's recursive").c_str());
				}
				else {
					instance = t1;
				}
			}); },
			[&](ysptr<var_type> tt1, ysptr<cons_type> tt2) {
			bind(tt1->as(), [&](ysize& id, yopt<type>& instance) {
				if (t2.contains(tt1)) {
					// TODO
					throw std::exception(("TODO: " + t2.to_str() + " contains " + t1.to_str() + " so it's recursive").c_str());
				}
				else {
					instance = t2;
				}
			}); }
		);
	}

	bool* checker::get_braced(expr const& e) {
		return match(e.Data) (
			[&](ysptr<block_expr> be) -> bool* { 
			return bind(be->as(), [&](yvec<stmt>& Statements, bool& ReturnDest) {
				return &ReturnDest;
			}); },
			[&](auto&) -> bool* { return nullptr; }
		);
	}

	yvec<ypair<ystr, yopt<type>>> checker::match_pat(pat_expr const& pat, yopt<type> ty) {
		yvec<ypair<ystr, yopt<type>>> res;
		match_pat_impl(res, pat, ty);
		return res;
	}

	void checker::match_pat_impl(yvec<ypair<ystr, yopt<type>>>& res, pat_expr const& pat, yopt<type> ty) {
		match(pat.Data) (
			[&](ysptr<ignore_pat_expr>) {
				return;
			},
			[&](ysptr<unit_pat_expr> ) {
				if (!ty) {
					throw std::exception("TODO: Unmeaningful bind without type!");
				}
				if (ty->same(symbol_table::UNIT_T)) {
					throw std::exception("TODO: Pattern match failed for unit! (type)");
				}
			},
			[&](ysptr<ident_pat_expr> ip) {
			bind(ip->as(), [&](ystr& Identifier) {
				// Just bind
				res.push_back({ Identifier, ty });
			}); },
			[&](ysptr<list_pat_expr> le) {
			bind(le->as(), [&](yvec<pat_expr>& Elements) {
				if (!ty) {
					for (ysize i = 0; i < Elements.size(); i++) {
						match_pat_impl(res, Elements[i], yopt<type>{});
					}
					return;
				}
				match(ty->Data) (
					[&](ysptr<cons_type> tt) {
					bind(tt->as(), [&](ystr& Name, yvec<type>& Types) {
						if (Name != "@tup") {
							throw std::exception("TODO: Cannot match non-tuple with tuple");
						}
						if (Types.size() != Elements.size()) {
							throw std::exception("TODO: Pattern list size mismatch!");
						}

						for (ysize i = 0; i < Elements.size(); i++) {
							match_pat_impl(res, Elements[i], Types[i]);
						}
					}); },
					[&](auto&) {
						throw std::exception("TODO: Cannot match non-tuple with tuple");
					}
				);
			}); }
		);
	}
}
