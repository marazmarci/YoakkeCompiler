#pragma once

#include "../common.h"

namespace yk {
	class unit_test {
	private:
		ystr m_Mod;
		ystr m_Name;
		bool m_TestingMod;
		bool m_TestingSub;
		ysize m_ModTests;
		ysize m_SubTests;
		ysize m_ModTestsSucc;
		ysize m_SubTestsSucc;
		bool m_ModStatus;
		bool m_SubStatus;

	private:
		unit_test();

	public:
		unit_test(unit_test const&)			= delete;
		void operator=(unit_test const&)	= delete;

		void begin_mod(ystr const& mod);
		void begin_sub(ystr const& sub);
		void end_sub();
		void end_mod();

		void expect(bool eval, ystr const& expr, ystr const& file, ysize line);

	public:
		static unit_test& instance();
	};
}

#define UNIT_TEST_MOD(x) ::yk::unit_test::instance().begin_mod(x); {
#define UNIT_TEST_ENDMOD() }; ::yk::unit_test::instance().end_mod();
#define UNIT_TEST_FUNC(x) ::yk::unit_test::instance().begin_sub(x); {
#define UNIT_TEST_ENDFUNC() }; ::yk::unit_test::instance().end_sub();
#define UNIT_TEST_EXPECT(x, y) yk::unit_test::instance().expect(x, #y, __FILE__, __LINE__)
#define UNIT_TEST_EXPECT_NOT(x, y) yk::unit_test::instance().expect(!(x), #y, __FILE__, __LINE__)
