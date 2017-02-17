#include "yexpr_parser.h"
#include "../ast/expr.h"

namespace yk {
	namespace expr_rules {
		typedef prec_parser<expr> expr_parser;
		typedef prefix_parselet<expr, expr_parser> expr_pre_parselet;

		class ident_parselet : public expr_pre_parselet {
		public:
			expr* parse(token const& begin, expr_parser* parser) override {
				return new ident_expr(begin);
			}
		};
	}

	yexpr_parser::yexpr_parser(token_buffer* buff)
		: prec_parser<expr>(buff) {

	}
}