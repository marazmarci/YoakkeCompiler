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
		std::string m_What;
		std::string m_Got;
		std::string m_File;
		interval m_Position;

	public:
		expect_parse_err(std::string const& wh, std::string const& g, std::string const& file, interval const& pos);

	public:
		virtual void print(std::ostream& os) const override;
	};

	std::ostream& operator<<(std::ostream& os, parse_err const& err);
}
