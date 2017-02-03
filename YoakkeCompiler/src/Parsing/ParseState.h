#pragma once

#include "Token.h"
#include "../Types.h"

namespace yk
{
	class ParseState
	{
	public:
		const char* SourcePointer;
		Token CurrentToken;
		ysize RowCount;
		ysize ColCount;

	public:
		ParseState(const char* src, Token ct, ysize rc, ysize cc);
	};
}