#include "LogStream.h"
#include "Logger.h"

namespace yk
{
	LogStream::LogStream(Logger& l, std::ostream& os)
		: m_Logger(l), m_Stream(os)
	{
	}

	void LogStream::Flush()
	{
		m_Stream << std::endl;
		m_Stream.flush();
		m_Logger.m_Logging = false;
	}
}