#include "Logger.h"

namespace yk
{
	namespace dbg
	{
		Logger::Logger(ystr const& nm)
			: m_Name(nm), m_Ostream(std::cout)
		{
		}
	}
}