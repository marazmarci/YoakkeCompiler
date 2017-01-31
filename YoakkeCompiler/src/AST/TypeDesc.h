#pragma once

#include <string>
#include "../Parsing/Token.h"

namespace yk
{
	class TypeSymbol;

	class TypeDesc
	{
	public:
		TypeSymbol* SymbolForm;

	public:
		TypeDesc();
		virtual ~TypeDesc() { }

	public:
		virtual std::string ToString() = 0;
	};

	class IdentTypeDesc : public TypeDesc
	{
	public:
		std::string Identifier;
		Token Reference;

	public:
		IdentTypeDesc(std::string const& id, Token const& ref);
		virtual ~IdentTypeDesc();

	public:
		virtual std::string ToString() override;
	};
}