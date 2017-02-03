#include "ParseState.h"

namespace yk
{
	ParseState::ParseState(const char* src, Token ct, ysize rc, ysize cc)
		: SourcePointer(src), CurrentToken(ct), RowCount(rc), ColCount(cc)
	{
	}
}