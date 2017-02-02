#pragma once

#include <string>
#include "Node.h"
#include "../Parsing/Token.h"

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
		std::string Identifier;

	public:
		IdentTypeDesc(Token const& ref);
		virtual ~IdentTypeDesc();

	public:
		virtual std::string ToXML() override;
	};
}