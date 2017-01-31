#pragma once

#include <string>

namespace yk
{
	namespace StringUtils
	{
		std::string GetLine(std::string const& orig, std::size_t idx);
		std::string GenArrow(std::size_t offs, std::size_t len);
	}
}