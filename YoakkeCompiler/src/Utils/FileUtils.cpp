#include "FileUtils.h"

#include <fstream>

namespace yk
{
	namespace FileUtils
	{
		ystr ReadFile(const char* name)
		{
			std::ifstream t(name);
			if (t.good())
			{
				ystr str((std::istreambuf_iterator<char>(t)),
					std::istreambuf_iterator<char>());
				t.close();
				return str;
			}

			return "";
		}
	}
}