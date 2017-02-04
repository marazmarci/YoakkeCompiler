#pragma once

#include <functional>
#include <string>
#include <vector>
#include "../Types.h"

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
		ystr Symbol;
		double Precedence;

	public:
		Operator(ystr const& sym, double prec);

	private:
		virtual void __nothing() {}
	};

	class UryOp : public Operator
	{
	public:
		FixityT Fixity;

	public:
		UryOp(ystr const& sym, double prec, FixityT fx);
	};

	class BinOp : public Operator
	{
	public:
		AssocT Assoc;

	public:
		BinOp(ystr const& sym, double prec, AssocT as);
	};

	class ExprElemR;
	typedef std::function<bool(yvec<ExprElemR>&, ysize)> MFOpHere;
	typedef std::function<ysize(yvec<ExprElemR>&, ysize, Operator*)> MFOpReduce;
	class MixfixOp : public Operator
	{
	public:
		MFOpHere Here;
		MFOpReduce Reduce;

	public:
		MixfixOp(ystr const& sym, double prec, MFOpHere h, MFOpReduce r);
	};
}