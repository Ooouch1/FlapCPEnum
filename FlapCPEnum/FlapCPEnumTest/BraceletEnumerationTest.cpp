#include "gtest/gtest.h"
#include "BraceletEnumeration.hpp"
#include "OutputReceiver.hpp"
#include "FlapPattern.hpp"
#include <iostream>

namespace {
	using namespace enumeration;
	using namespace enumeration::origami;
	using namespace enumeration::circular;
	using namespace std;

	class BraceletEnumerationTest : public ::testing::Test {
	protected:

		vector<mylib::BitSet> run(u_int placeCount) {
			typedef test::OutputReceiver<mylib::BitSet> AnswerReceiver;
			AnswerReceiver os;
			BinaryBraceletEnumeration<mylib::BitSet, AnswerReceiver> braceletEnum(os);
			braceletEnum.enumerate(placeCount);

			return os.answers;
		}

		vector<vector<char> > run(u_int placeCount, u_int elementCount) {
			typedef test::OutputReceiver<vector<char> > AnswerReceiver;
			AnswerReceiver os;
			BraceletEnumeration<AnswerReceiver> braceletEnum(os);
			braceletEnum.enumerate(placeCount, elementCount);

			return os.answers;
		}

	};

	//******************************************
	// The expected values are generated with:
	// https://www.jasondavies.com/necklaces/
	//******************************************

	TEST_F(BraceletEnumerationTest, testPlaceCountIs4) {
		ASSERT_EQ(6, run(4).size());
	}

	TEST_F(BraceletEnumerationTest, testPlaceCountIs8) {
		ASSERT_EQ(30, run(8).size());
	}

	TEST_F(BraceletEnumerationTest, testPlaceCountIs10) {
		auto results = run(10);

		//for (u_int i = 0; i < results.size(); i++) {
		//	std::cout << results[i].toBString() << std::endl;
		//}

		ASSERT_EQ(78, results.size());
	}

	TEST_F(BraceletEnumerationTest, testPlaceCountIs12) {
		//auto results = run(12);

		//for (u_int i = 0; i < results.size(); i++) {
		//	std::cout << results[i].toBString() << std::endl;
		//}

		//ASSERT_EQ(224, results.size());

		ASSERT_EQ(224, run(12).size());

	}

	TEST_F(BraceletEnumerationTest, testPlaceCountIs16) {
		ASSERT_EQ(2250, run(16).size());
	}

	TEST_F(BraceletEnumerationTest, testPlaceCountIs4_3elements) {
		ASSERT_EQ(21, run(4, 3).size());
	}

	TEST_F(BraceletEnumerationTest, testPlaceCountIs8_3elements) {
		ASSERT_EQ(498, run(8, 3).size());
	}

}
