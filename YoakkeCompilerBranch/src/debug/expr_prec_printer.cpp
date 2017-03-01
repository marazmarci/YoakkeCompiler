#include "expr_prec_printer.h"

namespace yk {
	expr_prec_printer::expr_prec_printer() {
		META_Visitables(expr, 
			ident_expr, bin_expr, preury_expr, postury_expr, list_expr, mixfix_expr);
	}

	ystr expr_prec_printer::print(expr& n) {
		throw std::exception("Expression not handled!");
	}

	ystr expr_prec_printer::print(ident_expr& ie) {
		return ie.Identifier;
	}

	ystr expr_prec_printer::print(bin_expr& be) {
		return '(' + (*this)(*be.LHS) + ' ' + be.OP.Identifier()
			+ ' ' + (*this)(*be.RHS) + ')';
	}

	ystr expr_prec_printer::print(preury_expr& pe) {
		return '(' + pe.OP.value() + print(*pe.Sub) + ')';
	}

	ystr expr_prec_printer::print(postury_expr& pe) {
		return '(' + (*this)(*pe.Sub) + pe.OP.value() + ')';
	}

	ystr expr_prec_printer::print(list_expr& ee) {
		ystr ret = (*this)(*ee.List[0]);
		for (ysize i = 1; i < ee.List.size(); i++) {
			ret += ", " + (*this)(*ee.List[i]);
		}
		return ret;
	}

	ystr expr_prec_printer::print(mixfix_expr& me) {
		ystr ret = me.OP + '(';
		if (me.Operands.size()) {
			ret += (*this)(*me.Operands[0]);
			for (ysize i = 0; i < me.Operands.size(); i++) {
				ret += ", " + (*this)(*me.Operands[i]);
			}
		}
		ret += ')';
		return ret;
	}
}
