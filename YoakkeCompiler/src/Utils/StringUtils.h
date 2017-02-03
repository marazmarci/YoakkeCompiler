#pragma once

#include <string>
#include "../Parsing/NodePos.h"
#include "../Types.h"

namespace yk
{
	namespace StringUtils
	{
		ystr GetLine(ystr const& orig, ysize idx);
		ystr GenArrow(ysize offs, ysize len);
		ystr Position(NodePos const& t);
	}
}