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
			case AST_stmt_t::Decl: {
				auto st = (AST_decl_stmt*)stmt;
				block(indent, "Decl: " + st->Name.Value, [&]() {
					print(st->Expression, indent + 1);
				});
			}
			break;

			case AST_stmt_t::Expr: {
				auto st = (AST_expr_stmt*)stmt;
				print(st->Expression, indent);
			}
			break;

			default:
				assert(false && "Statement print unimplemented!");
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

			default:
				assert(false && "Expression print unimplemented!");
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

			default:
				assert(false && "Type print unimplemented!");
			}
		}

		void print(AST_pat* pat, ysize indent) {
			switch (pat->Ty) {
			case AST_pat_t::Ident: {
				auto pt = (AST_ident_ty*)pat;
				do_indent(indent);
				std::cout << "id: " << pt->Value << std::endl;
			}
			break;

			default:
				assert(false && "Pattern print unimplemented!");
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
