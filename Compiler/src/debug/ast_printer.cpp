#include "ast_printer.h"
#include "../str_utils.h"

std::ostream* AST_printer::Out = &std::cout;

void AST_printer::print(const AST_expr* exp, ysize indent) {
	// Reference the outstream for simpler syntax
	std::ostream& outs = *Out;
	
	// Indent accordingly to the current depth
	for (ysize i = 0; i < indent; i++) {
		outs << "  |";
	}
	outs << "--";

	switch (exp->Type) {
	case expr_t::BinOp: {
		const AST_bin_expr* n = static_cast<const AST_bin_expr*>(exp);
		outs << "bin" << n->OP.Value << std::endl;
		print(n->LHS, indent + 1);
		print(n->RHS, indent + 1);
		break;
	}

	case expr_t::Ident: {
		const AST_ident_expr* n = static_cast<const AST_ident_expr*>(exp);
		outs << '\'' << n->ID << '\'' << std::endl;
		break;
	}

	case expr_t::IntLit: {
		const AST_int_lit_expr* n = static_cast<const AST_int_lit_expr*>(exp);
		outs << n->Value << std::endl;
		break;
	}

	case expr_t::PostOp: {
		const AST_post_expr* n = static_cast<const AST_post_expr*>(exp);
		outs << "post" << n->OP.Value << std::endl;
		print(n->Sub, indent + 1);
		break;
	}

	case expr_t::PreOp: {
		const AST_pre_expr* n = static_cast<const AST_pre_expr*>(exp);
		outs << "pre" << n->OP.Value << std::endl;
		print(n->Sub, indent + 1);
		break;
	}

	default:
		assert(false && "Unhandled case in printer()!");
	}
}

