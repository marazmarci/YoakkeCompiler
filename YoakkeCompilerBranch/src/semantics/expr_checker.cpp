#include "expr_checker.h"

namespace yk {
	expr_checker::expr_checker(semantic_checker* ch)
		: m_Checker(ch) {
	}

	void expr_checker::dispatch(ident_expr* exp) {

	}

	void expr_checker::dispatch(unit_expr* exp) {

	}

	void expr_checker::dispatch(bin_expr* exp) {

	}

	void expr_checker::dispatch(preury_expr* exp) {

	}

	void expr_checker::dispatch(postury_expr* exp) {

	}

	void expr_checker::dispatch(enclose_expr* exp) {

	}

	void expr_checker::dispatch(mixfix_expr* exp) {

	}

	void expr_checker::dispatch(func_proto* exp) {

	}

	void expr_checker::dispatch(func_expr* exp) {

	}

	void expr_checker::dispatch(body_expr* exp) {

	}

	void expr_checker::dispatch(param_expr* exp) {

	}

	void expr_checker::dispatch(let_expr* exp) {

	}
}