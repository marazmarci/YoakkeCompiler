#include "FileUtils.h"

#include <fstream>

namespace yk
{
	namespace FileUtils
	{
		std::string ReadFile(const char* name)
		{
			std::ifstream t(name);
			if (t.good())
			{
				std::string str((std::istreambuf_iterator<char>(t)),
					std::istreambuf_iterator<char>());
				t.close();
				return str;
			}

			return "";
		}
	}
}