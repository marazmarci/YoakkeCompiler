#pragma once

#include "../Types.h"

namespace yk
{
	class XMLAttribute
	{
	public:
		ystr Name;
		ystr Value;

	public:
		XMLAttribute(ystr const& n, ystr const& v);
		virtual ~XMLAttribute();
	};

	class XMLNode
	{
	public:
		ystr Name;
		yvec<XMLAttribute> Attributes;
		yvec<XMLNode*> Children;

	public:
		XMLNode(ystr const& name);
		virtual ~XMLNode();
	};
}