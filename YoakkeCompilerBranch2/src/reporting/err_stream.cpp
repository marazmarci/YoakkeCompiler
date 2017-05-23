#include "err_stream.h"
#include "msg.h"
#include "../settings.h"

namespace yk {
	namespace rep {
		std::ostream& err_stream::s_Ostream = std::cout;
		ysize err_stream::s_WarnCount = 0;
		ysize err_stream::s_ErrCount = 0;

		void err_stream::report(msg const& m) {
			msg const* mm = &m;
			if (auto const* e = dynamic_cast<err const*>(mm)) {
				s_ErrCount++;
				s_Ostream << "Error: ";
				m.print_head(s_Ostream);
				s_Ostream << std::endl;
				if (compiler_settings::ErrVerbosity >= verbosity_t::L1) {
					if (m.Message) {
						s_Ostream << *m.Message << std::endl;
					}
					if (compiler_settings::ErrVerbosity >= verbosity_t::L2) {
						m.print_verb(s_Ostream);
						if (compiler_settings::ErrVerbosity >= verbosity_t::L3
							&& m.Note) {
							s_Ostream << "Note: " << *m.Note << std::endl;
						}
					}
				}
				if (compiler_settings::ErrAlwaysTerminate || e->Fatal) {
					throw cannot_continue();
				}
			}
			else {
				s_WarnCount++;
				s_Ostream << "Warning: ";
				m.print_head(s_Ostream);
				s_Ostream << std::endl;
				if (compiler_settings::WarnVerbosity >= verbosity_t::L1) {
					if (m.Message) {
						s_Ostream << *m.Message << std::endl;
					}
					if (compiler_settings::WarnVerbosity >= verbosity_t::L2) {
						m.print_verb(s_Ostream);
						if (compiler_settings::WarnVerbosity >= verbosity_t::L3
							&& m.Note) {
							s_Ostream << "Note: " << *m.Note << std::endl;
						}
					}
				}
			}
		}
	}
}
