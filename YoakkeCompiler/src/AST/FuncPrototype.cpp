#include "FuncPrototype.h"
#include "TypeDesc.h"

namespace yk
{
	FuncPrototype::FuncPrototype(std::vector<ParamPair> const& pars, TypeDesc* ret)
		: Parameters(pars), ReturnType(ret)
	{
	}

	FuncPrototype::~FuncPrototype()
	{
		for (auto td : Parameters) delete td.second;
		delete ReturnType;
	}

	std::string FuncPrototype::ToString()
	{
		std::string ret = "<FuncPrototype><ReturnType>" + ReturnType->ToString() + "</ReturnType>";
		for (auto td : Parameters)
		{
			ret += "<Parameter name=\"" + td.first.Value + "\">" + td.second->ToString() + "</Parameter>";
		}
		ret += "</FuncPrototype>";
		return ret;
	}
}