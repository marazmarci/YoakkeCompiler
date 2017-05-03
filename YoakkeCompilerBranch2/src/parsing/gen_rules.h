#pragma once

#include "../reporting/err_stream.h"
#include "../reporting/err_msg.h"

#define expect_error(what, note, parser)			\
{													\
	auto const& tok = (parser).peek();				\
	rep::err_stream::report(						\
			rep::expect_token(						\
			(parser).file(),						\
			tok.Position,							\
			what,									\
			yparser::format_token(tok),				\
			note));									\
}													\
return nullptr;

namespace yk {
	namespace gen_rules {
		template <typename T>
		using pre_parselet = prefix_parselet<T, yparser>;

		template <typename T>
		using in_parselet = prefix_parselet<T, yparser>;

		template <typename T, typename R>
		class pass : public pre_parselet<T> {
		public:
			pre_parselet<T>::return_ptr
				parse(token const& begin, yparser& par) override {
				return std::make_shared<R>(begin);
			}
		};
	}
}
