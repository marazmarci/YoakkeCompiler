#pragma once

#include "../../Common.h"
#include "../Token.h"

namespace yk
{
	namespace parse
	{
		template <typename T, typename P>
		class IInfixParselet
		{
		public:
			typedef T* ReturnType;

		public:
			const ysize Precedence;

		public:
			IInfixParselet(ysize prec)
				: Precedence(prec)
			{
			}

		public:
			virtual T* Parse(P* parser, T* left, Token const& sym) = 0;
		};
	}
}