#pragma once

#include "../Types.h"

namespace yk
{
	namespace IR
	{
		class Type
		{
		public:
			ystr Identifier;
			ysize BitCount;

		public:
			Type(ystr const& nm, ysize bits);
		};

		class Integer : public Type
		{
		public:
			Integer(ysize bits);
		};
	}
}