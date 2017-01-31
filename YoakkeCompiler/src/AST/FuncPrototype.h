#pragma once

#include <string>
#include <utility>
#include <vector>
#include "../Parsing/Token.h"

namespace yk
{
	class TypeDesc;

	typedef std::pair<Token, TypeDesc*> ParamPair;
	class FuncPrototype
	{
	public:
		std::vector<ParamPair> Parameters;
		TypeDesc* ReturnType;

	public:
		FuncPrototype(std::vector<ParamPair> const& pars, TypeDesc* ret);
		~FuncPrototype();

	public:
		virtual std::string ToString();
	};
}