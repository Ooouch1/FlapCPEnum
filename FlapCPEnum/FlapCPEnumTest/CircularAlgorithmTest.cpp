#include "gtest/gtest.h"

#include "CircularAlgorithm.hpp"
#include <string>

namespace {
	using namespace mylib;
	using namespace std;

	class CircularAlgorithmTest : public ::testing::Test {

	};

	TEST(CircularAlgorithmTest, findFirstIndex) {
		CircularAlgorithm<char> algorithm;

		string pattern;

		//         01234567890
		pattern = "bbbabbbbbb";
		auto firstIndex = algorithm.findFirstIndexOfLeastCircular(pattern, pattern.size());
		ASSERT_EQ(3, firstIndex);

		//         01234567890
		pattern = "bbabbababb";
		firstIndex = algorithm.findFirstIndexOfLeastCircular(pattern, pattern.size());
		ASSERT_EQ(5, firstIndex);

		//         01234567890
		pattern = "bbbabbabab";
		firstIndex = algorithm.findFirstIndexOfLeastCircular(pattern, pattern.size());
		ASSERT_EQ(6, firstIndex);
	}

	TEST(CircularAlgorithmTest, testReverse) {
		CircularAlgorithm<char> algorithm;
		string pattern("01234");
		auto reversed = algorithm.createReverse(pattern, pattern.size());

		ASSERT_EQ('4', reversed[0]);
		ASSERT_EQ('3', reversed[1]);
		ASSERT_EQ('2', reversed[2]);
		ASSERT_EQ('1', reversed[3]);
		ASSERT_EQ('0', reversed[4]);

		auto shifted = algorithm.createShiftedString(reversed, 1, pattern.size());
		ASSERT_EQ('3', shifted[0]);
		ASSERT_EQ('2', shifted[1]);
		ASSERT_EQ('1', shifted[2]);
		ASSERT_EQ('0', shifted[3]);
		ASSERT_EQ('4', shifted[4]);


	}

	TEST(CircularAlgorithmTest, testCanonicality_CP) {
		CircularAlgorithm<char> algorithm;

		string pattern("010101010202");
		auto length = pattern.size();

		ASSERT_TRUE(algorithm.isCanonicalOnSymmetry(pattern, length));

		pattern[0] = -2;
		pattern[2] = -1;
		pattern[4] = -1;
		pattern[6] = -2;
		pattern[8] = -2;
		pattern[10] = -2;

		ASSERT_EQ(6, algorithm.findFirstIndexOfLeastCircular(pattern, length));

		auto reversed = algorithm.createReverse(
			pattern, length);
		ASSERT_EQ(3, algorithm.findFirstIndexOfLeastCircular(reversed, length));

		ASSERT_EQ(algorithm.createShiftedString(reversed, 3, length), algorithm.createCanonicalOnSymmetry(pattern, length));

		ASSERT_FALSE(algorithm.isCanonicalOnSymmetry(pattern, length));


		pattern[0] = -2;
		pattern[2] = -2;
		pattern[4] = -1;
		pattern[6] = -1;
		pattern[8] = -2;
		pattern[10] = -2;

		ASSERT_TRUE(algorithm.isCanonicalOnSymmetry(pattern, pattern.size()));
	}

	TEST(CircularAlgorithmTest, testCanonicality_CP_unique_nonminimal) {
		CircularAlgorithm<char> algorithm;

		string pattern("010101010202");

		pattern[0] = -1;
		pattern[2] = -2;
		pattern[4] = -2;
		pattern[6] = -2;
		pattern[8] = -1;
		pattern[10] = -2;

		ASSERT_FALSE(algorithm.isCanonicalOnSymmetry(pattern, pattern.size()));

	}


	TEST(CircularAlgorithmTest, testCreateCanonical_CP_16Lines_vsMinimal) {
		CircularAlgorithm<char> algorithm;

		//              0123456789012345
		string pattern("0001011010001011");
		//              0123456789012345
		string canonic("0001011000101101");

		// the pattern is not the smallest because
		// 0001011000101101 is it:
		// ring: 00010110100010110001011010001011
		// patt:          0001011010001011
		ASSERT_EQ(9, algorithm.findFirstIndexOfLeastCircular(pattern));
		ASSERT_EQ(canonic, algorithm.createCanonicalOnSymmetry(pattern));

	}

	TEST(CircularAlgorithmTest, testCreateCanonical_CP_16Lines_3patterns) {
		CircularAlgorithm<char> algorithm;

		//               0123456789012345
		string pattern1("1101000101100010");

		// mirror @ 5
		//               0123456789012345
		string pattern2("1101000101101000");

		// rot -7 of pattern2
		//               0123456789012345
		string pattern3("1101000110100010");

		auto canonical = algorithm.createCanonicalOnSymmetry(pattern1);

		ASSERT_EQ(canonical, algorithm.createCanonicalOnSymmetry(pattern2)) << "test canonical1 == canonical2";
		ASSERT_EQ(canonical, algorithm.createCanonicalOnSymmetry(pattern3)) << "test canonical1 == canonical3";

	}

}
