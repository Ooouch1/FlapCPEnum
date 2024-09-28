#include "gtest/gtest.h"

#include "IsFoldable.hpp"
#include "FlapPattern.hpp"

namespace {
	using namespace enumeration::origami;
	using namespace mylib;

	class FoldabilityTest : public ::testing::Test {
	protected:

		
	};

	// This case fails if minimality check is done only on the merged angle.
	// we have to do that on the adjacent angles as well.
	TEST_F(FoldabilityTest, isFoldableLinear_16_MMMVVVVEMEMEEMEE) {
		FlapPatternForBraceletEnum flap(16);

		//0123456789012345
		//MMMVVVVEMEMEEMEE
		flap.add(0);	//#01
		flap.add(1);	//#02
		flap.add(2);	//#03
		flap.add(3);	//#04
		flap.add(4);	//#05
		flap.add(5);	//#06
		flap.add(6);	//#07
		flap.add(8);	//#08
		flap.add(10);	//#09
		flap.add(13);	//#10

		IsFoldableLinear<mylib::BitSet> isFoldable(flap);
		BitSet assignments(10);
		assignments.add(3);
		assignments.add(4);
		assignments.add(5);
		assignments.add(6);

		ASSERT_TRUE(isFoldable.isAnswer(assignments));
	}
}
