#include "Unit.h"
#include "Function.h"

namespace yk
{
	namespace IR
	{
		Unit::Unit()
		{
		}

		void Unit::Add(Function* fn)
		{
			fn->Name = NameGen.Get(fn->Name);
			Functions.push_back(fn);
		}
	}
}