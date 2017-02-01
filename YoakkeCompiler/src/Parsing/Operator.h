#pragma once

#include <functional>
#include <string>
#include <vector>

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
		Operator(std::string const& sym, double prec);

	private:
		virtual void __nothing() {}
	};

	class UryOp : public Operator
	{
	public:
		FixityT Fixity;

	public:
		UryOp(std::string const& sym, double prec, FixityT fx);
	};

	class BinOp : public Operator
	{
	public:
		AssocT Assoc;

	public:
		BinOp(std::string const& sym, double prec, AssocT as);
	};

	class StackElemR;
	typedef std::function<bool(std::vector<StackElemR*>&, std::size_t)> MFOpHere;
	typedef std::function<std::size_t(std::vector<StackElemR*>&, std::size_t)> MFOpReduce;
	class MixfixOp : public Operator
	{
	public:
		MFOpHere Here;
		MFOpReduce Reduce;

	public:
		MixfixOp(std::string const& sym, double prec, MFOpHere h, MFOpReduce r);
	};
}