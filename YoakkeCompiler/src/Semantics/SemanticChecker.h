#pragma once

#include <string>
#include "../AST/Stmt.h"

namespace yk
{
	class SemanticChecker
	{
	public:
		void Check(Stmt* st);

	private:
		void Error(std::string const& msg);

		void Check(Expr* exp);
	};
}