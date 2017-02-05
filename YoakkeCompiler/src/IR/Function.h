#pragma once

#include "../Types.h"
#include "NameGenerator.h"

namespace yk
{
	namespace IR
	{
		class BasicBlock;
		class Type;

		struct Parameter
		{
		public:
			ystr Name;
			Type* PType;

		public:
			Parameter(ystr const& n, Type* t);
		};

		class Function
		{
		public:
			NameGenerator NameGen;
			ystr Name;
			Type* ReturnType;
			yvec<Parameter> Parameters;
			yvec<BasicBlock*> Blocks;

		public:
			Function(ystr const& name, Type* rt, yvec<Parameter> const& params);

		public:
			void Add(BasicBlock* bb);
		};
	}
}