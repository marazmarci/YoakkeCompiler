#pragma once

#include <map>
#include <string>
#include <vector>
#include "Token.h"
#include "../Types.h"
#include "../Logging/Logger.h"

namespace yk
{
	class Lexer
	{
	private:
		friend class Parser;

	private:
		const char* m_Buffer;
		const char* m_Ptr;
		ymap<const ystr, TokenT> m_Lexemes;
		yset<ystr> m_Keywords;
		ysize m_RowCount;
		ysize m_ColCount;
		Logger m_Logger;

	public:
		Lexer();

	public:
		void AddLexeme(ystr const& l, TokenT tt);
		void AddKeyword(ystr const& l);
		void SetSource(const char* buf);

		Token Next();

	private:
		void ConsumeUntil(ystr const& c);
		bool Match(ystr const& c);

		bool ValidLexeme(ystr const& v);
		bool ValidKeyword(ystr const& v);

		inline char IncPtr()
		{
			if (*m_Ptr == '\n')
			{
				m_ColCount = 0;
				m_RowCount++;
			}
			else
			{
				m_ColCount++;
			}

			return *m_Ptr++;
		}

		inline void IncPtrBy(ysize n)
		{
			for (ysize i = 0; i < n; i++) IncPtr();
		}

		inline char IncPtrS()
		{
			m_ColCount++;
			return *m_Ptr++;
		}

		inline void IncPtrSBy(ysize n)
		{
			m_Ptr += n;
			m_ColCount += n;
		}
	};
}