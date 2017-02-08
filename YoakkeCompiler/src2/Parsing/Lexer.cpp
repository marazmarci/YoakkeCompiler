#include <cctype>
#include <cstring>
#include "Lexer.h"
#include "../Utility/StringUtils.h"

namespace yk
{
	namespace parse
	{
		Lexer::Lexer()
			: m_State(LexerState(nullptr)), m_Logger(dbg::Logger("Lexer"))
		{
		}

		void Lexer::SetSource(const char* buf, ystr const& fn)
		{
			m_State.PosX = 0;
			m_State.PosY = 0;
			m_State.Buffer = buf;
			m_File = fn;
		}

		void Lexer::RegisterSymbol(ystr const& kw)
		{
			auto it = m_Symbols.find(kw);
			auto it2 = m_Keywords.find(kw);
			if (it == m_Symbols.end() && it2 == m_Keywords.end())
				m_Symbols.insert(kw);
		}

		void Lexer::RegisterKeyword(ystr const& kw)
		{
			auto it = m_Symbols.find(kw);
			auto it2 = m_Keywords.find(kw);
			if (it == m_Symbols.end() && it2 == m_Keywords.end())
				m_Keywords.insert(kw);
		}

		Token Lexer::Next()
		{
		begin:
			// Skip spaces
			switch (*m_State.Buffer)
			{
			case ' ':
			case '\t':
				m_State.Buffer++;
				m_State.PosX++;
				goto begin;

			case '\n':
				m_State.Buffer++;
				m_State.PosX = 0;
				m_State.PosY++;
				goto begin;
			}

			// Skip comment
			if (std::strncmp(m_State.Buffer, "//", 2) == 0)
			{
				m_State.Buffer += 2;
				m_State.PosX += 2;
				while (*m_State.Buffer && *m_State.Buffer != '\n')
				{
					m_State.Buffer++;
					m_State.PosX++;
				}
				goto begin;
			}
			if (std::strncmp(m_State.Buffer, "/*", 2) == 0)
			{
				ysize depth = 1;
				m_State.Buffer += 2;
				m_State.PosX += 2;
				while (*m_State.Buffer && depth)
				{
					if (std::strncmp(m_State.Buffer, "/*", 2) == 0)
					{
						m_State.Buffer += 2;
						m_State.PosX += 2;
						depth++;
					}
					else if (std::strncmp(m_State.Buffer, "*/", 2) == 0)
					{
						m_State.Buffer += 2;
						m_State.PosX += 2;
						depth--;
					}
					else if (*m_State.Buffer == '\n')
					{
						m_State.Buffer++;
						m_State.PosX = 0;
						m_State.PosY++;
					}
					else
					{
						m_State.Buffer++;
						m_State.PosX++;
					}
				}
				goto begin;
			}

			// EOF
			if (!*m_State.Buffer)
				return Token(TokenT::Epsilon, " <eof> ", m_State.PosX, m_State.PosY, m_File);

			// Find a best-matching symbol if any
			for (auto it = m_Symbols.rbegin(); it != m_Symbols.rend(); it++)
			{
				auto len = it->size();
				if (std::strncmp(m_State.Buffer, it->c_str(), len) == 0)
				{
					// Found the longest
					Token ret(TokenT::Symbol, *it, m_State.PosX, m_State.PosY, m_File);
					m_State.PosX += len;
					m_State.Buffer += len;
					return ret;
				}
			}

			// Match for a number
			{
				ystr num = "";
				if (std::isdigit(*m_State.Buffer))
				{
					while (std::isdigit(*m_State.Buffer)) num += *m_State.Buffer++;
				}
				// A real can be: x.y, x., .y 
				if (*m_State.Buffer == '.' && 
					(num.size() || std::isdigit(*(m_State.Buffer + 1))))
				{
					num += '.';
					m_State.Buffer++;
					while (std::isdigit(*m_State.Buffer)) num += *m_State.Buffer++;
					Token ret(TokenT::Real, num, m_State.PosX, m_State.PosY, m_File);
					m_State.PosX += num.size();
					return ret;
				}
				if (num.size())
				{
					Token ret(TokenT::Integer, num, m_State.PosX, m_State.PosY, m_File);
					m_State.PosX += num.size();
					return ret;
				}
			}

			// Match for an identifier
			{
				ystr ident = "";
				if (std::isalpha(*m_State.Buffer) || *m_State.Buffer == '_')
				{
					while ((std::isalnum(*m_State.Buffer) || *m_State.Buffer == '_'))
						ident += *m_State.Buffer++;
					auto it = m_Keywords.find(ident);
					Token ret(
						(it == m_Keywords.end()) ? TokenT::Identifier : TokenT::Keyword,
						ident, m_State.PosX, m_State.PosY, m_File);
					m_State.PosX += ident.size();
					return ret;
				}
			}

			char curr = *m_State.Buffer;
			m_Logger.log<dbg::Error>() << 
				"Unknown token: '" << curr << "' (code: " << +curr << ") "
				<< str_utils::FileStamp(m_File, m_State.PosY, m_State.PosX)
				<< std::endl;
			Token ret(TokenT::Epsilon, "" + curr, m_State.PosX, m_State.PosY, m_File);
			m_State.Buffer++;
			m_State.PosX++;
			return ret;
		}

		bool Lexer::HasNext() 
		{
			return *m_State.Buffer != '\0';
		}

		LexerState Lexer::Position()
		{
			return m_State;
		}

		void Lexer::Rollback(LexerState const& pos)
		{
			m_State = pos;
		}
	}
}