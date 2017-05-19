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
return {};

namespace yk {
	namespace gen_rules {
		template <typename T>
		using pre_parselet = prefix_parselet<T, yparser>;

		template <typename T>
		using in_parselet = prefix_parselet<T, yparser>;

		template <typename T, typename R>
		class pass : public pre_parselet<T> {
		public:
			yopt<typename pre_parselet<T>::return_t>
			parse(token const& tok, yparser& par) override {
				return typename pre_parselet<T>::return_t(
					tok.Position,
					std::make_shared<R>(tok.Value)
				);
			}
		};

		template <typename T, typename R>
		class pass_ign : public pre_parselet<T> {
		public:
			yopt<typename pre_parselet<T>::return_t>
				parse(token const& tok, yparser& par) override {
				return typename pre_parselet<T>::return_t(
					tok.Position,
					std::make_shared<R>()
				);
			}
		};
	}
}
