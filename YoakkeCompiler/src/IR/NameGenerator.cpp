#include "NameGenerator.h"

namespace yk
{
	namespace IR
	{
		ystr NameGenerator::Get(ystr const& name)
		{
			ystr realn = name;
			auto it = m_UsedNames.find(realn);
			if (it == m_UsedNames.end() && realn != "")
				m_UsedNames.insert(realn);
			else
			{
				ysize cnt = 0;
				while (true)
				{
					ystr name2 = realn + std::to_string(cnt);
					auto it2 = m_UsedNames.find(name2);
					if (it2 == m_UsedNames.end())
					{
						m_UsedNames.insert(name2);
						realn = name2;
						break;
					}
					cnt++;
				}
			}

			return realn;
		}
	}
}