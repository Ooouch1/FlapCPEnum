#include "gtest/gtest.h"
#include "FlapPattern.hpp"

namespace {

	class FlapPatternTest : public ::testing::Test {
	};

	TEST_F(FlapPatternTest, testEncode) {
		enumeration::origami::FlapPattern flap(8);

		flap.add(0);
		flap.add(1);
		flap.add(3);
		flap.add(6);

		mylib::BitSet mv(flap.count());
		mv.add(2);

		;

		ASSERT_EQ("++1-2+1", flap.encode(mv));

	}

	TEST_F(FlapPatternTest, testKawasakiTheoremHolds) {
		enumeration::origami::FlapPattern flap(8);

		//oox
		//x o
		//oxx
		flap.add(0);

		flap.add(2);
		ASSERT_TRUE(flap.kawasakiCountCanBeZero());

		flap.add(5);
		ASSERT_TRUE(flap.kawasakiCountCanBeZero());

		flap.add(7);
		ASSERT_TRUE(flap.kawasakiCountIsZero());

		// undo / redo

		flap.remove(7);
		ASSERT_TRUE(flap.kawasakiCountCanBeZero());

		flap.remove(5);
		ASSERT_TRUE(flap.kawasakiCountCanBeZero());

		flap.add(5);
		ASSERT_TRUE(flap.kawasakiCountCanBeZero());

		flap.add(7);
		ASSERT_TRUE(flap.kawasakiCountIsZero());


	}


	TEST_F(FlapPatternTest, testKawasakiTheoremHolds_Bracelet) {
		enumeration::origami::FlapPatternForBraceletEnum flap(8);

		//oxx
		//x o
		//xoo
		flap.add(2);
		ASSERT_EQ(0, flap.peekLeftGap());
		ASSERT_EQ(-8, flap.peekRightGap());
		ASSERT_EQ(0, flap.peekLeftkawasakiCount());

		flap.add(3);
		ASSERT_EQ(-1, flap.peekLeftGap());
		ASSERT_EQ(7, flap.peekRightGap());
		ASSERT_EQ(-1, flap.peekLeftkawasakiCount());
		ASSERT_TRUE(flap.kawasakiCountCanBeZero());

		flap.add(4);
		ASSERT_EQ(1, flap.peekLeftGap());
		ASSERT_EQ(-6, flap.peekRightGap());
		ASSERT_EQ(0, flap.peekLeftkawasakiCount());
		ASSERT_TRUE(flap.kawasakiCountCanBeZero());

		flap.add(7);
		ASSERT_EQ(-3, flap.peekLeftGap());
		ASSERT_EQ(3, flap.peekRightGap());
		ASSERT_EQ(-3, flap.peekLeftkawasakiCount());
		ASSERT_TRUE(flap.kawasakiCountIsZero());

		// undo / redo

		flap.remove(7);
		ASSERT_TRUE(flap.kawasakiCountCanBeZero());

		flap.remove(4);
		ASSERT_TRUE(flap.kawasakiCountCanBeZero());

		flap.add(4);
		ASSERT_TRUE(flap.kawasakiCountCanBeZero());

		flap.add(7);
		ASSERT_TRUE(flap.kawasakiCountIsZero());


	}

	TEST_F(FlapPatternTest, testKawasakiTheoremViolated) {
		enumeration::origami::FlapPattern flap(8);

		//oxo
		//o x
		//oxx
		flap.add(1);
		ASSERT_TRUE(flap.kawasakiCountCanBeZero());

		flap.add(5);
		ASSERT_TRUE(flap.kawasakiCountCanBeZero());

		flap.add(6);
		ASSERT_TRUE(flap.kawasakiCountCanBeZero());

		flap.add(7);
		ASSERT_FALSE(flap.kawasakiCountCanBeZero());
		ASSERT_FALSE(flap.kawasakiCountIsZero());

	}


}
