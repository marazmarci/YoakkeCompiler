#pragma once

#include <string>

namespace yk
{
	enum class OperatorT
	{
		Unary, Binary
	};

	enum class FixityT
	{
		Prefix, Infix, Postfix
	};

	enum class AssocT
	{
		Left, Right
	};

	struct Operator
	{
	public:
		OperatorT Type;
		FixityT Fixity;
		std::string Symbol;
		std::uint32_t Precedence;
		AssocT Associativity;

	public:
		Operator(OperatorT typ, FixityT fix, std::string sym, std::uint32_t prec, AssocT as)
			: Type(typ), Fixity(fix), Symbol(sym), Precedence(prec), Associativity(as)
		{
		}
	};
}