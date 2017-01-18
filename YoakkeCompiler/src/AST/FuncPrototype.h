#pragma once

#include <string>
#include <utility>
#include <vector>

namespace yk
{
	class TypeDesc;

	typedef std::pair<std::string, TypeDesc*> ParamPair;
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