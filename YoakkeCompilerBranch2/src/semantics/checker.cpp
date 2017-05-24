#include "checker.h"
#include "../utility/match.h"
#include "../ast/ast.h"
#include "type.h"
#include "symbol.h"
#include "../reporting/err_stream.h"
#include "../reporting/msg.h"
#include "scope.h"
#include "unifier.h"

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
					DoesReturn		= true;
					auto ret_ty		= check_expr(*Sub);
					auto ret_dest	= m_Table.enclosing_return();
					if (!ret_dest) {
						throw std::exception("Sanity error: no enclosing return destination!");
					}
					if (auto o_ret_ty = ret_dest->ReturnType) {
						try {
							unifier::unify(ret_ty, *o_ret_ty);
						}
						catch (unifier::err& err) {
							rep::err_stream::report(
								rep::type_mismatch(
									m_File,
									ret_dest->ReturnPos,
									Sub->Position
								)
							);
						}
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
				if (auto sym = m_Table.ref(Identifier)) {
					return sym->get().Type;
				}
				else {
					rep::err_stream::report(
						rep::undefined_symbol(m_File, Identifier, ex.Position)
					);
					return symbol_table::UNIT_T;
				}
			}); },
			[&](ysptr<int_lit_expr>) -> type {
				return symbol_table::I32_T;
			},
			[&](ysptr<real_lit_expr>) -> type {
				return symbol_table::F32_T;
			},
			[&](ysptr<preury_expr> pe) -> type {
			return bind(pe->as(), [&](token& Operator, expr& Sub) {
				if (Operator.Type == ytoken_t::Typeof) {
					auto st = check_expr(Sub);
					std::cout << st.to_str() << std::endl;
				}
				return symbol_table::UNIT_T;
			}); },
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
				try {
					unifier::unify(left, right);
				}
				catch (unifier::err& err) {
					rep::err_stream::report(
						rep::type_mismatch(
							m_File,
							LHS.Position,
							RHS.Position
						)
					);
				}
				return symbol_table::UNIT_T;
			}); },
			[&](ysptr<const_asgn_expr> ce) -> type {
			return bind(ce->as(), [&](token& Op, expr& LHS, expr& RHS) {
				auto& left = std::get<0>(*std::get<ysptr<ident_expr>>(LHS.Data));
				auto right = check_expr(RHS);

				if (auto sym = m_Table.ref(left)) {
					match(sym->get().Data) (
						[&](ysptr<constant_symbol>) {
							auto& sym_t = sym->get().Type;
							match(sym_t.Data) (
								[&](ysptr<set_type> st) {
								bind(st->as(), [&](yvec<type>& types) {
									types.push_back(right);
								}); },
								[&](auto&) {
									sym_t = type::create_set(sym_t, right);
								}
							);
						},
						[&](auto&) {
							throw std::exception("Cannot classify type!");
						}
					);
				}
				else {
					m_Table.decl(symbol::create_constant(left, right));
				}
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
				auto my_t = type::create_fn(args_ty, type::create_var());;
				auto fn_ty = check_expr(Function);
				try {
					unifier::unify(my_t, fn_ty);
					return fn_ty;
				}
				catch (unifier::err& err) {
					throw std::exception("TODO: call no function :(");
				}
			}); },
			[&](ysptr<block_expr> be) -> type {
			return bind(be->as(), [&](yvec<stmt>& Statements, bool& ReturnDest, ysptr<scope>& Scope) {
				Scope = m_Table.push(ReturnDest);
				for (auto st : Statements) {
					check_stmt(st);
				}
				m_Table.pop();
				return Scope->ReturnType.value_or(symbol_table::UNIT_T);
			}); },
			[&](ysptr<fnproto_expr>) -> type {
				// TODO
				return symbol_table::UNIT_T;
			},
			[&](ysptr<fn_expr> fe) -> type {
			return bind(fe->as(), [&](yvec<param_t>& Parameters, yopt<ty_expr>& ReturnType, expr& Body) {
				auto& Bbody = *std::get<ysptr<block_expr>>(Body.Data);
				
				// TODO: warn if no parameter name
				// TODO: error if same parameter name
				auto sc = m_Table.push(true);

				yvec<type> params;
				for (auto par : Parameters) {
					auto par_ty = check_type(par.second);
					params.push_back(par_ty);
					if (auto name = par.first) {
						auto par_sym = symbol::create_variable(name.value().Value, par_ty);
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
				try {
					unifier::unify(ret_t, body_t);
				}
				catch (unifier::err& err) {
					auto& body_scope = std::get<2>(Bbody);
					// Report error based on provided annotations
					if (body_scope->ReturnType && ReturnType) {
						// We have annotations
						rep::err_stream::report(
							rep::type_mismatch(
								m_File,
								ReturnType->Position,
								body_scope->ReturnPos
							)
						);
					}
					else if (body_scope->ReturnType) {
						// No annotation but body returns
						rep::err_stream::report(
							rep::unannot_ret(m_File, body_scope->ReturnPos)
						);
					}
					else if (ReturnType) {
						// Annotation but no return
						rep::err_stream::report(
							rep::no_ret(m_File, interval(Body.Position.End, 1))
						);
					}
					else {
						throw std::exception("SANITY");
					}
				}
				return fn_ty;
			});},
			[&](ysptr<let_expr> le) -> type {
			return bind(le->as(), [&](pat_expr& Pattern, yopt<ty_expr>& Type, yopt<expr>& Value) {
				// TODO: shadowing does not work!!!
				yopt<type> fin_sym = {};
				yopt<type> ty_sym = {};
				yopt<type> val_sym = {};
				if (Type) {
					ty_sym = check_type(*Type);
					fin_sym = ty_sym;
					if (Value) {
						Value->HintType = ty_sym;
						Value->HintPosition = Type->Position;
					}
				}
				if (Value) {
					val_sym = check_expr(*Value);
					fin_sym = val_sym;
				}
				if (ty_sym && val_sym) {
					try {
						unifier::unify(*ty_sym, *val_sym);
					}
					catch (unifier::err& err) {
						rep::err_stream::report(
							rep::type_mismatch(
								m_File,
								Type->Position,
								Value->Position
							)
						);
					}
				}
				auto entries = match_pat(Pattern, fin_sym);
				for (auto e : entries) {
					m_Table.decl(
						symbol::create_variable(e.first,
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
				if (auto ty = m_Table.ref_t(Identifier)) {
					return *ty;
				}
				throw std::exception("TODO: undefined type!");
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

	bool* checker::get_braced(expr const& e) {
		return match(e.Data) (
			[&](ysptr<block_expr> be) -> bool* { 
			return bind(be->as(), [&](yvec<stmt>& Statements, bool& ReturnDest, ysptr<scope>& Scope) {
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
