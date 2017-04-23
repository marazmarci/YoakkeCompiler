#pragma once

#define expect_error(what, note, parser)			\
{													\
	auto const& tok = (parser).peek();				\
	rep::r()										\
		<< rep::expect_tok(							\
			(parser).file(),						\
			tok.Position,							\
			what,									\
			yparser::format_token(tok),				\
			note) <<rep::endr;						\
}													\
return nullptr;

namespace yk {
	namespace common_rules {
		template <typename Parselet_T, typename Return_T>
		class pass : public Parselet_T {
		public:
			ysptr<typename Parselet_T::return_type>
				parse(token const& begin, yparser& par) override {
				return std::make_shared<Return_T>(begin);
			}
		};
	}
}
