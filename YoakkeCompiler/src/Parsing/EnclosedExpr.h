#pragma once

#include "ExprElem.h"

namespace yk
{
	namespace Enclosed
	{
		bool EncloseHere(yvec<ExprElemR>& stack, ysize idx, ystr const& l, ystr const& r);
		bool FuncCallHere(yvec<ExprElemR>& stack, ysize idx);
		ysize FuncCallReduce(yvec<ExprElemR>& stack, ysize idx, Operator*);
	}
}