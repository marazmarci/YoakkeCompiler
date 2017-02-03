#pragma once

#include <string>
#include "../Types.h"

namespace yk
{
	class Symbol
	{
	public:
		ystr Name;

	public:
		Symbol(ystr const& n);

	private:
		virtual void __unneeded() { }
	};
}