//#define DEBUG_PRINT_ANSWER

#include "gtest/gtest.h"
#include "OutputReceiver.hpp"

#include "FlapPattern.hpp"
#include "BitSet.hpp"
#include "MVEnumeration.hpp"
#include "inverters.hpp"

namespace {
	using namespace std;
	using namespace enumeration::origami;
	typedef mylib::BitSet BitSet;
	typedef test::OutputReceiver<BitSet> OutputReceiver;

	class MVEnumerationTest : public ::testing::Test {
	
	};

	TEST_F(MVEnumerationTest, testMapCreation) {

		FlapPattern flap(8);

		//xox
		//o o
		//xox
		flap.add(0);
		flap.add(2);
		flap.add(4);
		flap.add(6);

		LineIndexMapFactory factory;
		auto map = factory.create(flap);

		for (u_int i = 0; i < flap.count(); i++) {
			ASSERT_TRUE(flap.contains(map[i]));
		}


	}

	TEST_F(MVEnumerationTest, testMappedIndexInverter) {
		const u_int placeCount = 8;
		const u_int lineCount = 4;
		FlapPattern flap(placeCount);

		//xox
		//o o
		//xox
		flap.add(0);
		flap.add(2);
		flap.add(4);
		flap.add(6);

		LineIndexMapFactory factory;
		auto map = factory.create(flap);

		using namespace enumeration;

		circular::RotationInverter rot1(1, placeCount);
		circular::RotationInverter rot2(2, placeCount);

		MappedIndexInverter<circular::RotationInverter> mapped1(rot1, placeCount, map, lineCount);
		MappedIndexInverter<circular::RotationInverter> mapped2(rot2, placeCount, map, lineCount);

		ASSERT_FALSE(mapped1.canInvert());
		ASSERT_TRUE(mapped2.canInvert());

		ASSERT_EQ(0, mapped2(1));
		ASSERT_EQ(1, mapped2(2));
		ASSERT_EQ(2, mapped2(3));
		ASSERT_EQ(3, mapped2(0));

	}

	TEST_F(MVEnumerationTest, testEnumerationRotSymMirrorSym) {
		FlapPattern flap(8);
		OutputReceiver os;

		flap.add(0);
		flap.add(2);
		flap.add(4);
		flap.add(6);

		MVEnumeration<OutputReceiver, BitSet> enumerator;
		enumerator.enumerate(flap, os);
		ASSERT_EQ(1, os.answers.size());

	}

	TEST_F(MVEnumerationTest, testEnumerationMirrorSym) {
		FlapPattern flap(8);
		OutputReceiver os;

		flap.add(0);
		flap.add(2);
		flap.add(4);
		flap.add(6);

		flap.add(1);
		flap.add(3);

		MVEnumeration<OutputReceiver, BitSet> enumerator;
		enumerator.enumerate(flap, os);

//		for (auto answer = os.answers.begin(); answer != os.answers.end(); answer++) {
//#ifdef DEBUG_PRINT_ANSWER
//			std::cout << answer->toString();
//			std::cout << std::endl;
//#endif
//		}


		ASSERT_EQ(9, os.answers.size());
	}

	TEST_F(MVEnumerationTest, testEnumerationMirrorSym_FAST) {
		FlapPattern flap(8);
		OutputReceiver os;

		flap.add(0);
		flap.add(2);
		flap.add(4);
		flap.add(6);

		flap.add(1);
		flap.add(3);

		LinearMVEnumeration<OutputReceiver, BitSet> enumerator;
		enumerator.enumerate(flap, os);

		//		for (auto answer = os.answers.begin(); answer != os.answers.end(); answer++) {
		//#ifdef DEBUG_PRINT_ANSWER
		//			std::cout << answer->toString();
		//			std::cout << std::endl;
		//#endif
		//		}


		ASSERT_EQ(9, os.answers.size());
	}

	TEST_F(MVEnumerationTest, test16Lines) {
		u_int placeCount = 16;

		FlapPattern flap(placeCount);

		for (u_int i = 0; i < placeCount; i++) {
			flap.add(i);
		}

		BitSet assignments(placeCount);
		vector<u_int> indices;

		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(3);
		indices.push_back(7);
		indices.push_back(9);
		indices.push_back(10);
		indices.push_back(12);

		MVSymmetryDetecter<BitSet> detecter(flap);
		auto inverterRefs = detecter.createInverterReferences();

		bool generated = false;
		for (const auto& index : indices){
			assignments.add(index);
			if (detecter.hasGenerated(assignments, placeCount, index)) {
				cout << "hit " << assignments.toString() << endl;
				generated |= true;
				break;
			}
		}
		ASSERT_TRUE(generated) << "before rotated";

		indices.clear();
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(3);
		indices.push_back(7);
		indices.push_back(8);
		indices.push_back(10);
		indices.push_back(14);

		assignments.remove(0, placeCount - 1);
		generated = false;
		for (const auto& index : indices) {
			assignments.add(index);
			if (detecter.hasGenerated(assignments, placeCount, index)) {
				cout << "hit " << assignments.toString() << endl;
				generated |= true;
				break;
			}
		}
		ASSERT_FALSE(generated) << "rotated";
	}
}





//#undef DEBUG_PRINT_ANSWER
