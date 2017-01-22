#pragma once

namespace yk
{
	enum class AssocT
	{
		Left, Right, Noassoc
	};

	enum class FixityT
	{
		Prefix, Infix, Postfix
	};

	class OperDesc
	{
	public:
		std::string Symbol;
		double Precedence;
		AssocT Assoc;
		FixityT Fixity;

	public:
		OperDesc(std::string const& sym,  double prec, AssocT asc)
			: Symbol(sym), Precedence(prec), Assoc(asc), Fixity(FixityT::Infix)
		{
		}

		OperDesc(std::string const& sym, double prec, FixityT fx)
			: Symbol(sym), Precedence(prec), Assoc(AssocT::Noassoc), Fixity(fx)
		{
		}
	};
}