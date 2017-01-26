#pragma once

#include <string>

namespace yk
{
	enum class FixityT
	{
		Prefix, Postfix
	};

	enum class AssocT
	{
		Left, Right, Noassoc
	};

	class Operator
	{
	public:
		std::string Symbol;
		double Precedence;

	public:
		Operator(std::string const& sym, double prec)
			: Symbol(sym), Precedence(prec)
		{
		}

	private:
		virtual void __nothing() {}
	};

	class UryOp : public Operator
	{
	public:
		FixityT Fixity;

	public:
		UryOp(std::string const& sym, double prec, FixityT fx)
			: Operator(sym, prec), Fixity(fx)
		{
		}
	};

	class BinOp : public Operator
	{
	public:
		AssocT Assoc;

	public:
		BinOp(std::string const& sym, double prec, AssocT as)
			: Operator(sym, prec), Assoc(as)
		{
		}
	};
}