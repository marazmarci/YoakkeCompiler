#include "err_stream.h"
#include "err_msg.h"

namespace yk {
	namespace rep {
		std::ostream& err_stream::s_Ostream = std::cout;
		ysize err_stream::s_WarnCount = 0;
		ysize err_stream::s_ErrCount = 0;

		void err_stream::report(msg const& m) {
			m.print(s_Ostream);
			msg const* mm = &m;
			if (auto const* e = dynamic_cast<err const*>(mm)) {
				s_ErrCount++;
				if (!e->can_continue()) {
					throw cannot_continue();
				}
			}
			else {
				s_WarnCount++;
			}
		}
	}
}
