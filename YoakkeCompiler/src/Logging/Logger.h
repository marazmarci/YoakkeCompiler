#pragma once

#include <string>
#include <iostream>
#include "LogStream.h"

namespace yk
{
	class Logger
	{
	private:
		friend class LogStream;

	private:
		std::string m_Name;
		bool m_Logging;
		LogStream m_Stream;

	public:
		Logger(std::string const& name);

	public:
		LogStream& log();
	};
}