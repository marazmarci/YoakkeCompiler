#include <cassert>
#include <iostream>
#include "../syntax/ast_expr.h"
#include "../syntax/ast_stmt.h"
#include "../syntax/ast_ty.h"
#include "../syntax/ast_pat.h"

namespace AST_printer {
	namespace {
		void print(AST_stmt* stmt, ysize indent);
		void print(AST_expr* expr, ysize indent);
		void print(AST_ty* ty, ysize indent);
		void print(AST_pat* par, ysize indent);

		ysize IndentSize = 2;

		void do_indent(ysize amount) {
			std::cout << ystr(amount * IndentSize, ' ');
		}

		template <typename Fn>
		auto block(ysize indent, ystr const& title, Fn fn) {
			do_indent(indent);
			std::cout << title << " {" << std::endl;
			fn();
			do_indent(indent);
			std::cout << '}' << std::endl;
		}

		void print(AST_stmt* stmt, ysize indent) {
			switch (stmt->Ty) {
			case AST_stmt_t::FnDecl: {
				auto st = (AST_fn_decl_stmt*)stmt;
				block(indent, "Fn_Decl: " + st->Name.Value, [&]() {
					print(st->Expression, indent + 1);
				});
			}
			break;

			case AST_stmt_t::ConstDecl: {
				auto st = (AST_const_decl_stmt*)stmt;
				block(indent, "Const_Decl: " + st->Name.Value, [&]() {
					print(st->Expression, indent + 1);
				});
			}
			break;

			case AST_stmt_t::TyDecl: {
				auto st = (AST_ty_decl_stmt*)stmt;
				block(indent, "Type_Decl: " + st->Name.Value, [&]() {
					print(st->Type, indent + 1);
				});
			}
			break;

			case AST_stmt_t::Expr: {
				auto st = (AST_expr_stmt*)stmt;
				print(st->Expression, indent);
			}
			break;

			case AST_stmt_t::DbgWriteTy: {
			}
			break;

			default:
				UNIMPLEMENTED;
			}
		}

		void print(AST_expr* expr, ysize indent) {
			switch (expr->Ty) {
			case AST_expr_t::Block: {
				auto ex = (AST_block_expr*)expr;
				block(indent, "Block", [&]() {
					for (auto& s : ex->Statements) {
						print(s, indent + 1);
					}
					if (ex->Value) {
						block(indent + 1, "Return", [&]() {
							print(*ex->Value, indent + 2);
						});
					}
				});
			}
			break;

			case AST_expr_t::Fn: {
				auto ex = (AST_fn_expr*)expr;
				block(indent, "Fn", [&]() {
					if (ex->Return) {
						block(indent + 1, "Return Type", [&]() {
							print(*ex->Return, indent + 2);
						});
					}
					if (ex->Params.size()) {
						block(indent + 1, "Parameters", [&]() {
							for (auto& par : ex->Params) {
								auto& name = std::get<0>(par);
								auto& ty = std::get<1>(par);
								
								block(indent + 2, "Param", [&]() {
									if (name) {
										do_indent(indent + 3);
										std::cout << "name: " << name->Value << std::endl;
									}
									block(indent + 3, "Type", [&]() {
										print(ty, indent + 4);
									});
								});
							}
						});
					}
					block(indent + 1, "Body", [&]() {
						print(ex->Body, indent + 2);
					});
				});
			}
			break;

			case AST_expr_t::Ident: {
				auto ex = (AST_ident_expr*)expr;
				do_indent(indent);
				std::cout << "id: " << ex->Value << std::endl;
			}
			break;
			
			case AST_expr_t::If: {
				auto ex = (AST_if_expr*)expr;
				block(indent, "If", [&]() {
					block(indent + 1, "Condition", [&]() {
						print(ex->Condition, indent + 2);
					});
					block(indent + 1, "Then", [&]() {
						print(ex->Then, indent + 2);
					});
					if (ex->Else) {
						block(indent + 1, "Else", [&]() {
							print(*ex->Else, indent + 2);
						});
					}
				});
			}
			break;

			case AST_expr_t::Let: {
				auto ex = (AST_let_expr*)expr;
				block(indent, "Let", [&]() {
					block(indent + 1, "Pattern", [&]() {
						print(ex->Pattern, indent + 2);
					});
					if (ex->Type) {
						block(indent + 1, "Type", [&]() {
							print(*ex->Type, indent + 2);
						});
					}
					if (ex->Value) {
						block(indent + 1, "Value", [&]() {
							print(*ex->Value, indent + 2);
						});
					}
				});
			}
			break;

			case AST_expr_t::Call: {
				auto ex = (AST_call_expr*)expr;
				block(indent, "Call", [&]() {
					block(indent + 1, "Function", [&]() {
						print(ex->Func, indent + 2);
					});
					if (ex->Params.size()) {
						block(indent + 1, "Parameters", [&]() {
							for (auto& par : ex->Params) {
								block(indent + 2, "Param", [&]() {
									print(par, indent + 3);
								});
							}
						});
					}
				});
			}
			break;

			case AST_expr_t::Pre: {
				auto ex = (AST_pre_expr*)expr;
				block(indent, "Pre: " + ex->Oper.Value, [&]() {
					print(ex->Subexpr, indent + 1);
				});
			}
			break;

			case AST_expr_t::Post: {
				auto ex = (AST_post_expr*)expr;
				block(indent, "Post: " + ex->Oper.Value, [&]() {
					print(ex->Subexpr, indent + 1);
				});
			}
			break;

			case AST_expr_t::Bin: {
				auto ex = (AST_bin_expr*)expr;
				block(indent, "Bin: " + ex->Oper.Value, [&]() {
					block(indent + 1, "Left", [&]() {
						print(ex->Left, indent + 2);
					});
					block(indent + 1, "Right", [&]() {
						print(ex->Right, indent + 2);
					});
				});
			}
			break;

			case AST_expr_t::List: {
				auto ex = (AST_list_expr*)expr;
				block(indent, "List", [&]() {
					for (auto& el : ex->Elements) {
						block(indent + 1, "Element", [&]() {
							print(el, indent + 2);
						});
					}
				});
			}
			break;

			case AST_expr_t::IntLit: {
				auto ex = (AST_int_lit_expr*)expr;
				do_indent(indent);
				std::cout << "integer: " << ex->Value << std::endl;
			}
			break;

			default:
				UNIMPLEMENTED;
			}
		}

