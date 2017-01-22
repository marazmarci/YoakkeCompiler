#pragma once

#include "Token.h"

namespace yk
{
	class ParseState
	{
	public:
		const char* SourcePointer;
		Token CurrentToken;

	public:
		ParseState(const char* src, Token ct)
			: SourcePointer(src), CurrentToken(ct)
		{
		}
	};
}