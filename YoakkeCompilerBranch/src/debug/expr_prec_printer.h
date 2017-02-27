#pragma once

#include "../ast/expr.h"
#include "../utility/double_dispatcher.h"
#include "../common.h"

namespace yk {
	class expr_prec_printer : public double_dispatcher
		<ystr, expr, 
		ident_expr, bin_expr, preury_expr, postury_expr, list_expr, mixfix_expr> {
	public:
		ystr dispatch(ident_expr* ie) override;
		ystr dispatch(bin_expr* be) override;
		ystr dispatch(preury_expr* be) override;
		ystr dispatch(postury_expr* be) override;
		ystr dispatch(list_expr* be) override;
		ystr dispatch(mixfix_expr* be) override;
	};
}
