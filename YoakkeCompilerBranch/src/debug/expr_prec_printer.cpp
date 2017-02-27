#include "expr_prec_printer.h"

namespace yk {
	ystr expr_prec_printer::dispatch(ident_expr* ie) {
		return ie->identifier;
	}

	ystr expr_prec_printer::dispatch(bin_expr* be) {
		return '(' + dispatch_gen(be->LHS) + ' ' + be->OP.identifier()
			+ ' ' + dispatch_gen(be->RHS) + ')';
	}

	ystr expr_prec_printer::dispatch(preury_expr* pe) {
		return '(' + pe->OP.value() + dispatch_gen(pe->Sub) + ')';
	}

	ystr expr_prec_printer::dispatch(postury_expr* pe) {
		return '(' + dispatch_gen(pe->Sub) + pe->OP.value() + ')';
	}

	ystr expr_prec_printer::dispatch(list_expr* ee) {
		ystr ret = dispatch_gen(ee->List[0]);
		for (ysize i = 1; i < ee->List.size(); i++) {
			ret += ", " + dispatch_gen(ee->List[i]);
		}
		return ret;
	}

	ystr expr_prec_printer::dispatch(mixfix_expr* me) {
		ystr ret = me->OP + '(';
		if (me->Operands.size()) {
			ret += dispatch_gen(me->Operands[0]);
			for (ysize i = 0; i < me->Operands.size(); i++) {
				ret += ", " + dispatch_gen(me->Operands[i]);
			}
		}
		ret += ')';
		return ret;
	}
}
