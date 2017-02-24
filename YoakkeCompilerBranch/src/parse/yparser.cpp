#include "yparser.h"
#include "../ast/expr.h"
#include "../utility/map_ext.h"

namespace yk {
	namespace expr_rules {
		typedef prec_parser<expr> expr_parser;
		typedef prefix_parselet<expr, expr_parser> expr_pre_parselet;
		typedef infix_parselet<expr, expr_parser> expr_in_parselet;

		class ident : public expr_pre_parselet {
		public:
			expr* parse(token const& begin, expr_parser* parser) override {
				return new ident_expr(begin);
			}
		};

		class binop : public expr_in_parselet {
		public:
			const bool right;

		public:
			binop(ysize prec, bool r)
				: expr_in_parselet(prec), right(r) {
			}

		public:
			expr* parse(expr* left, token const& begin, expr_parser* parser) override {
				auto rhs = parser->parse(precedence() - (right ? 1 : 0));
				if (rhs) {
					return new bin_expr(left, rhs, begin);
				}
				else {
					throw std::exception("RHS expected for operator!");
				}
			}
		};

		class pre_uryop : public expr_pre_parselet {
		public:
			ysize precedence;

		public:
			pre_uryop(ysize prec)
				: precedence(prec) {
			}

		public:
			expr* parse(token const& begin, expr_parser* parser) override {
				auto right = parser->parse(precedence);
				if (right) {
					return new preury_expr(right, begin);
				}
				else {
					throw std::exception("RHS expected!");
				}
			}
		};

		class post_uryop : public expr_in_parselet {
		public:
			post_uryop(ysize prec)
				: expr_in_parselet(prec) {
			}

		public:
			expr* parse(expr* left, token const& begin, expr_parser* parser) override {
				return new postury_expr(left, begin);
			}
		};

		class enclosed : public expr_pre_parselet {
		public:
			expr* parse(token const& begin, expr_parser* parser) override {
				yparser* ypar = reinterpret_cast<yparser*>(parser->parent());
				yvec<param_expr*> params;
				type_desc* rett = nullptr;
				expr* body = nullptr;
				yopt<token> end = None;
				if (parser->peek().identifier() != ")") {
					if (auto exp = parser->parse()) {
						end = parser->match_id(")");
						if (end.some()) {
							return new enclose_expr(exp, begin, end.get());
						}
						else {
							throw std::exception("')' expected!");
						}
					}
					else if (auto par = ypar->parse_param()) {
						params.push_back(par);
						while (ypar->peek().identifier() == ",") {
							ypar->consume();
							if (par = ypar->parse_param()) {
								params.push_back(par);
							}
							else {
								throw std::exception("Parameter expected after ','!");
							}
						}
						end = parser->match_id(")");
						if (end.none()) {
							throw std::exception("')' expected!");
						}
					}
					else {
						throw std::exception("')' or expression expected after '('!");
					}
				}
				else {
					end = parser->consume();
				}
				if (parser->peek().identifier() == "->") {
					parser->consume();
					if (!(rett = ypar->parse_type())) {
						throw std::exception("Type expected after '->'!");
					}
				}
				body = ypar->parse_body();
				if (params.size() || rett || body) {
					func_proto* proto = nullptr;
					if (rett) {
						proto = new func_proto(begin, rett->Position, params, rett);
					}
					else {
						proto = new func_proto(begin, position::get(end.get()), params, 
							new ident_type_desc(token("unit", "unit")));
					}
					if (body) {
						return new func_expr(proto, body);
					}
					else {
						return proto;
					}
				}
				else {
					return new unit_expr(begin, end.get());
				}
			}
		};

		class func_call : public expr_in_parselet {
		public:
			func_call(ysize prec)
				: expr_in_parselet(prec) {
			}

		public:
			bool matches(expr* exp) override {
				return dynamic_cast<block_expr*>(exp) == nullptr;
			}

			expr* parse(expr* left, token const& begin, expr_parser* parser) override {
				yvec<expr*> operands;
				operands.push_back(left);
				auto last = parser->peek();
				if (last.identifier() != ")") {
					auto params = parser->parse();
					if (!params) {
						throw std::exception("Parameters expected!");
					}
					operands.push_back(params);
					last = parser->peek();
					if (last.identifier() != ")") {
						throw std::exception("')' expected!");
					}
				}
				parser->consume();
				return new mixfix_expr("call", operands,
					position::interval(left->Position, position::get(last)));
			}
		};

