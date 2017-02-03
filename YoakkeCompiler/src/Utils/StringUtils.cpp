#include <sstream>
#include <cctype>
#include "StringUtils.h"

namespace yk
{
	namespace StringUtils
	{
		ystr GetLine(ystr const& orig, ysize idx)
		{
			std::istringstream f(orig);
			ystr line;
			ystr ret;
			while (std::getline(f, line) && idx--) ret = line;
			return ret;
		}

		ystr GenArrow(ysize offs, ysize len, ystr const& ref)
		{
			ystr res = "";
			for (ysize i = 0; i < offs; i++)
			{
				if (std::isspace(ref[i]))
					res += ref[i];
				else
					res += ' ';
			}
			while (len--) res += '^';
			return res;
		}

		ystr Position(NodePos const& t)
		{
			return "at line " + std::to_string(t.StartY) +
				", character " + std::to_string(t.StartX);
		}
	}
}