#pragma once

#include "../Common.h"

namespace yk
{
	namespace dbg
	{
		struct Info
		{
			static const ystr name() { return "INFO"; }
		};

		struct Warn
		{
			static const ystr name() { return "WARN"; }
		};

		struct Error
		{
			static const ystr name() { return "ERROR"; }
		};
	}
}