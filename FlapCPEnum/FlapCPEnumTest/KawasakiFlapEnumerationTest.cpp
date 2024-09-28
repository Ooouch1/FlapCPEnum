//#define DEBUG_PRINT_ANSWER

#include "gtest/gtest.h"
#include "kawasakiflapenumeration.hpp"
#include <algorithm>
#include "OutputReceiver.hpp"

namespace {
	using namespace enumeration::origami;

	class KawasakiFlapEnumerationTest : public ::testing::Test {
	protected:

		std::vector<FlapPatternForBraceletEnum> run(u_int placeCount) {
			KawasakiFlapEnumeration<true> enumerator;

			test::OutputReceiver<FlapPatternForBraceletEnum> os;
			enumerator.enumerateSlowly(placeCount, os);

			using namespace std;
			cout << "kawasaki #validCall "<< enumerator.kawasakiStats().validCallCount << endl;
	
			return os.answers;
		}

		std::vector<FlapPatternForBraceletEnum> runFaster(u_int placeCount) {
			KawasakiFlapEnumeration<true> enumerator;

			test::OutputReceiver<FlapPatternForBraceletEnum> os;
			enumerator.enumerate(placeCount, os);

			using namespace std;
			cout << "kawasaki_fast #validCall " << enumerator.kawasakiStats().validCallCount << endl;

			return os.answers;
		}

		std::vector<FlapPatternForBraceletEnum> runNoPruning(u_int placeCount) {
			KawasakiFlapEnumeration<false> enumerator;
			test::OutputReceiver<FlapPatternForBraceletEnum> os;
			enumerator.enumerateNoPruning(placeCount, os);

			return os.answers;
		}

		template <typename TFlap> 
		void print(std::vector<TFlap>& results) {
			for (auto result : results) {
				std::cout << result.encode() << std::endl;
			}
		}
	};

	TEST_F(KawasakiFlapEnumerationTest, placeCountIs4) {
		ASSERT_EQ(2, run(4).size());
	}

	TEST_F(KawasakiFlapEnumerationTest, placeCountIs6) {
		ASSERT_EQ(3, run(6).size());
	}

	TEST_F(KawasakiFlapEnumerationTest, placeCountIs8) {
		ASSERT_EQ(7, run(8).size());
	}
	TEST_F(KawasakiFlapEnumerationTest, placeCountIs8_FASTER) {
		auto results = runFaster(8);
		print(results);
		ASSERT_EQ(7, results.size());
	}

	TEST_F(KawasakiFlapEnumerationTest, placeCountIs10) {
		auto results = run(10);
		ASSERT_EQ(13, results.size());
	}
	TEST_F(KawasakiFlapEnumerationTest, placeCountIs10_FASTER) {
		auto results = runFaster(10);

		//auto results_normal = run(10);		

		//std::cout << "faster:" << std::endl;
		//print(results);

		//std::cout << "normal:" << std::endl;
		//print(results_normal);

		ASSERT_EQ(13, results.size());
	}

	TEST_F(KawasakiFlapEnumerationTest, placeCountIs12) {
		ASSERT_EQ(35, run(12).size());
	}

	TEST_F(KawasakiFlapEnumerationTest, placeCountIs14) {
		ASSERT_EQ(85, run(14).size());
	}

	TEST_F(KawasakiFlapEnumerationTest, placeCountIs16) {
		ASSERT_EQ(257, run(16).size());
	}
	TEST_F(KawasakiFlapEnumerationTest, placeCountIs16_FASTER) {
		ASSERT_EQ(257, runFaster(16).size());
	}

	//TEST_F(KawasakiFlapEnumerationTest, placeCountIs24) {
	//	ASSERT_EQ(28968, run(24).size());
	//}
	TEST_F(KawasakiFlapEnumerationTest, placeCountIs24_FASTER) {
		ASSERT_EQ(28968, runFaster(24).size());
	}

	//TEST_F(KawasakiFlapEnumerationTest, placeCountIs24_Nopruning) {
	//	ASSERT_EQ(28968, runNoPruning(24).size());
	//}

	//TEST_F(KawasakiFlapEnumerationTest, placeCountIs32) {
	//	ASSERT_EQ(4707969, run(32, RunType::FASTER).size()); //NORMAL: 227966 ms
	//}

	// corresponds to A006840   Number of 2n-bead black-white reversible complementable necklaces with n black beads (Online Encyclopedia of Integer Sequences)


}

//#undef DEBUG_PRINT_ANSWER
