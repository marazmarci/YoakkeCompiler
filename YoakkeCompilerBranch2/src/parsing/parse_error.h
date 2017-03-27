#pragma once

#include "../lexing/position.h"
#include "../yerror.h"

namespace yk {
	class expect_parse_err : public yerror {
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
}
