#pragma once

#include <string>

namespace yk
{
	class Symbol
	{
	public:
		std::string Name;

	public:
		Symbol(std::string const& n);

	private:
		virtual void __unneeded() { }
	};
}