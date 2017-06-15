#include <iostream>
#include <cassert>
#include "ast_printer.h"

// TODO: commenting

std::ostream* ast_printer::Out = &std::cout;
ysize ast_printer::IndentWidth = 4;

ysize ast_printer::s_Indent = 0;

void ast_printer::print(const AST_expr* exp) {
	std::ostream& outs = *Out;

	switch (exp->Type) {
	case expr_t::BinOp: {
		const AST_bin_expr* bexp = static_cast<const AST_bin_expr*>(exp);
		tag_begin("bin_op" + bexp->OP.Value);
		print(bexp->LHS);
		print(bexp->RHS);
		tag_end("bin_op");
	}
	break;

	case expr_t::Ident: {
		const AST_ident_expr* iexp = static_cast<const AST_ident_expr*>(exp);
		tag_begin("ident");
		write_indent();
		outs << iexp->ID << std::endl;
		tag_end("ident");
	}
	break;

	case expr_t::IntLit: {
		const AST_int_lit_expr* iexp = static_cast<const AST_int_lit_expr*>(exp);
		tag_begin("int_lit");
		write_indent();
		outs << iexp->Value << std::endl;
		tag_end("int_lit");
	}
	break;

	case expr_t::PostOp: {
		const AST_post_expr* pexp = static_cast<const AST_post_expr*>(exp);
		tag_begin("post_op" + pexp->OP.Value);
		print(pexp->Sub);
		tag_end("post_op");
	}
	break;

	case expr_t::PreOp: {
		const AST_pre_expr* pexp = static_cast<const AST_pre_expr*>(exp);
		tag_begin("pre_op" + pexp->OP.Value);
		print(pexp->Sub);
		tag_end("pre_op");
	}
	break;

	default:
		assert(false && "Unhandled expression for printer!");
	}
}

void ast_printer::write_indent() {
	std::ostream& outs = *Out;

	outs << ystr(s_Indent, ' ');
}

void ast_printer::indent_in() {
	s_Indent += IndentWidth;
}

void ast_printer::indent_out() {
	assert(IndentWidth > 0 && "Cannot indent out from no indent");
	s_Indent -= IndentWidth;
}

void ast_printer::tag_begin(ystr const& name) {
	std::ostream& outs = *Out;

	write_indent();
	outs << '<' << name << '>' << std::endl;
	indent_in();
}

void ast_printer::tag_end(ystr const& name) {
	std::ostream& outs = *Out;

	indent_out();
	write_indent();
	outs << "</" << name << '>' << std::endl;
}
