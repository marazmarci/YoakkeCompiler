#include "Parser.h"
#include "../Utility/StringUtils.h"

namespace yk
{
	namespace parse
	{
		Parser::Parser(Lexer& lexer, dbg::Logger& logger, ystr const& fn)
			: m_Lexer(lexer), m_Logger(logger), m_File(fn)
		{
		}

		bool Parser::Match(ystr const& expect)
		{
			auto tok = Peek(0);
			if (tok.Value == expect)
			{
				Consume();
				return true;
			}
			return false;
		}

		bool Parser::Expect(ystr const& expect)
		{
			auto tok = Peek(0);
			if (tok.Value == expect)
			{
				Consume();
				return true;
			}
			m_Logger.log<dbg::Error>() 
				<< str_utils::FileStamp(m_File, tok.PosY, tok.PosX)
				<< " expected: '" << expect << "' but got: '" << tok.Value << "'"
				<< std::endl;
			return false;
		}

		Token Parser::Consume()
		{
			Peek(0);
			auto tok = m_TokenBuffer[0];
			m_TokenBuffer.erase(m_TokenBuffer.begin());
			return tok;
		}

		Token Parser::Peek(ysize delta)
		{
			while (delta >= m_TokenBuffer.size())
			{
				m_TokenBuffer.push_back(m_Lexer.Next());
			}
			return m_TokenBuffer[delta];
		}
	}
}