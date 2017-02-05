#pragma once

#include "NameGenerator.h"
#include "../Types.h"

namespace yk
{
	namespace IR
	{
		class Function;

		class Unit
		{
		public:
			NameGenerator NameGen;
			yvec<Function*> Functions;

		public:
			Unit();

		public:
			void Add(Function* fn);
		};
	}
}