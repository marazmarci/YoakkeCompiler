#pragma once

#include "position.h"
#include "../common.h"
#include "../yerror.h"

namespace yk {
	class unknown_tok_err : public yerror {
	private:
		ystr m_Token;
		ystr m_File;
		position m_Position;

	public:
		unknown_tok_err(ystr const& tok, ystr const& file, position const& pos);

	public:
		virtual void print(std::ostream& os) const override;
	};

	class lex_no_file_err : public yerror {
	private:
		ystr m_File;

	public:
		lex_no_file_err(ystr const& file);

	public:
		virtual void print(std::ostream& os) const override;
	};

	class unexpected_eof : public yerror {
	private:
		ystr m_File;
		position m_Position;
		ystr m_Hint;

	public:
		unexpected_eof(ystr const& file, position const& pos, ystr const& hint = "");

	public:
		virtual void print(std::ostream& os) const override;
	};
}
