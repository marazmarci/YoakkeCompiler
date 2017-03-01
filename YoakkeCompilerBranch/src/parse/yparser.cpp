#include "yparser.h"
#include "../ast/expr.h"
#include "../utility/map_ext.h"

namespace yk {
	namespace common_rules {
		template <typename T>
		using T_parser = prec_parser<T>;

		template <typename T>
		using T_pre_parselet = prefix_parselet<T, T_parser<T>>;
		
		template <typename T>
		using T_in_parselet = infix_parselet<T, T_parser<T>>;

		template <typename T, typename RET>
		class ident : public T_pre_parselet<T> {
		public:
			T* parse(token const& begin, T_parser<T>* parser) override {
				return new RET(begin);
			}
		};

		template <typename T, typename RET>
		class binop : public T_in_parselet<T> {
		public:
			const bool right;

		public:
			binop(ysize prec, bool r)
				: T_in_parselet<T>(prec), right(r) {
			}

		public:
			T* parse(T* left, token const& begin, T_parser<T>* parser) override {
				auto rhs = parser->parse(precedence() - (right ? 1 : 0));
				if (rhs) {
					return new RET(left, rhs, begin);
				}
				else {
					throw std::exception("RHS expected for operator!");
				}
			}
		};

		template <typename T, typename RET>
		class list : public T_in_parselet<T> {
		public:
			list(ysize prec)
				: T_in_parselet<T>(prec) {
			}

		public:
			T* parse(T* left, token const& begin, T_parser<T>* parser) override {
				yvec<T*> list = { left };
				auto rhs = parser->parse(precedence() - 1);
				if (rhs) {
					list.push_back(rhs);
					while (parser->peek().Identifier() == begin.Identifier()) {
						parser->consume();
						if (rhs = parser->parse(precedence() - 1)) {
							list.push_back(rhs);
						}
						else {
							throw std::exception("RHS expected for list!");
						}
					}
					return new RET(list);
				}
				else {
					throw std::exception("RHS expected for list!");
				}
			}
		};
	}

	namespace expr_rules {
		typedef prec_parser<expr> expr_parser;
		typedef prefix_parselet<expr, expr_parser> expr_pre_parselet;
		typedef infix_parselet<expr, expr_parser> expr_in_parselet;

		using ident = common_rules::ident<expr, ident_expr>;
		using binop = common_rules::binop<expr, bin_expr>;
		using list = common_rules::list<expr, list_expr>;

		class int_literal : public expr_pre_parselet {
		public:
			expr* parse(token const& begin, expr_parser* parser) override {
				return new int_lit_expr(begin);
			}
		};

