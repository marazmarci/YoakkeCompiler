#include "YStmtParser.h"
#include "YExprParser.h"
#include "../AST/Expr.h"

namespace yk
{
	namespace parse
	{
		YStmtParser::YStmtParser(const char* buf, ystr const& fn)
			: Parser(new Lexer(), new yvec<Token>(), new dbg::Logger("Parser"), fn)
		{
			m_Lexer->SetSource(buf, fn);

			m_ExprParser = new YExprParser(m_Lexer, m_TokenBuffer, m_Logger, m_File);

			m_Lexer->RegisterSymbol("(");
			m_Lexer->RegisterSymbol(")");
			m_Lexer->RegisterSymbol(";");
			m_Lexer->RegisterSymbol(":");
		}

		ast::Stmt* YStmtParser::ParseStmt()
		{
			// Expression statement
			if (ast::Expr* exp = m_ExprParser->ParseExpr())
			{
				auto tok = Peek(0);
				if (tok.Value == ";")
				{
					Consume();
					return new ast::ExprStmt(exp, tok);
				}
				return new ast::ExprStmt(exp);
			}

			return nullptr;
		}
	}
}