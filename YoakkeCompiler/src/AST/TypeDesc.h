#pragma once

#include <string>

namespace yk
{
	class TypeDesc
	{
	public:
		virtual ~TypeDesc() { }

	public:
		virtual std::string ToString() = 0;
	};

	class IdentTypeDesc : public TypeDesc
	{
	public:
		std::string Identifier;

	public:
		IdentTypeDesc(std::string const& id);
		virtual ~IdentTypeDesc();

	public:
		virtual std::string ToString() override;
	};
}