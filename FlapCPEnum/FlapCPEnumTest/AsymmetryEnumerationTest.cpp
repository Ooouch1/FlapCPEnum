//#define DEBUG_PRINT_ANSWER
//#define RUN_HEAVY_TEST

#include "gtest/gtest.h"
#include "AsymmetryEnumeration.hpp"
#include "naive_asymmetryenumeration.hpp"

#include "OutputReceiver.hpp"

#include "ppcsearch.hpp"
#include "BitSet.hpp"

namespace {
	using namespace enumeration::circular;
	typedef test::OutputReceiver<CircularPattern> OutputReceiver;

	class AsymmetryEnumerationTest : public ::testing::Test {
	protected:

		template<typename TEnumerator>
		std::vector<CircularPattern> run(TEnumerator& enumerator, int placeCount) {
			OutputReceiver receiver;

			enumerator.enumerate<CircularPattern>(placeCount, receiver);

			return receiver.answers;
		}

		template<int placeCount, typename TEnumerator1, typename TEnumerator2>
		void testAnswerCountAreEqual(u_int expected, TEnumerator1& e1, TEnumerator2& e2) {
			auto answers1 = run(e1, placeCount);
			auto answers2 = run(e2, placeCount);

			ASSERT_EQ(expected, answers1.size());

			ASSERT_EQ(answers1.size(), answers2.size());
		}

		typedef naive::NaiveSearchTool<
			CircularPattern,
			naive::NoPruning<CircularPattern>,
			naive::EverythingIsAnswer<CircularPattern>,
			SymmetryDetecterNaive<CircularPattern> > NaiveSearchTool;
	};


	TEST_F(AsymmetryEnumerationTest, testSearchTool) {
		const int placeCount = 6;

		SymmetryDetecter<CircularPattern> detecter(placeCount);
		mylib::BitSet garbages(placeCount);

		ppc::PPCSearchTool<CircularPattern> tool;
		tool.setDuplication(detecter);

		CircularPattern pattern(placeCount);

		pattern.add(0);
		pattern.add(2);
		pattern.add(3);
		pattern.add(4);

		ASSERT_TRUE(tool.hasGenerated(pattern, 5, 2));

	}

	TEST_F(AsymmetryEnumerationTest, testNaiveSearchTool) {
		const int placeCount = 6;
		SymmetryDetecterNaive<CircularPattern> naiveDetecter(placeCount);
		NaiveSearchTool naiveTool;
		naiveTool.setDuplication(naiveDetecter);

		CircularPattern pattern(placeCount);
		pattern.add(0);
		pattern.add(1);
		pattern.add(3);

		std::vector<CircularPattern> answers;
		CircularPattern answer(placeCount);
		answer.add(placeCount - 1);
		answer.add(1 - 1);
		answer.add(3 - 1);
		answers.push_back(answer);

		ASSERT_TRUE(naiveTool.hasGenerated(pattern, answers.begin(), answers.end()));
	}


	TEST_F(AsymmetryEnumerationTest, placeCountIs4) {
		AsymmetryEnumeration<OutputReceiver> ppcSearch;
		AsymmetryEnumerationNaive<OutputReceiver> naiveSearch;

		testAnswerCountAreEqual<4>(5, ppcSearch, naiveSearch);

	}

	TEST_F(AsymmetryEnumerationTest, placeCountIs6) {
		AsymmetryEnumeration<OutputReceiver> ppcSearch;
		AsymmetryEnumerationNaive<OutputReceiver> naiveSearch;

		testAnswerCountAreEqual<6>(12, ppcSearch, naiveSearch);

	}

	TEST_F(AsymmetryEnumerationTest, placeCountIs8) {
		AsymmetryEnumeration<OutputReceiver> ppcSearch;
		AsymmetryEnumerationNaive<OutputReceiver> naiveSearch;

		testAnswerCountAreEqual<8>(29, ppcSearch, naiveSearch);
	}

	TEST_F(AsymmetryEnumerationTest, placeCountIs12) {
		AsymmetryEnumeration<OutputReceiver> ppcSearch;
		AsymmetryEnumerationNaive<OutputReceiver> naiveSearch;

		testAnswerCountAreEqual<12>(223, ppcSearch, naiveSearch);
	}


	TEST_F(AsymmetryEnumerationTest, placeCountIs14) {
		AsymmetryEnumeration<OutputReceiver> ppcSearch;
		AsymmetryEnumerationNaive<OutputReceiver> naiveSearch;

		testAnswerCountAreEqual<14>(686, ppcSearch, naiveSearch);
	}

	TEST_F(AsymmetryEnumerationTest, placeCountIs16) {
		AsymmetryEnumeration<OutputReceiver> ppcSearch;
		ASSERT_EQ(2249, run(ppcSearch, 16).size());
	}

	TEST_F(AsymmetryEnumerationTest, placeCountIs18) {
		AsymmetryEnumeration<OutputReceiver> ppcSearch;
		ASSERT_EQ(7684, run(ppcSearch, 18).size());
	}

#ifdef RUN_HEAVY_TEST
	TEST_F(AsymmetryEnumerationTest, placeCountIs18_naive) {
		AsymmetryEnumerationNaive<OutputReceiver> naiveSearch;
		ASSERT_EQ(7684, run(naiveSearch, 18).size());
	}
#endif

}
//#undef DEBUG_PRINT_ANSWER
//#undef RUN_HEAVY_TEST
