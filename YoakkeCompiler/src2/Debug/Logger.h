#pragma once

#include <iostream>
#include "../Common.h"
#include "LogLevel.h"

namespace yk
{
	namespace dbg
	{
		class Logger
		{
		private:
			ystr m_Name;
			std::ostream& m_Ostream;

		public:
			Logger(ystr const& nm);

		public:
			template <typename T>
			std::ostream& log()
			{
				m_Ostream << m_Name << " [" << T::name() << "]: ";
				return m_Ostream;
			}
		};
	}
}