#pragma once

#include <iostream>
#include <string>
#include "../lexing/position.h"

namespace yk {
	class parse_err {
	public:
		virtual void print(std::ostream& os) const = 0;
	};

	class expect_parse_err : public parse_err {
	private:
		ystr m_What;
		ystr m_Got;
		ystr m_File;
		interval m_Position;

	public:
		expect_parse_err(ystr const& wh, ystr const& g, ystr const& file, interval const& pos);

	public:
		virtual void print(std::ostream& os) const override;
	};

	std::ostream& operator<<(std::ostream& os, parse_err const& err);
}
