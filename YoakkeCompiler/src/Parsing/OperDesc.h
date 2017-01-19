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
		double Precedence;
		AssocT Assoc;
		FixityT Fixity;

	public:
		OperDesc(double prec, AssocT asc)
			: Precedence(prec), Assoc(asc), Fixity(FixityT::Infix)
		{
		}

		OperDesc(double prec, FixityT fx)
			: Precedence(prec), Assoc(AssocT::Noassoc), Fixity(fx)
		{
		}
	};
}