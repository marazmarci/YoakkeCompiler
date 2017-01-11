#include <sstream>
#include <iostream>
#include "AST.h"

namespace yk
{
	std::ostream& operator<<(std::ostream& stream, ASTNode* exp)
	{
		stream << exp->ToString();
		return stream;
	}
}