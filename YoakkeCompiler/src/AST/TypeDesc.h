#pragma once

#include <string>
#include "Node.h"
#include "../Parsing/Token.h"
#include "../Types.h"

namespace yk
{
	class TypeSymbol;

	class TypeDesc : public Node
	{
	public:
		TypeSymbol* SymbolForm;

	public:
		TypeDesc(NodePos const& p);
		virtual ~TypeDesc();
	};

	class IdentTypeDesc : public TypeDesc
	{
	public:
		ystr Identifier;

	public:
		IdentTypeDesc(Token const& ref);
		virtual ~IdentTypeDesc();

	public:
		virtual XMLNode* ToXML() override;
	};
}