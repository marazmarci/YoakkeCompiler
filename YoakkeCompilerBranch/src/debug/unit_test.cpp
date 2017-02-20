#include <iostream>
#include "unit_test.h"

namespace yk {
	yvec<unit_test*> unit_test::Tests;

	unit_test::unit_test(ystr const& name) 
		: m_Name(name) {
	}

	void unit_test::run_all() {
		ysize cnt = 0;
		for (auto t : Tests) {
			if (t->run_test()) {
				cnt++;
			}
		}
	}
}