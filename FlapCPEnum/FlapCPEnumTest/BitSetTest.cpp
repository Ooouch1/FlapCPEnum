#include "gtest/gtest.h"

#include "BitSet.hpp"
#include "SetTestBase.hpp"

namespace {
	using namespace mylib;

	class BitSetTest : public SetTestBase {
	protected:
		const int length = 16;
	};

	TEST_F(BitSetTest, testAddAndContains) {
		BitSet bitset(length);

		bitset.add(0);
		bitset.add(length / 2);

		assertContains(bitset, 0);
		assertContains(bitset,(length / 2));

		assertContainsNothing(bitset, 1, length / 2);
		assertContainsNothing(bitset, length / 2 + 1, length);

		for (int i = 1; i < length / 2; i++) {
			bitset.add(i);
		}

		assertContainsAll(bitset, 0, length / 2 + 1);
		assertContainsNothing(bitset, length / 2 + 1, length);
	}

	TEST_F(BitSetTest, testRemove) {
		BitSet bitset(length);

		for (int i = 0; i < length; i++) {
			bitset.add(i);
		}

		// 1011...1 111...1
		bitset.remove(1);

		// test 1011...
		assertContains(bitset, 0);
		assertNotContains(bitset, 1);
		assertContainsAll(bitset, 2, length);

		// 1011...1 000...01
		bitset.remove(length / 2, length - 1);

		// test 1011...
		assertContains(bitset, 0);
		assertNotContains(bitset, 1);
		assertContainsAll(bitset, 2, length / 2);

		// test 000...01
		assertContainsNothing(bitset, length /2, length - 1);
		assertContains(bitset, length - 1);

		
	}
}
