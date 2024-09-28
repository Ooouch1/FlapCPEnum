#include "gtest/gtest.h"
#include "BitArray.hpp"
#include <iostream>

namespace {
	using namespace mylib;

	class BitArrayCalcTest : public ::testing::Test {
		virtual void SetUp() {
		}

	};

	TEST(BitArrayCalcTest, AndOpTest) {
		const int length = 65;
		BitArray b1(length);
		BitArray b2(length);

		b1.fillOne(0, length);
		b2.setOne(0);
		b2.setOne(length - 1);

		auto result = (b1 & b2);

		ASSERT_TRUE(result.isOne(0));
		ASSERT_TRUE(result.isOne(length - 1));
		ASSERT_TRUE(result.areAllZero(1, length - 1));
	}

	TEST(BitArrayCalcTest, XOrOpTest) {
		const int length = 64;
		BitArray b1(length);
		BitArray b2(length);

		//  0s 0s 1s 1s
		//  0s 1s 0s 1s
		//->0s 1s 1s 0s
		b1.fillOne(length / 2, length);

		b2.fillOne(length/4, length / 4 * 2);
		b2.fillOne(length / 4 * 3, length);

		auto result = (b1 ^ b2);
		//std::cout << result.toString() << std::endl;

		ASSERT_TRUE(result.areAllZero(0, length/4));
		ASSERT_TRUE(result.areAllOne(length/4, length/4 * 2));
		ASSERT_TRUE(result.areAllOne(length / 4 * 2, length / 4 * 3));
		ASSERT_TRUE(result.areAllZero(length / 4 *3, length));

	}
}
