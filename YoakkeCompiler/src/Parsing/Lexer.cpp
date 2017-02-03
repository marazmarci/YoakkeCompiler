#include <algorithm>
#include <cctype>
#include <cstring>
#include <iostream>
#include <sstream>
#include "Lexer.h"

namespace yk
{
	Lexer::Lexer()
		: m_Logger(Logger("Lexical Analysis"))
	{
	}

	void Lexer::AddLexeme(ystr const& l, TokenT tt)
	{
		if (ValidLexeme(l))
			m_Lexemes.insert(std::make_pair(l, tt));
		else
			m_Logger.log() << "Cannot add invalid lexeme: '" << l << '\'' << log::endlog;
	}

	void Lexer::SetSource(const char* buf)
	{
		m_Buffer = buf;
		m_Ptr = buf;

		m_RowCount = 1;
		m_ColCount = 0;
	}

	Token Lexer::Next()
	{
		begin:

		// EOF
		if (!*m_Ptr) 
			return Token(TokenT::Epsilon, " <End of File> ", m_RowCount, m_ColCount);

		// Comment
		if (Match("//"))
		{
			ConsumeUntil("\n");
			goto begin;
		}

		if (Match("/*"))
		{
			ysize depth = 1;
			while (*m_Ptr)
			{
				if (Match("/*"))
				{
					depth++;
					continue;
				}
				if (Match("*/"))
				{
					depth--;
					if (depth == 0) break;
					continue;
				}
				IncPtr();
			}

			goto begin;
		}


		// Skip whitespace
		if (std::isspace(*m_Ptr))
		{
			while (std::isspace(*m_Ptr)) IncPtr();
			goto begin;
		}

		// Custom token
		TokenT type = TokenT::Epsilon;
		for (auto it = m_Lexemes.rbegin(); it != m_Lexemes.rend(); ++it)
		{
			ystr const& lex = it->first;
			ysize len = lex.length();
			if (std::strncmp(lex.c_str(), m_Ptr, len) == 0)
			{
				IncPtrSBy(len);
				return Token(it->second, lex, m_RowCount, m_ColCount);
			}
		}

		// Number literal
		{
			ystr num = "";
			while (std::isdigit(*m_Ptr))
			{
				num += IncPtrS();
			}
			if (*m_Ptr == '.' && (num.length() || std::isdigit(*(m_Ptr + 1))))
			{
				IncPtrS();
				num += '.';

				while (std::isdigit(*m_Ptr))
				{
					num += IncPtrS();
				}

				return Token(TokenT::Real, num, m_RowCount, m_ColCount);
			}

			if (num.length())
				return Token(TokenT::Integer, num, m_RowCount, m_ColCount);
		}

		// Identifier
		if (std::isalpha(*m_Ptr) || *m_Ptr == '_')
		{
			ystr ident = "";
			while (std::isalnum(*m_Ptr) || *m_Ptr == '_')
			{
				ident += IncPtrS();
			}
			return Token(TokenT::Identifier, ident, m_RowCount, m_ColCount);
		}

		char unknown = IncPtrS();
		m_Logger.log() << "Unknown token: '" 
			<< unknown << "' (ASCII: " << +unknown << ')' << log::endlog;
		return Token(TokenT::Unknown, ystr(1, unknown), m_RowCount, m_ColCount);
	}

	void Lexer::ConsumeUntil(ystr const& c)
	{
		while (*m_Ptr && std::strncmp(m_Ptr, c.c_str(), c.length()) != 0)
		{
			IncPtrS();
		}
	}

	bool Lexer::Match(ystr const& c)
	{
		if (*m_Ptr && std::strncmp(m_Ptr, c.c_str(), c.length()) == 0)
		{
			IncPtrSBy(c.length());
			return true;
		}
		return false;
	}

	bool Lexer::ValidLexeme(ystr const& v)
	{
		return v.find_first_of(' ') == ystr::npos;
	}
}