#include "gtest/gtest.h"

#include "RingList.hpp"

namespace {

	class RingListTest : public ::testing::Test {
	protected:
		template<typename Value>
		void assertRingState(mylib::RingList<Value>& ring, 
			const Value& expectedHead, const Value& expectedTail,
			const int& expectedCount) {
			ASSERT_EQ(expectedCount, ring.count());
			ASSERT_EQ(expectedHead, *(ring.head()));
			ASSERT_EQ(expectedTail, *(ring.tail()));
		}

		template<typename Value>
		void assertRingState(mylib::RingArrayList<Value>& ring,
			const Value& expectedValue, const int& index) {
			ASSERT_EQ(expectedValue, ring.get(index));
		}

	};

	TEST_F(RingListTest, testAddRemove) {
		mylib::RingList<int> ring(4);

		ASSERT_EQ(0, ring.count());

		ring.add(0);
		assertRingState(ring, 0, 0, 1);

		ring.add(1);
		assertRingState(ring, 0, 1, 2);

		ring.add(2);
		assertRingState(ring, 0, 2, 3);

		auto itr = ring.head();
		itr = itr.remove();
		ASSERT_EQ(1, *itr);
		assertRingState(ring, 1, 2, 2);

		ring.add(4);
		itr = ring.tail().remove();
		assertRingState(ring, 1, 2, 2);


		itr++;
		ASSERT_EQ(2, *itr);

		itr = ring.tail().remove();
		assertRingState(ring, 1, 1, 1);

		itr = ring.head().remove();

		ASSERT_EQ(0, ring.count());
	}


	TEST_F(RingListTest, testArrayAddRemove) {
		mylib::RingArrayList<int> ring(4);

		ASSERT_EQ(0, ring.count());

		ring.set(0, 10);
		ring.set(1, 12);
		ring.set(3, 14);

		ring.makeLinks();
		ASSERT_EQ(3, ring.count());

		ASSERT_EQ(10, ring[0]);
		ASSERT_EQ(12, ring[1]);
		ASSERT_EQ(14, ring[3]);

		ASSERT_EQ(3, ring.prevIndexOf(0));
		ASSERT_EQ(1, ring.nextIndexOf(0));

		ASSERT_EQ(1, ring.prevIndexOf(3));
		ASSERT_EQ(0, ring.nextIndexOf(3));

		ring.remove(1);
		ASSERT_FALSE(ring.exists(1));
		ASSERT_EQ(2, ring.count());

		ASSERT_EQ(3, ring.prevIndexOf(0));
		ASSERT_EQ(3, ring.nextIndexOf(0));

		ASSERT_EQ(0, ring.prevIndexOf(3));
		ASSERT_EQ(0, ring.nextIndexOf(3));

		ring.insert(2, 22);
		ASSERT_EQ(3, ring.count());
		ASSERT_EQ(22, ring[2]);
		ASSERT_EQ(0, ring.prevIndexOf(2));
		ASSERT_EQ(3, ring.nextIndexOf(2));
	}

	TEST_F(RingListTest, testArrayCopy) {
		mylib::RingArrayList<int> ring1(4);
		ring1.set(0, 10);
		ring1.set(1, 12);
		ring1.set(3, 14);
		ring1.makeLinks();
		ASSERT_EQ(3, ring1.count());
		ASSERT_EQ(3, ring1.peekTailIndex());

		mylib::RingArrayList<int> ring2(6);
		ring2.set(0, 20);
		ring2.set(1, 22);
		ring2.set(3, 24);
		ring2.set(4, 28);
		ring2.set(5, 29);
		ring2.makeLinks();

		ring1 = ring2;
		ASSERT_EQ(6, ring1.size());
		ASSERT_EQ(5, ring1.count());

	}
}