		void print(AST_ty* ty, ysize indent) {
			switch (ty->Ty) {
			case AST_ty_t::Ident: {
				auto t = (AST_ident_ty*)ty;
				do_indent(indent);
				std::cout << "id: " << t->Value << std::endl;
			}
			break;

			case AST_ty_t::Pre: {
				auto t = (AST_pre_ty*)ty;
				block(indent, "Pre: " + t->Oper.Value, [&]() {
					print(t->Subexpr, indent + 1);
				});
			}
			break;

			case AST_ty_t::Post: {
				auto t = (AST_post_ty*)ty;
				block(indent, "Post: " + t->Oper.Value, [&]() {
					print(t->Subexpr, indent + 1);
				});
			}
			break;

			case AST_ty_t::Bin: {
				auto t = (AST_bin_ty*)ty;
				block(indent, "Bin: " + t->Oper.Value, [&]() {
					block(indent + 1, "Left", [&]() {
						print(t->Left, indent + 2);
					});
					block(indent + 1, "Right", [&]() {
						print(t->Right, indent + 2);
					});
				});
			}
			break;

			case AST_ty_t::List: {
				auto t = (AST_list_ty*)ty;
				block(indent, "List", [&]() {
					for (auto& el : t->Elements) {
						block(indent + 1, "Element", [&]() {
							print(el, indent + 2);
						});
					}
				});
			}
			break;

			default:
				UNIMPLEMENTED;
			}
		}

		void print(AST_pat* pat, ysize indent) {
			switch (pat->Ty) {
			case AST_pat_t::Ident: {
				auto pt = (AST_ident_pat*)pat;
				do_indent(indent);
				std::cout << "id: " << pt->Value << std::endl;
			}
			break;

			case AST_pat_t::Pre: {
				auto pt = (AST_pre_pat*)pat;
				block(indent, "Pre: " + pt->Oper.Value, [&]() {
					print(pt->Subexpr, indent + 1);
				});
			}
			break;

			case AST_pat_t::Post: {
				auto pt = (AST_post_pat*)pat;
				block(indent, "Post: " + pt->Oper.Value, [&]() {
					print(pt->Subexpr, indent + 1);
				});
			}
			break;

			case AST_pat_t::Bin: {
				auto pt = (AST_bin_pat*)pat;
				block(indent, "Bin: " + pt->Oper.Value, [&]() {
					block(indent + 1, "Left", [&]() {
						print(pt->Left, indent + 2);
					});
					block(indent + 1, "Right", [&]() {
						print(pt->Right, indent + 2);
					});
				});
			}
			break;

			case AST_pat_t::List: {
				auto pt = (AST_list_pat*)pat;
				block(indent, "List", [&]() {
					for (auto& el : pt->Elements) {
						block(indent + 1, "Element", [&]() {
							print(el, indent + 2);
						});
					}
				});
			}
			break;

			default:
				UNIMPLEMENTED;
			}
		}
	}

	void print(AST_stmt* stmt) {
		print(stmt, 0);
	}

	void print(AST_expr* expr) {
		print(expr, 0);
	}

	void print(AST_ty* ty) {
		print(ty, 0);
	}

	void print(AST_pat* pat) {
		print(pat, 0);
	}
}
