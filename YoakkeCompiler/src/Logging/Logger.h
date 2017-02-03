#pragma once

#include <string>
#include <iostream>
#include "LogStream.h"
#include "../Types.h"

namespace yk
{
	class Logger
	{
	private:
		friend class LogStream;

	private:
		ystr m_Name;
		bool m_Logging;
		LogStream m_Stream;

	public:
		Logger(ystr const& name);

	public:
		LogStream& log();
	};
}