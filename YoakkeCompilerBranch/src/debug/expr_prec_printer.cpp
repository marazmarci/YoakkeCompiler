#include "expr_prec_printer.h"

namespace yk {
	ystr expr_prec_printer::dispatch(ident_expr* ie) {
		return ie->identifier;
	}

	ystr expr_prec_printer::dispatch(bin_expr* be) {
		return '(' + dispatch_gen(be->LHS) + ' ' + be->OP.identifier()
			+ ' ' + dispatch_gen(be->RHS) + ')';
	}
}