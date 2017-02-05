#include "BasicBlock.h"

namespace yk
{
	namespace IR
	{
		BasicBlock::BasicBlock(ystr const& name)
			: Name(name), Func(nullptr)
		{
		}
	}
}