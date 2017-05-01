#pragma once

#include <iostream>
#include "../common.h"

namespace yk {
	namespace rep {
		class msg;

		struct cannot_continue { };

		class err_stream {
		private:
			static std::ostream& s_Ostream;
			static ysize s_WarnCount;
			static ysize s_ErrCount;

		public:
			static void report(msg const& m);
		};
	}
}