		class real_literal : public expr_pre_parselet {
		public:
			expr* parse(token const& begin, expr_parser* parser) override {
				return new real_lit_expr(begin);
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
				if (parser->peek().Identifier() != ")") {
					if (auto par = ypar->parse_param()) {
						params.push_back(par);
						while (parser->peek().Identifier() == ",") {
							parser->consume();
							par = ypar->parse_param();
							if (par) {
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
					else if (auto exp = parser->parse()) {
						end = parser->match_id(")");
						if (end.some()) {
							return exp;
						}
						else {
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
				if (parser->peek().Identifier() == "->") {
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
				if (last.Identifier() != ")") {
					auto params = parser->parse();
					if (!params) {
						throw std::exception("Parameters expected!");
					}
					operands.push_back(params);
					last = parser->peek();
					if (last.Identifier() != ")") {
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
		
		class let : public expr_pre_parselet {
		public:
			expr* parse(token const& begin, expr_parser* parser) override {
				yparser* ypar = reinterpret_cast<yparser*>(parser->parent());
				auto patt = ypar->parse_pattern();
				if (patt) {
					type_desc* type = nullptr;
					expr* val = nullptr;
					if (parser->peek().Identifier() == ":") {
						parser->consume();
						if (!(type = ypar->parse_type())) {
							throw std::exception("Type expected after ':' in let!");
						}
					}
					if (parser->peek().Identifier() == "=") {
						parser->consume();
						if (!(val = ypar->parse_expr())) {
							throw std::exception("Value expected after '=' in let!");
						}
					}
					return new let_expr(begin, patt, type, val);
				}
				else {
					throw std::exception("Pattern expected after 'let'!");
				}
			}
		};
	}

	namespace type_rules {
		typedef prec_parser<type_desc> type_parser;
		typedef prefix_parselet<type_desc, type_parser> type_pre_parselet;
		typedef infix_parselet<type_desc, type_parser> type_in_parselet;

		using ident = common_rules::ident<type_desc, ident_type_desc>;
		using binop = common_rules::binop<type_desc, bin_type_desc>;
		using list = common_rules::list<type_desc, list_type_desc>;

		class enclose : public type_pre_parselet {
		public:
			type_desc* parse(token const& begin, type_parser* parser) override {
				auto sub = parser->parse();
				if (parser->peek().Identifier() == ")") {
					auto end = parser->consume();
					if (sub) {
						return sub;
					}
					else {
						return new ident_type_desc(token("unit", "unit"));
					}
				}
				else {
					throw std::exception("')' expected!");
				}
			}
		};
	}

	namespace pattern_rules {
		typedef prec_parser<pattern> pattern_parser;
		typedef prefix_parselet<pattern, pattern_parser> pattern_pre_parselet;
		typedef infix_parselet<pattern, pattern_parser> pattern_in_parselet;

		using ident = common_rules::ident<pattern, ident_pattern>;
		using binop = common_rules::binop<pattern, bin_pattern>;
		using list = common_rules::list<pattern, list_pattern>;

		class skip : public pattern_pre_parselet {
		public:
			pattern* parse(token const& begin, pattern_parser* parser) override {
				return new skip_pattern(begin);
			}
		};

		class enclose : public pattern_pre_parselet {
		public:
			pattern* parse(token const& begin, pattern_parser* parser) override {
				auto sub = parser->parse();
				if (sub) {
					if (parser->peek().Identifier() == ")") {
						auto end = parser->consume();
						return sub;
					}
					else {
						throw std::exception("')' expected!");
					}
				}
				else {
					throw std::exception("Pattern expected after '('!");
				}
			}
		};
	}

	yparser::yparser(token_buffer* buff)
		: parser(buff), m_ExprParser(buff, this), m_TypeParser(buff, this), 
		m_PatternParser(buff, this) {
		// Atomic
		m_ExprParser.register_rule("Identifier", new expr_rules::ident());
		m_ExprParser.register_rule("Integer", new expr_rules::int_literal());
		m_ExprParser.register_rule("Real", new expr_rules::real_literal());
		m_ExprParser.register_rule("(", new expr_rules::enclosed());
		m_ExprParser.register_rule("let", new expr_rules::let());

		m_ExprParser.register_rule("::", new expr_rules::const_asgn(1));

		// Operators
		m_ExprParser.register_rule(",", new expr_rules::list(2));
		
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

		// TYPE DESCRIPTOR ///////////////////////////////////////////////
		m_TypeParser.register_rule("Identifier", new type_rules::ident());
		m_TypeParser.register_rule("(", new type_rules::enclose());

		m_TypeParser.register_rule(",", new type_rules::list(1));
		m_TypeParser.register_rule("->", new type_rules::binop(2, false));

		// PATTERN DESCRIPTOR ////////////////////////////////////////////
		m_PatternParser.register_rule("_", new pattern_rules::skip());
		m_PatternParser.register_rule("Identifier", new pattern_rules::ident());
		m_PatternParser.register_rule("(", new pattern_rules::enclose());
		m_PatternParser.register_rule(",", new pattern_rules::list(1));
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

	pattern* yparser::parse_pattern() {
		return m_PatternParser.parse();
	}

	param_expr* yparser::parse_param() {
		auto peek_0 = peek();
		auto peek_1 = peek(1);
		yopt<token> name = None;
		if ((peek_0.Identifier() == "Identifier" && peek_1.Identifier() == ":")
			|| peek_0.Identifier() == ":") {
			if (peek_0.Identifier() == "Identifier") {
				name = consume();
			}
			token semicol = consume();
			auto type = m_TypeParser.parse(1);
			if (type) {
				if (name.some()) {
					return new param_expr(name.get(), semicol, type);
				}
				else {
					return new param_expr(semicol, type);
				}
			}
			else {
				throw std::exception("Type expected!");
			}
		}
		return nullptr;
	}

	expr* yparser::parse_body() {
		if (peek().Identifier() == "{") {
			token begin = consume();
			
			yvec<stmt*> stmts;
			while (stmt* st = parse_stmt()) {
				stmts.push_back(st);
			}

			if (peek().Identifier() == "}") {
				token end = consume();
				return new body_expr(begin, end, stmts);
			}
			else {
				throw std::exception("'}' expected at the end of a body!");
			}
		}
		else {
			return nullptr;
		}
	}

	stmt* yparser::parse_stmt() {
		expr* sub = parse_expr();
		if (sub) {
			bool need_semicol = false;
			if (sub) {
				if (auto binexp = dynamic_cast<bin_expr*>(sub)) {
					if (binexp->OP.Identifier() == "::") {
						need_semicol = dynamic_cast<block_expr*>(binexp->RHS) == nullptr;
					}
				}
			}
			if (peek().Identifier() == ";") {
				token semicol = consume();
				return new expr_stmt(sub, semicol);
			}
			else {
				if (need_semicol) {
					throw std::exception("';' expected at the end of const assignment!");
				}
				else {
					return new expr_stmt(sub);
				}
			}
		}
		else {
			return nullptr;
		}
	}
}