#pragma once

#include <functional>
#include "../Common.h"
#include "../Debug/Logger.h"

namespace yk
{
	namespace dbg
	{
		template  <typename T, typename R>
		class UnitTestCase
		{
		public:
			T Arguments;
			R ExpectedResult;
			bool Success;

		public:
			UnitTestCase(T const& args, R const& res, bool succ)
				: Arguments(args), ExpectedResult(res), Success(succ)
			{
			}
		};

		template <typename DATA, typename RESULT>
		class UnitTest
		{
		public:
			typedef UnitTestCase<DATA, RESULT> TestCase;
			typedef std::function<RESULT(DATA)> Tester;

		private:
			yvec<TestCase> m_TestData;
			Tester m_TestFunction;
			dbg::Logger m_Logger;

		public:
			UnitTest(Tester tfunc, ystr const& nam)
				: m_TestFunction(tfunc), m_Logger(nam)
			{
			}

		public:
			void AddTest(DATA const& d, RESULT const& r, bool s)
			{
				m_TestData.push_back(TestCase(d, r, s));
			}

			void RunAll()
			{
				ysize good = 0;
				for (auto c : m_TestData)
				{
					auto res = m_TestFunction(c.Arguments);
					if (c.ExpectedResult == res)
					{
						if (c.Success) good++;
						else
						{
							m_Logger.log<dbg::Error>()
								<< "Unit test succeeded for illegal case: "
								<< c << '!' << std::endl;
						}
					}
					else
					{
						if (c.Success)
						{
							m_Logger.log<dbg::Error>()
								<< "Unit test failed for case: "
								<< c << ", got: '" << res << "'!" << std::endl;
						}
						else good++;
					}
				}
				m_Logger.log<dbg::Info>() << "Unit test ended, success: "
					<< good << " / " << m_TestData.size() << std::endl;
			}
		};
	}
}