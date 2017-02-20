#pragma once

#include "../ast/expr.h"
#include "../utility/double_dispatcher.h"
#include "../common.h"

namespace yk {
	class expr_prec_printer : public double_dispatcher
		<ystr, ident_expr, bin_expr> {
	public:
		ystr dispatch(ident_expr* ie) override;
		ystr dispatch(bin_expr* be) override;
	};
}
