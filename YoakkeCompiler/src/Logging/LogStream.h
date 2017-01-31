#pragma once

#include <iostream>
#include <sstream>

namespace yk
{
	class Logger;

	namespace log
	{
		struct __endlog {};
		struct __endl {};

		static __endlog endlog;
		static __endl endl;
	}

	class LogStream
	{
	private:
		std::ostream& m_Stream;
		Logger& m_Logger;

	public:
		LogStream(Logger& l, std::ostream& os);

	public:
		void Flush();

		template <typename T>
		LogStream& operator<<(T const& v)
		{
			if (m_Logger.m_Logging)
				m_Stream << v;
			return *this;
		}

		template <>
		LogStream& operator<<(log::__endlog const& el)
		{
			Flush();
			return *this;
		}

		template <>
		LogStream& operator<<(log::__endl const& el)
		{
			m_Stream << std::endl;
			return *this;
		}
	};
}