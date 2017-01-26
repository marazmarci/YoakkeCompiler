#pragma once

#include "Token.h"

namespace yk
{
	class ParseState
	{
	public:
		const char* SourcePointer;
		Token CurrentToken;
		std::size_t RowCount;
		std::size_t ColCount;

	public:
		ParseState(const char* src, Token ct, std::size_t rc, std::size_t cc)
			: SourcePointer(src), CurrentToken(ct), RowCount(rc), ColCount(cc)
		{
		}
	};
}