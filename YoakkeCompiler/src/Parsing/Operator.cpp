#include "Operator.h"

namespace yk
{
	Operator::Operator(ystr const& sym, double prec)
		: Symbol(sym), Precedence(prec)
	{
	}

	UryOp::UryOp(ystr const& sym, double prec, FixityT fx)
		: Operator(sym, prec), Fixity(fx)
	{
	}

	BinOp::BinOp(ystr const& sym, double prec, AssocT as)
		: Operator(sym, prec), Assoc(as)
	{
	}

	MixfixOp::MixfixOp(ystr const& sym, double prec, MFOpHere h, MFOpReduce r)
		: Operator(sym, prec), Here(h), Reduce(r)
	{
	}
}