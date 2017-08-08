#pragma once

#include "../reporting/err_stream.h"
#include "../reporting/msg.h"

#define expect_error(what, parser)					\
{													\
	auto const& tok = (parser).peek();				\
	rep::err_stream::report(						\
			rep::expect_token(						\
			(parser).file(),						\
			tok.Position)							\
			.set(what, tok)							\
			);										\
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
			yopt<typename pre_parselet<T>::result_t>
			parse(token const& tok, yparser& par) override {
				return typename pre_parselet<T>::result_t(
					tok.Position,
					std::make_shared<R>(tok.Value)
				);
			}
		};

		template <typename T, typename R>
		class pass_ign : public pre_parselet<T> {
		public:
			yopt<typename pre_parselet<T>::result_t>
				parse(token const& tok, yparser& par) override {
				return typename pre_parselet<T>::result_t(
					tok.Position,
					std::make_shared<R>()
				);
			}
		};
	}
}
