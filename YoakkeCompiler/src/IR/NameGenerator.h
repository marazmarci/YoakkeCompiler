#pragma once

#include "../Types.h"

namespace yk
{
	namespace IR
	{
		class NameGenerator
		{
		private:
			yset<ystr> m_UsedNames;

		public:
			ystr Get(ystr const& name);
		};
	}
}