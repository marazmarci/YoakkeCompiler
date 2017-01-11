#include <cstring>
#include <cctype>
#include "Lexer.h"

namespace yk
{
	Lexer::Lexer()
	{
	}

	token_list Lexer::Tokenize(const char* src)
	{
		std::vector<Token> tokens;

		while (*src)
		{
			if (*src == ' ' || *src == '\t' || *src == '\r')
			{
				src++;
				continue;
			}

			// Symbol ////////////////////////////////////////////////////////
			// Find the longest fitting
			std::size_t longest = 0;
			std::string sym = "";
			for (auto it = m_Symbols.begin(); it != m_Symbols.end(); it++)
			{
				std::string const& val = it->first;
				std::string const& orig = it->second;
				if (std::strncmp(val.c_str(), src, val.length()) == 0)
				{
					if (longest < val.length())
					{
						longest = val.length();
						sym = orig;
					}
				}
			}
			if (longest > 0)
			{
				src += longest;
				tokens.push_back(Token(TokenT::Symbol, sym));
				continue;
			}

			// Number ////////////////////////////////////////////////////////
			std::string num = "";
			bool real = false;
			while (std::isdigit(*src))
			{
				num += *src++;
			}
			if (*src == '.' && std::isdigit(*(src + 1)))
			{
				real = true;
				if (num.length() == 0)
				{
					num += "0";
				}
				num += '.';
				src++;
				while (std::isdigit(*src))
				{
					num += *src++;
				}
			}
			if (num.length() > 0)
			{
				if (real)
				{
					tokens.push_back(Token(TokenT::Float, num));
				}
				else
				{
					tokens.push_back(Token(TokenT::Int, num));
				}
				continue;
			}

			// Ident /////////////////////////////////////////////////////////
			std::string ident = "";
			if (std::isalpha(*src) || *src == '_')
			{
				while (std::isalnum(*src) || *src == '_')
				{
					ident += *src++;
				}

				tokens.push_back(Token(TokenT::Ident, ident));
				continue;
			}

			// Unknown
			src++;
		}

		return tokens;
	}

	bool Lexer::AddSymbol(std::string const& val)
	{
		auto it = m_Symbols.find(val);
		if (it == m_Symbols.end())
		{
			m_Symbols.insert(std::make_pair(val, val));
			return true;
		}

		return true;
	}

	bool Lexer::AddAlias(std::string const& val, std::string const& alias)
	{
		auto it = m_Symbols.find(alias);
		if (it == m_Symbols.end())
		{
			m_Symbols.insert(std::make_pair(alias, val));
			return true;
		}

		return true;
	}
}