#pragma once

#include "../Common.h"

namespace yk
{
	namespace parse
	{
		class LexerState
		{
		public:
			const char* Buffer;
			ysize PosX;
			ysize PosY;

		public:
			LexerState(const char* buf);
		};
	}
}