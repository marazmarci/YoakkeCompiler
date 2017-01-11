#pragma once

namespace yk
{
	struct ASTNode
	{
	public:
		virtual ~ASTNode() { }

	public:
		virtual std::string ToString() = 0;
	};

	std::ostream& operator<<(std::ostream& stream, ASTNode* exp);
}