#include "StringUtils.h"

namespace yk
{
	namespace str_utils
	{
		ystr FileStamp(ystr const& file, ysize line, ysize chara)
		{
			return "in file '" + file + "', at line " + 
				std::to_string(line) + ", character " + std::to_string(chara);
		}
	}
}