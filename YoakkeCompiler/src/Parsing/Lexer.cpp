#include <cctype>
#include <cstring>
#include "Lexer.h"

namespace yk
{
	void Lexer::AddLexeme(std::string const& l, TokenT tt)
	{
		m_Lexemes.insert(std::make_pair(l, tt));
	}

	void Lexer::SetSource(const char* buf)
	{
		m_Buffer = buf;
		m_Ptr = buf;
	}

	Token Lexer::Next()
	{
		begin:

		// EOF
		if (!*m_Ptr) return Token::EPSILON;

		// Comment
		if (*m_Ptr == '/')
		{
			if (*(m_Ptr + 1) == '/')
			{
				m_Ptr += 2;
				while (*m_Ptr && *m_Ptr != '\n') m_Ptr++;
				goto begin;
			}

			if (*(m_Ptr + 1) == '*')
			{
				m_Ptr += 2;
				std::size_t depth = 1;
				while (*m_Ptr)
				{
					if (*m_Ptr == '/' && *(m_Ptr + 1) == '*')
					{
						m_Ptr += 2;
						depth++;
						continue;
					}
					if (*m_Ptr == '*' && *(m_Ptr + 1) == '/')
					{
						m_Ptr += 2;
						depth--;
						if (depth == 0) break;
						continue;
					}
					m_Ptr++;
				}
				goto begin;
			}
		}


		// Skip whitespace
		if (std::isspace(*m_Ptr))
		{
			while (std::isspace(*m_Ptr)) m_Ptr++;
			goto begin;
		}

		// Custom token
		std::string longest = "";
		TokenT type = TokenT::Epsilon;
		for (auto it = m_Lexemes.begin(); it != m_Lexemes.end(); it++)
		{
			std::string const& lex = it->first;
			if (std::strncmp(lex.c_str(), m_Ptr, lex.length()) == 0 && lex.length() > longest.length())
			{
				longest = lex;
				type = it->second;
			}
		}
		if (type != TokenT::Epsilon)
		{
			m_Ptr += longest.length();
			return Token(type, longest);
		}

		// Number literal
		{
			bool real = false;
			std::string num = "";
			while (std::isdigit(*m_Ptr))
			{
				num += *m_Ptr++;
			}
			if (*m_Ptr == '.' && (num.length() || std::isdigit(*(m_Ptr + 1))))
			{
				m_Ptr++;
				num += '.';
				real = true;

				while (std::isdigit(*m_Ptr)) num += *m_Ptr++;
			}

			if (num.length())
			{
				if (real) return Token(TokenT::Real, num);
				return Token(TokenT::Integer, num);
			}
		}

		// Identifier
		if (std::isalpha(*m_Ptr) || *m_Ptr == '_')
		{
			std::string ident = "";
			while (std::isalnum(*m_Ptr) || *m_Ptr == '_') ident += *m_Ptr++;
			return Token(TokenT::Identifier, ident);
		}

		char unknown = *m_Ptr++;
		std::string uk = "";
		uk += unknown;
		std::cout << "Unknown token: " << unknown << " (ASCII: " << +unknown << ")" << std::endl;
		return Token(TokenT::Unknown, uk);
	}
}