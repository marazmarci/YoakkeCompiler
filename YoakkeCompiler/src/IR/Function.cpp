#include "Function.h"
#include "BasicBlock.h"

namespace yk
{
	namespace IR
	{
		Parameter::Parameter(ystr const& n, Type* t)
			: Name(n), PType(t)
		{
		}

		Function::Function(ystr const& name, Type* rt, yvec<Parameter> const& params)
			: Name(name), ReturnType(rt), Parameters(params)
		{
			for (auto& p : Parameters)
				p.Name = NameGen.Get(p.Name);
		}

		void Function::Add(BasicBlock* bb)
		{
			bb->Func = this;
			bb->Name = NameGen.Get(bb->Name);
			Blocks.push_back(bb);
		}
	}
}