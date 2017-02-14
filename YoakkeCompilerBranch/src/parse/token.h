#pragma once

#include "../common.h"
#include "../utility/variant.h"

namespace yk {
	struct keyword_token {
	public:
		ystr Value;

	public:
		keyword_token(ystr const& val);
	};

	struct literal_token {
	public:
		const ystr Identifier;
		const ystr Value;

	public:
		literal_token(ystr const& id, ystr const& val);
	};

	class token {
	private:
		using var_t = yvar<keyword_token, literal_token>;

	private:
		var_t m_Data;

	public:
		token(ystr const& val);
		token(ystr const& id, ystr const& val);

	public:
		bool is_keyword();
		bool is_literal();

		ystr value();
		ystr identifier();
	};
}