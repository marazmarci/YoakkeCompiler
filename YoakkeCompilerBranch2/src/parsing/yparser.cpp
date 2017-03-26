#include "yparser.h"
#include "parse_error.h"
#include "../ast/expr.h"

#define throw_expect(x)					\
token const& m_tok_ = parser.peek();	\
throw expect_parse_err(x,				\
	yparser::format_token(m_tok_),		\
	parser.file(), m_tok_.Position)

namespace yk {
	namespace expr_rules {
		using expr_pre_parselet	= prefix_parselet<expr, yparser>;
		using expr_in_parselet	= infix_parselet<expr, yparser>;

		template <typename Return_T>
		class pass : public expr_pre_parselet {
		public:
			yshared_ptr<expr> parse(token const& begin, yparser& parser) override {
				return std::make_shared<Return_T>(begin);
			}
		};

		using ident =		pass<ident_expr>;
		using int_lit =		pass<int_lit_expr>;
		using real_lit =	pass<real_lit_expr>;

		class enclose : public expr_pre_parselet {
		public:
			yshared_ptr<expr> parse(token const& begin, yparser& parser) override {
				if (auto sub = parser.parse_expr()) {
					if (parser.match(ytoken_t::Rpar)) {
						return sub;
					}
					else {
						token const& tok = parser.peek();
						throw_expect("')'");
					}
				}
				else {
					token const& tok = parser.peek();
					throw_expect("expression");
				}
			}
		};

		class preury : public expr_pre_parselet {
		private:
			ysize m_Precedence;

		public:
			preury(ysize prec)
				: m_Precedence(prec) {
			}

		public:
			yshared_ptr<expr> parse(token const& begin, yparser& parser) override {
				if (auto right = parser.parse_expr(m_Precedence)) {
					return std::make_shared<preury_expr>(begin, right);
				}
				else {
					throw_expect("right-hand side operand");
				}
			}
		};

		class postury : public expr_in_parselet {
		public:
			postury(ysize prec)
				: expr_in_parselet(prec) {
			}

		public:
			yshared_ptr<expr> parse(
				yshared_ptr<expr> left, token const& begin, yparser& parser) override {
				return std::make_shared<postury_expr>(begin, left);
			}
		};

		class list : public expr_in_parselet {
		public:
			list(ysize prec)
				: expr_in_parselet(prec) {
			}

		public:
			yshared_ptr<expr> parse(
				yshared_ptr<expr> left, token const& begin, yparser& parser) override {
				auto ls = std::make_shared<list_expr>(left);
				do {
					if (auto rhs = parser.parse_expr(precedence() - 1)) {
						ls->add(rhs);
					}
					else {
						throw_expect("expression");
					}
				} while (parser.match(ytoken_t::Comma));
				return ls;
			}
		};

		class call : public expr_in_parselet {
		public:
			call(ysize prec)
				: expr_in_parselet(prec) {
			}

		public:
			yshared_ptr<expr> parse(
				yshared_ptr<expr> left, token const& begin, yparser& parser) override {
				auto args = parser.parse_expr();
				if (auto tok = parser.match(ytoken_t::Rpar)) {
					return std::make_shared<call_expr>(left, args, tok.value());
				}
				else {
					throw_expect("')'");
				}
			}
		};

		template <bool Right, typename Return_T>
		class bin : public expr_in_parselet {
		public:
			bin(ysize prec)
				: expr_in_parselet(prec) {
			}

		public:
			yshared_ptr<expr> parse(
				yshared_ptr<expr> left, token const& begin, yparser& parser) override {
				if (auto rhs = parser.parse_expr(precedence() - (Right ? 1 : 0))) {
					return std::make_shared<Return_T>(begin, left, rhs);
				}
				else {
					throw_expect("expression");
				}
			}
		};

		class const_asgn : public bin<true, const_asgn_expr> {
		public:
			const_asgn(ysize prec)
				: bin(prec) {
			}

		public:
			bool matches(yshared_ptr<expr> left, yparser& parser) override { 
				return std::dynamic_pointer_cast<ident_expr>(left) != nullptr; 
			}
		};

		using lbinop		= bin<false, binop_expr>;
		using rbinop		= bin<true,	binop_expr>;
		using asgn			= bin<true, asgn_expr>;
	}

	yparser::yparser(ystr const& file)
		: parser(m_Lexer, m_Buffer), m_File(file), m_Lexer(file), 
		m_ExprParser(m_Lexer, m_Buffer) {
		// Literals
		register_expr<expr_rules::ident>	(ytoken_t::Ident);
		register_expr<expr_rules::int_lit>	(ytoken_t::Integer);
		register_expr<expr_rules::real_lit>	(ytoken_t::Real);

		// Enclosing
		register_expr<expr_rules::enclose>	(ytoken_t::Lpar);

		// Operators
		register_expr<expr_rules::const_asgn>	(ytoken_t::Dcolon,	1);
		register_expr<expr_rules::list>			(ytoken_t::Comma,	2);
		register_expr<expr_rules::asgn>			(ytoken_t::Asgn,	3);
		register_expr<expr_rules::lbinop>		(ytoken_t::Or,		4);
		register_expr<expr_rules::lbinop>		(ytoken_t::And,		5);

		register_expr<expr_rules::lbinop>		(ytoken_t::Eq,		6);
		register_expr<expr_rules::lbinop>		(ytoken_t::Neq,		6);

		register_expr<expr_rules::lbinop>		(ytoken_t::Gr,		7);
		register_expr<expr_rules::lbinop>		(ytoken_t::Le,		7);
		register_expr<expr_rules::lbinop>		(ytoken_t::Goe,		7);
		register_expr<expr_rules::lbinop>		(ytoken_t::Loe,		7);

		register_expr<expr_rules::lbinop>		(ytoken_t::Add,		8);
		register_expr<expr_rules::lbinop>		(ytoken_t::Sub,		8);

		register_expr<expr_rules::lbinop>		(ytoken_t::Mul,		9);
		register_expr<expr_rules::lbinop>		(ytoken_t::Div,		9);
		register_expr<expr_rules::lbinop>		(ytoken_t::Mod,		9);

		register_expr<expr_rules::preury>		(ytoken_t::Add,		10);
		register_expr<expr_rules::preury>		(ytoken_t::Sub,		10);
		register_expr<expr_rules::preury>		(ytoken_t::Not,		10);
		
		register_expr<expr_rules::lbinop>		(ytoken_t::Dot,		11);
		register_expr<expr_rules::call>			(ytoken_t::Lpar,	11);
	}

	yshared_ptr<expr> yparser::parse_expr(ysize prec) {
		return m_ExprParser.parse(*this, prec);
	}

	ystr const& yparser::file() const {
		return m_File;
	}

	ystr yparser::format_token(token const& t) {
		switch (ytoken_t(t.Type)) {
		case ytoken_t::Ident:
			return "identifier";

		case ytoken_t::Integer:
			return "integer literal";

		case ytoken_t::Real:
			return "real literal";

		case ytoken_t::Epsilon:
			return "end of file";

		default:
			return "'" + t.Value + "'";
		}
	}
}
