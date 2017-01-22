#pragma once

#include <map>
#include <string>
#include <vector>
#include "Token.h"

namespace yk
{
	class Lexer
	{
	private:
		friend class Parser;

	private:
		const char* m_Buffer;
		const char* m_Ptr;
		std::map<const std::string, TokenT> m_Lexemes;
		std::size_t m_RowCount;
		std::size_t m_ColCount;

	public:
		Lexer();

	public:
		void AddLexeme(std::string const& l, TokenT tt);
		void SetSource(const char* buf);

		Token Next();

	private:
		void ConsumeUntil(std::string const& c);
		bool Match(std::string const& c);

		bool ValidLexeme(std::string const& v);

		void Error(std::string const& msg);

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

		inline void IncPtrBy(std::size_t n)
		{
			for (std::size_t i = 0; i < n; i++) IncPtr();
		}

		inline char IncPtrS()
		{
			m_ColCount++;
			return *m_Ptr++;
		}

		inline void IncPtrSBy(std::size_t n)
		{
			m_Ptr += n;
			m_ColCount += n;
		}
	};
}