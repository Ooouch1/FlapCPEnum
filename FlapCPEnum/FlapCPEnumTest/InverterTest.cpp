#include "gtest/gtest.h"

#include "inverters.hpp"
#include "SetTestBase.hpp"

namespace {
	using namespace enumeration::circular;

	class InverterTest : public SetTestBase {
	protected:
		const unsigned int placeCount = 8;

		//xox
		//x o
		//xxx
		CircularPattern create() {
			CircularPattern pattern(placeCount);

			pattern.add(0);
			pattern.add(2);

			return pattern;
		}
	};

	TEST_F(InverterTest, testRotation) {

		RotationInverter inverterRot2(2, placeCount);
		RotationInverter inverterRot4(4, placeCount);

		CircularPattern pattern(placeCount);
		CircularPattern ans(placeCount);
		pattern.add(0);
		pattern.add(2);
		ans.add(0);
		ans.add(6);
		ASSERT_EQ(ans, inverterRot2(pattern));

		//pattern:
		//xox
		//x o
		//xxx


		//rotated:
		//xox
		//o x
		//xxx
		ASSERT_EQ(0, inverterRot2(2));
		ASSERT_EQ(6, inverterRot2(0));

		//xxx
		//o x
		//xox
		ASSERT_EQ(4, inverterRot4(0));
		ASSERT_EQ(6, inverterRot4(2));
	}

	TEST_F(InverterTest, testMirror) {
		CircularPattern pattern = create();
		MirrorInverter inverterMirror1(1, placeCount);
		MirrorInverter inverterMirror3(3, placeCount);

		//xox
		//x o
		//xxx
		ASSERT_EQ(0, inverterMirror1(2));
		ASSERT_EQ(2, inverterMirror1(0));

		//xxx
		//o x
		//xox
		ASSERT_EQ(4, inverterMirror3(2));
		ASSERT_EQ(6, inverterMirror3(0));

	}
}
