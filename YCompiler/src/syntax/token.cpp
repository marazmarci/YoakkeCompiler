#include "token.h"

token::token(interval const& pos, token_t type, ystr const& val)
	: Pos(pos), Type(type), Value(val) {
}

token::token()
	: token(interval(), token_t::Epsilon) {
}

ystr token::fmt() const {
	switch (Type) {
	case token_t::Epsilon:
		return "<epsilon>";

	case token_t::EndOfFile:
		return "end-of-file";

	case token_t::Ident:
		return "identifier";

	case token_t::IntLit:
		return "integer literal";

	case token_t::RealLit:
		return "real literal";

	default:
		return '\'' + Value + '\'';
	}
	return "<error>";
}

const char* token::static_desc(token_t tt) {
	switch (tt) {
	case token_t::Add:			return "'+'";
	case token_t::Arrow:		return "'->'";
	case token_t::Asgn:			return "'='";
	case token_t::Colon:		return "':'";
	case token_t::Comma:		return "','";
	case token_t::Div:			return "'/'";
	case token_t::Dot:			return "'.'";
	case token_t::Else:			return "'else'";
	case token_t::EndOfFile:	return "end-of-file";
	case token_t::Epsilon:		return "<epsilon>";
	case token_t::Eq:			return "'=='";
	case token_t::Fn:			return "'fn'";
	case token_t::Greater:		return "'>'";
	case token_t::GrEq:			return "'>='";
	case token_t::Ident:		return "identifier";
	case token_t::If:			return "'if'";
	case token_t::IntLit:		return "integer literal";
	case token_t::LBrace:		return "'{'";
	case token_t::LBracket:		return "'['";
	case token_t::LeEq:			return "'<='";
	case token_t::Less:			return "'<'";
	case token_t::Let:			return "'let'";
	case token_t::LParen:		return "'('";
	case token_t::Mod:			return "'%'";
	case token_t::Mul:			return "'*'";
	case token_t::Mut:			return "'mut'";
	case token_t::Neq:			return "'<>'";
	case token_t::RBrace:		return "'}'";
	case token_t::RBracket:		return "']'";
	case token_t::RealLit:		return "real literal";
	case token_t::RParen:		return "')'";
	case token_t::Semicol:		return "';'";
	case token_t::Sub:			return "'-'";
	case token_t::Type:			return "'type'";
	case token_t::Const:		return "'const'";
	case token_t::True:			return "'true'";
	case token_t::False:		return "'false'";

	case token_t::DbgWriteTy:	return "<debug>";

	default:					UNIMPLEMENTED;
	}

	UNREACHABLE;
}

std::ostream& operator<<(std::ostream& os, token const& tok) {
	os << '<' << tok.Value << '>';
	return os;
}