		class const_asgn : public binop {
		public:
			const_asgn(ysize prec)
				: binop(prec, true) {
			}

		public:
			bool matches(expr* left) override {
				return dynamic_cast<ident_expr*>(left) != nullptr;
			}
		};
	}

	namespace type_rules {
		typedef prec_parser<type_desc> type_parser;
		typedef prefix_parselet<type_desc, type_parser> type_pre_parselet;
		typedef infix_parselet<type_desc, type_parser> type_in_parselet;

		class ident : public type_pre_parselet {
		public:
			type_desc* parse(token const& begin, type_parser* parser) override {
				return new ident_type_desc(begin);
			}
		};

		class binop : public type_in_parselet {
		public:
			const bool right;

		protected:
			binop(ysize prec, bool r)
				: type_in_parselet(prec), right(r) {
			}

		public:
			type_desc* parse(type_desc* left, token const& begin, type_parser* parser) override {
				auto rhs = parser->parse(precedence() - (right ? 1 : 0));
				if (rhs) {
					return create(left, rhs, begin);
				}
				else {
					throw std::exception("RHS expected for operator!");
				}
			}

		protected:
			virtual type_desc* create(type_desc* left, type_desc* right, token const& op) = 0;
		};
	}

	yparser::yparser(token_buffer* buff)
		: parser(buff), m_ExprParser(buff, this), m_TypeParser(buff, this) {
		// Atomic
		m_ExprParser.register_rule("Identifier", new expr_rules::ident());
		m_ExprParser.register_rule("(", new expr_rules::enclosed());

		m_ExprParser.register_rule("::", new expr_rules::const_asgn(1));

		// Operators
		infixl_expr(",", 2);
		
		infixr_expr("=", 3);
		
		infixl_expr("||", 4);

		infixl_expr("&&", 5);

		infixl_expr("==", 6);
		infixl_expr("<>", 6);

		infixl_expr("<", 7);
		infixl_expr(">", 7);
		infixl_expr("<=", 7);
		infixl_expr(">=", 7);

		infixl_expr("+", 8);
		infixl_expr("-", 8);

		infixl_expr("*", 9);
		infixl_expr("/", 9);
		infixl_expr("%", 9);

		prefix_expr("+", 10);
		prefix_expr("-", 10);
		prefix_expr("!", 10);

		infixl_expr(".", 11);
		m_ExprParser.register_rule("(", new expr_rules::func_call(11));

		// TYPE DESCRIPTOR //////////////////////////////////////////////
		m_TypeParser.register_rule("Identifier", new type_rules::ident());
	}

	void yparser::prefix_expr(ystr const& op, ysize prec) {
		m_ExprParser.register_rule(op, new expr_rules::pre_uryop(prec));
	}

	void yparser::postfix_expr(ystr const& op, ysize prec) {
		m_ExprParser.register_rule(op, new expr_rules::post_uryop(prec));
	}

	void yparser::infixl_expr(ystr const& op, ysize prec) {
		m_ExprParser.register_rule(op, new expr_rules::binop(prec, false));
	}

	void yparser::infixr_expr(ystr const& op, ysize prec) {
		m_ExprParser.register_rule(op, new expr_rules::binop(prec, true));
	}

	expr* yparser::parse_expr() {
		return m_ExprParser.parse();
	}

	type_desc* yparser::parse_type() {
		return m_TypeParser.parse();
	}

	param_expr* yparser::parse_param() {
		auto peek_0 = peek();
		auto peek_1 = peek(1);
		yopt<token> name = None;
		if ((	peek_0.identifier() == "Identifier" && peek_1.identifier() == ":")
			||	peek_0.identifier() == ":") {
			if (peek_0.identifier() == "Identifier") {
				name = consume();
			}
			token semicol = consume();
			auto type = m_TypeParser.parse();
			if (type) {
				return new param_expr(name, semicol, type);
			}
			else {
				throw std::exception("Type expected!");
			}
		}
		return nullptr;
	}

	expr* yparser::parse_body() {
		if (peek().identifier() == "{") {
			token begin = consume();
			// TODO

			if (peek().identifier() == "}") {
				token end = consume();
				return new body_expr(begin, end);
			}
			else {
				throw std::exception("'}' expected at the end of a body!");
			}
		}
		else {
			return nullptr;
		}
	}
}