#pragma once

#include <iostream>
#include "report_msg.h"
#include "../common.h"

namespace yk {
	namespace rep {
		class report_stream;

		struct __endrep {};
		extern __endrep endr;

		report_stream& r();

		class report_stream {
		private:
			static std::ostream& s_Ostream;

		public:
			static report_stream& instance();

		private:
			yvec<ysptr<msg>> m_Messages;

		private:
			report_stream();

		public:
			report_stream(report_stream const&) = delete;
			void operator=(report_stream const&) = delete;

		public:
			report_stream& operator<<(msg const& msg);
			report_stream& operator<<(rep::__endrep end);
		};
	}
}
