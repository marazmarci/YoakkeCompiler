#include "semantic_checker.h"
#include "../ast/mch_bindings.h"

namespace yk {
	void semantic_checker::check_stmt(yshared_ptr<stmt> st) {
		mch::var<yshared_ptr<expr>> expression;
		mch::var<bool> semicol;

		Match(*st.get())
		{
			Case(expr_stmt, expression, semicol) {
				check_expr(expression);
				return;
			}
			Otherwise() {
				throw std::exception("Unhandled visit for semantic check (statement)!");
			}
		}
		EndMatch
	}

	void semantic_checker::check_expr(yshared_ptr<expr> ex) {

	}

	void semantic_checker::check_type(yshared_ptr<type_desc> ty) {

	}
}
