#include "BitArray.hpp"
#include <iostream>
#include "gtest/gtest.h"


namespace {
	using namespace mylib;

	class BitMapTest : public ::testing::Test {
	protected:
		static const int BIT_LENGTH = 128;
		static const unsigned int oneBlockLength = 8 * sizeof(BitArray::BitBlock);

		BitMapTest() : bitmap(BIT_LENGTH) {}
		virtual void SetUp() {
		}

		BitArray bitmap;

		void assertAllZero(BitArray &bitmap);
		void assertAllOne(BitArray &bitmap);
		void assertSetOne(BitArray &bm, unsigned int index);
		void assertZeroSeq(BitArray &bitmap, unsigned int from, unsigned int end);
		void assertOneSeq(BitArray &bitmap, unsigned int from, unsigned int end);
	};


	TEST_F(BitMapTest, GetSetBits) {

		assertAllZero(bitmap);

		// get/set
		const int index1 = 5;
		const int index2 = BIT_LENGTH - 6;

		assertSetOne(bitmap, index1);
		assertSetOne(bitmap, index2);

		// operated exactly?
		int countOfOne = 0;
		for (unsigned int i = 0; i < bitmap.bitLength(); i++) {
			if (i == index1 || i == index2) {
				ASSERT_TRUE(bitmap.isOne(i)) << bitmap.toString();
				countOfOne++;
			}
			else {
				ASSERT_FALSE(bitmap.isOne(i)) << bitmap.toString();
			}
		}

		ASSERT_EQ(2, countOfOne);
	}

	TEST_F(BitMapTest, FillSeveralBlocks) {
		//std::cout << "all zero" << std::endl;
		//std::cout << bitmap.toString() << std::endl;
		assertAllZero(bitmap);

		//std::cout << "all one" << std::endl;
		bitmap.fillOne(0, BIT_LENGTH);
		//std::cout << bitmap.toString() << std::endl;
		assertAllOne(bitmap);

		const unsigned int halfLength = BIT_LENGTH / 2;

		//std::cout << "11...111..1 -> 11..100..0" << std::endl;
		bitmap.fillZero(halfLength, BIT_LENGTH);
		//std::cout << bitmap.toString() << std::endl;

		assertOneSeq(bitmap, 0, halfLength);
		assertZeroSeq(bitmap, halfLength, BIT_LENGTH);

		//011...10
		bitmap.clear();
		bitmap.fillOne(1, BIT_LENGTH - 1);
		EXPECT_TRUE(bitmap.areAllOne(1, BIT_LENGTH - 1));

		ASSERT_FALSE(bitmap.isOne(0));
		assertOneSeq(bitmap, 1, BIT_LENGTH - 1);
		ASSERT_FALSE(bitmap.isOne(BIT_LENGTH - 1));

	}

	TEST_F(BitMapTest, FillOneBlock) {
		//01...10
		bitmap.fillOne(1, oneBlockLength - 1);
		ASSERT_FALSE(bitmap.isOne(0))
			<< "assert: expect 0 at the first bit.";
		ASSERT_FALSE(bitmap.isOne(oneBlockLength - 1))
			<< "assert: expect 0 at the last bit.";

		assertOneSeq(bitmap, 1, oneBlockLength - 2);

		//0100...010
		bitmap.fillZero(2, oneBlockLength - 2);
		ASSERT_FALSE(bitmap.isOne(0))
			<< "assert: expect 0 at the first bit.";
		ASSERT_FALSE(bitmap.isOne(oneBlockLength - 1))
			<< "assert: expect 0 at the last bit.";

		assertZeroSeq(bitmap, 2, oneBlockLength - 2);
	}


	TEST_F(BitMapTest, AreAllOne) {
		bitmap.fillOne(0, BIT_LENGTH);

		ASSERT_TRUE(bitmap.areAllOne(10, 100));
	}

	TEST_F(BitMapTest, RotateToLower_BitOnly) {
		for (int i = 0; i < 4; i++) {
			bitmap.setOne(i);
		}

		bitmap.rotateToLower(2);

		ASSERT_TRUE(bitmap.isOne(0)) << bitmap.toString() << std::endl;
		ASSERT_TRUE(bitmap.isOne(1)) << bitmap.toString() << std::endl;

		assertZeroSeq(bitmap, 2, BIT_LENGTH - 2);

		ASSERT_TRUE(bitmap.isOne(BIT_LENGTH - 1)) << bitmap.toString() << std::endl;
		ASSERT_TRUE(bitmap.isOne(BIT_LENGTH - 2)) << bitmap.toString() << std::endl;
	}

	TEST_F(BitMapTest, RotateToLower_OneBlock) {
		// to be rotated
		for (int i = 0; i < 4; i++) {
			bitmap.setOne(i);
		}

		// to be shifted
		bitmap.setOne(oneBlockLength + 2);

		//std::cout << "Before: " << std::endl << bitmap.toString() << std::endl;
		bitmap.rotateToLower(oneBlockLength + 2);

		//std::cout << "After: " << std::endl << bitmap.toString() << std::endl;

		// test shift 
		ASSERT_TRUE(bitmap.isOne(0)) << bitmap.toString() << std::endl;
		ASSERT_TRUE(!bitmap.isOne(1)) << bitmap.toString() << std::endl;

		assertZeroSeq(bitmap, 2, BIT_LENGTH - oneBlockLength - 2);

		// test rotate
		ASSERT_TRUE(bitmap.isOne(BIT_LENGTH - oneBlockLength + 1)) << bitmap.toString() << std::endl;
		ASSERT_TRUE(bitmap.isOne(BIT_LENGTH - oneBlockLength)) << bitmap.toString() << std::endl;
		ASSERT_TRUE(bitmap.isOne(BIT_LENGTH - oneBlockLength - 1)) << bitmap.toString() << std::endl;
		ASSERT_TRUE(bitmap.isOne(BIT_LENGTH - oneBlockLength - 2)) << bitmap.toString() << std::endl;
	}


	void BitMapTest::assertSetOne(BitArray &bitmap, unsigned int index) {
		bitmap.setOne(index);
		//std::cout << bitmap.toString() << std::endl;
		ASSERT_TRUE(bitmap.isOne(index));
	}


	void BitMapTest::assertAllZero(BitArray &bitmap) {
		assertZeroSeq(bitmap, 0, bitmap.bitLength());
	}

	void BitMapTest::assertAllOne(BitArray &bitmap) {
		assertOneSeq(bitmap, 0, bitmap.bitLength());
	}

	void BitMapTest::assertZeroSeq(BitArray &bitmap,
		unsigned int from, unsigned int end) {
		for (unsigned int i = from; i < end; i++) {
			ASSERT_FALSE(bitmap.isOne(i)) << "assert fails: expect 0 at "
				<< i << std::endl
				<< bitmap.toString() << std::endl;
		}
	}

	void BitMapTest::assertOneSeq(BitArray &bitmap,
		unsigned int from, unsigned int end) {
		for (unsigned int i = from; i < end; i++) {
			ASSERT_TRUE(bitmap.isOne(i)) << "assert fails: expect 1 at "
				<< i << std::endl
				<< bitmap.toString() << std::endl;
		}
	}

}
