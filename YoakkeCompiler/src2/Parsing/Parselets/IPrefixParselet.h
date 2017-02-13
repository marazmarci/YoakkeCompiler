#pragma once

#include "../Token.h"

namespace yk
{
	namespace parse
	{
		template <typename T, typename P>
		class IPrefixParselet
		{
		public:
			typedef T* ReturnType;

		public:
			virtual T* Parse(P* parser, Token const& sym) = 0;
		};
	}
}