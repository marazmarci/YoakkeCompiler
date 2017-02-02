#pragma once

#include <string>
#include "../Parsing/NodePos.h"

namespace yk
{
	namespace StringUtils
	{
		std::string GetLine(std::string const& orig, std::size_t idx);
		std::string GenArrow(std::size_t offs, std::size_t len);
		std::string Position(NodePos const& t);
	}
}