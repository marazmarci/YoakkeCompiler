#include "ast_printer.h"
#include "../str_utils.h"
#include "../syntax/ast_stmt.h"

std::ostream* AST_printer::Out = &std::cout;

void AST_printer::print(const AST_expr* exp, ysize indent) {
	// Reference the outstream for simpler syntax
	std::ostream& outs = *Out;
	
	// Indent accordingly to the current depth
	print_indent(indent);

	switch (exp->Type) {
	case AST_expr_t::BinOp: {
		const AST_bin_expr* n = static_cast<const AST_bin_expr*>(exp);
		outs << "bin" << n->OP.Value << std::endl;
		print(n->LHS, indent + 1);
		print(n->RHS, indent + 1);
		break;
	}

	case AST_expr_t::Ident: {
		const AST_ident_expr* n = static_cast<const AST_ident_expr*>(exp);
		outs << '\'' << n->ID << '\'' << std::endl;
		break;
	}

	case AST_expr_t::IntLit: {
		const AST_int_lit_expr* n = static_cast<const AST_int_lit_expr*>(exp);
		outs << n->Value << std::endl;
		break;
	}

	case AST_expr_t::PostOp: {
		const AST_post_expr* n = static_cast<const AST_post_expr*>(exp);
		outs << "post" << n->OP.Value << std::endl;
		print(n->Sub, indent + 1);
		break;
	}

	case AST_expr_t::PreOp: {
		const AST_pre_expr* n = static_cast<const AST_pre_expr*>(exp);
		outs << "pre" << n->OP.Value << std::endl;
		print(n->Sub, indent + 1);
		break;
	}

	case AST_expr_t::Func: {
		const AST_func_expr* n = static_cast<const AST_func_expr*>(exp);
		outs << "fn";
		if (n->Label) {
			outs << '[' << n->Label->Value << ']';
		}
		outs << std::endl;
		for (auto st : n->Body->Statements) {
			print(st, indent + 1);
		}
		if (n->Body->Return) {
			print(n->Body->Return, indent + 1);
		}
		break;
	}

	case AST_expr_t::Call: {
		const AST_call_expr* n = static_cast<const AST_call_expr*>(exp);
		outs << "call" << std::endl;
		print(n->Func, indent + 1);
		for (auto a : n->Args) {
			print(a, indent + 1);
		}
		break;
	}

	case AST_expr_t::If: {
		const AST_if_expr* n = static_cast<const AST_if_expr*>(exp);
		outs << "if";
		if (n->Label) {
			outs << '[' << n->Label->Value << ']';
		}
		outs << std::endl;
		print(n->Condition, indent + 1);
		for (auto st : n->Then->Statements) {
			print(st, indent + 1);
		}
		if (n->Then->Return) {
			print(n->Then->Return, indent + 1);
		}
		if (n->Else) {
			print_indent(indent);
			outs << "else" << std::endl;
			for (auto el : n->Else->Statements) {
				print(el, indent + 1);
			}
			if (n->Else->Return) {
				print(n->Else->Return, indent + 1);
			}
		}
		break;
	}

	default:
		assert(false && "Unhandled case in printer()!");
	}
}

void AST_printer::print(const AST_stmt* stmt, ysize indent) {
	// Reference the outstream for simpler syntax
	std::ostream& outs = *Out;

	switch (stmt->Type) {
	case AST_stmt_t::Expr: {
		const AST_expr_stmt* n = static_cast<const AST_expr_stmt*>(stmt);
		print(n->Sub, indent);
		break;
	}

	default:
		assert(false && "Unhandled case in printer()!");
	}
}

void AST_printer::print_indent(ysize indent) {
	// Reference the outstream for simpler syntax
	std::ostream& outs = *Out;

	// Indent accordingly to the current depth
	for (ysize i = 0; i < indent; i++) {
		outs << "  |";
	}
	outs << "--";
}
