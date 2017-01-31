#include "Logger.h"

namespace yk
{
	Logger::Logger(std::string const& name)
		: m_Name(name), m_Logging(false), m_Stream(LogStream(*this, std::cout))
	{
	}

	LogStream& Logger::log()
	{
		m_Logging = true;
		return m_Stream << '[' << m_Name << "]: ";
	}
}