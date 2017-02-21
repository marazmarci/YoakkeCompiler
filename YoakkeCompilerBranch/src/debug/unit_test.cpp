#include <iostream>
#include "unit_test.h"

namespace yk {
	unit_test::unit_test() 
		: m_ModTests(0), m_ModTestsSucc(0) {
	}

	unit_test& unit_test::instance() {
		static unit_test inst;
		return inst;
	}

	void unit_test::begin_mod(ystr const& mod) {
		// TODO: ASSERT
		if (m_TestingMod) return;

		m_TestingMod = true;
		m_Mod = mod;
		m_ModTests++;
		m_SubTests = 0;
		m_SubTestsSucc = 0;
		m_ModStatus = true;
	}

	void unit_test::begin_sub(ystr const& sub) {
		// TODO: ASSERT
		if (m_TestingSub) return;
		if (!m_TestingMod) return;

		m_TestingSub = true;
		m_Name = sub;
		m_SubTests++;
		m_SubStatus = true;
	}

	void unit_test::end_sub() {
		// TODO: ASSERT
		if (!m_TestingSub) return;

		if (m_SubStatus) {
			m_SubTestsSucc++;
		}

		m_TestingSub = false;
	}

	void unit_test::end_mod() {
		// TODO: ASSERT
		if (!m_TestingMod) return;
		if (m_TestingSub) return;

		if (m_ModStatus) {
			m_ModTestsSucc++;
		}

		std::cout << "Unit testing " << (m_ModStatus ? "succeeded" : "failed")
			<< " for module: " << m_Mod << 
			" [" << m_SubTestsSucc << '/' << m_SubTests << ']' << std::endl;

		m_TestingMod = false;
	}

	void unit_test::expect(bool eval, ystr const& expr, ystr const& file, ysize line) {
		if (!eval) {
			m_ModStatus = false;
			m_SubStatus = false;
			std::cout << "Test failed for " << m_Mod << " - " << m_Name << ':' << std::endl;
			std::cout << "\tFile: '" << file << "', line: " << line << ", expression: " << expr << std::endl;
		}
	}
}
