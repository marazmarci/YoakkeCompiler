#include <sstream>
#include "StringUtils.h"

namespace yk
{
	namespace StringUtils
	{
		std::string GetLine(std::string const& orig, std::size_t idx)
		{
			std::istringstream f(orig);
			std::string line;
			std::string ret;
			while (std::getline(f, line) && idx--) ret = line;
			return ret;
		}

		std::string GenArrow(std::size_t offs, std::size_t len)
		{
			std::string res = "";
			while (offs--) res += ' ';
			while (len--) res += '^';
			return res;
		}

		std::string Position(NodePos const& t)
		{
			return "at line " + std::to_string(t.StartY) +
				", character " + std::to_string(t.StartX);
		}
	}
}