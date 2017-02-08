#include "LexerState.h"

namespace yk
{
	namespace parse
	{
		LexerState::LexerState(const char* buf)
			: Buffer(buf), PosX(0), PosY(0)
		{
		}
	}
}