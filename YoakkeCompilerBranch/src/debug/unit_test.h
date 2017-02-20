#pragma once

#include "../common.h"

namespace yk {
	class unit_test {
	public:
		static yvec<unit_test*> Tests;

	private:
		ystr m_Name;

	public:
		unit_test(ystr const& name);

	public:
		virtual bool run_test() = 0;

	public:
		static void run_all();
	};
}
