
#include "gtest/gtest.h"
#include <iostream>

#include "ItemCountingStream.hpp"
#include <algorithm>
#include "FlapCPEnumeration.hpp"
#include "OutputReceiver.hpp"
#include "IsFoldable.hpp"
#include "CircularAlgorithm.hpp"

#include "FlapCPDirectEnumeration.hpp"

#include "DiffTool.hpp"

namespace {
	using namespace enumeration::origami;
	using namespace std;

	class FlapCPEnumerationTest : public ::testing::Test {
	protected:
		typedef enumeration::Countable<u_int, string> Countable;
		typedef test::OutputReceiver<Countable> OutputReceiver;

		template<typename AnswerAction>
		void forEachAnswer(OutputReceiver& receiver, AnswerAction& action) {
			auto& answers = receiver.answers;
			for_each(answers.begin(), answers.end(),
				action);
		}

		void printAnswers(OutputReceiver& receiver) {
			vector<string> values;
			forEachAnswer(receiver, [&](const auto& ans) {
				values.push_back(ans.value);
			});

			sort(values.begin(), values.end());

			for_each(values.begin(), values.end(),	[](const auto& value) {
				cout << value << endl; 
			});
		}

		template<typename Modifier>
		void storeAnswers(OutputReceiver& receiver, vector<std::string>& storage, const Modifier& m) {
			forEachAnswer(receiver, [&](const auto& ans) {
				storage.push_back(m(ans.value));
			});
		}


		template<int expectedAnswerCount, typename TestedEnumeration, typename ConfidentEnumeration>
		void answerComparisonTest(int placeCount) {

			OutputReceiver receiver;
			ConfidentEnumeration enumerator;
			TestedEnumeration enumerator_tested;


			vector<string> answers, answers_test;

			enumerator.enumerateCPString(placeCount, receiver);

			mylib::CircularAlgorithm<char> circularAlgorithm;
			auto canonicalizer = [&](const auto& v) -> string {
				return circularAlgorithm.createCanonicalOnSymmetry(v, v.size()); };
			storeAnswers(receiver, answers, canonicalizer);
			receiver.clear();

			enumerator_tested.enumerateCPString(placeCount, receiver);
			storeAnswers(receiver, answers_test, canonicalizer);

			auto diffs = mylib::DiffTool::diffBetweenUniques<string>(
				answers.begin(), answers.end(), "conf",
				answers_test.begin(), answers_test.end(), "test"
			);
			for (auto& diff: diffs) {
				std::cout << diff.second << "_" << diff.first << std::endl;
			}


			ASSERT_EQ(expectedAnswerCount, receiver.answers.size());

		}

		template<typename TEnumerator, int expectedAnswerCount>
		void simpleTest(int placeCount, bool doPrint = false) {
			OutputReceiver receiver;
			TEnumerator enumerator;

			enumerator.enumerateCPString(placeCount, receiver);

			if(doPrint)
				printAnswers(receiver);

			cout << "MV stats: " << enumerator.mvStats().validCallCount << endl;

			ASSERT_EQ(expectedAnswerCount, receiver.answers.size());

		}

		class DummyOutStream {
		public:
			template<typename Obj>
			DummyOutStream& operator<<(Obj& o) { return *this; }
		};
		template<typename TEnumerator, int expectedAnswerCount>
		void countingTest(int placeCount) {
			DummyOutStream os;
			enumeration::origami::CountOnlyEncoder<DummyOutStream> stream(os);
			TEnumerator enumerator;

			enumerator.enumerate(placeCount, stream);
			ASSERT_EQ(expectedAnswerCount, enumerator.mvStats().answerCount); // bug fix: use mvStats() to get the # of flat-foldable pattern.

		}

		template<int expectedAnswerCount>
		void simpleTest_direct(int placeCount) {
			typedef test::OutputReceiver<vector<char> > Receiver;
			Receiver receiver;

			FlapCPDirectEnumeration<Receiver> enumerator(receiver);

			enumerator.enumerate(placeCount);

			ASSERT_EQ(expectedAnswerCount, receiver.answers.size());

		}

	};


	TEST_F(FlapCPEnumerationTest, testPlaceCountIs6) {
		simpleTest<FoldableFlapCPEnumeration<>, 6>(6);
	}
	TEST_F(FlapCPEnumerationTest, testPlaceCountIs6_DIRECT) {
		simpleTest_direct<6>(6);
	}

	TEST_F(FlapCPEnumerationTest, testPlaceCountIs6_CRIMP) {
		simpleTest<FoldableFlapCPCrimpBasedEnumeration<>, 6>(6);
	}

	TEST_F(FlapCPEnumerationTest, testPlaceCountIs8) {
		simpleTest<FoldableFlapCPEnumeration<>, 20>(8);
	}
	TEST_F(FlapCPEnumerationTest, testPlaceCountIs8_DIRECT) {
		simpleTest_direct<20>(8);
	}
	//TEST_F(FlapCPEnumerationTest, testPlaceCountIs8_CRIMP) {
	//	simpleTest<FoldableFlapCPAmortizedEnumeration<>, 20>(8);
	//	//simpleTest<FoldableFlapCPAmortizedEnumeration<>, 20>(8, true);
	//}

	TEST_F(FlapCPEnumerationTest, testPlaceCountIs10) {
		simpleTest<FoldableFlapCPEnumeration<>, 87>(10);
	}
	//TEST_F(FlapCPEnumerationTest, testPlaceCountIs10_CRIMP) {
	//	simpleTest<FoldableFlapCPAmortizedEnumeration<>, 87>(10);
	//}

	//TEST_F(FlapCPEnumerationTest, testPlaceCountIs12) {
	//	//simpleTest<FoldableFlapCPEnumeration<>, 420>(12);
	//	answerComparisonTest<420, FoldableFlapCPEnumeration<>, SlowTestFoldableFlapCPEnumeration<> >(12);
	//}

	TEST_F(FlapCPEnumerationTest, testPlaceCountIs12_DIRECT) {
		simpleTest_direct<420>(12);
	}

	TEST_F(FlapCPEnumerationTest, testPlaceCountIs14) {
		simpleTest<FoldableFlapCPEnumeration<>, 2254>(14);
	}
	TEST_F(FlapCPEnumerationTest, testPlaceCountIs14_DIRECT) {
		simpleTest_direct<2254>(14);
	}
	TEST_F(FlapCPEnumerationTest, testPlaceCountIs14_linearMV) {
		simpleTest<LinearMVFoldableFlapCPEnumeration<>, 2254>(14);
	}
	TEST_F(FlapCPEnumerationTest, testPlaceCountIs14_MVLSL) {
		simpleTest<MVLSLFoldableFlapCPEnumeration<>, 2254>(14);
	}


	TEST_F(FlapCPEnumerationTest, testPlaceCountIs16) {
		simpleTest<FoldableFlapCPEnumeration<>, 12676>(16);
	}
	TEST_F(FlapCPEnumerationTest, testPlaceCountIs16_DIRECT) {
		simpleTest_direct<12676>(16);
	}
	TEST_F(FlapCPEnumerationTest, testPlaceCountIs16_linearMV) {
		simpleTest<LinearMVFoldableFlapCPEnumeration<>, 12676>(16);
	}
	TEST_F(FlapCPEnumerationTest, testPlaceCountIs16_MVLSL) {
		simpleTest<MVLSLFoldableFlapCPEnumeration<>, 12676>(16);
	}

	TEST_F(FlapCPEnumerationTest, testPlaceCountIs16_ExMVLSL) {
		simpleTest<ExMVLSLFoldableFlapCPEnumeration<>, 12676>(16);
	}

	//TEST_F(FlapCPEnumerationTest, testPlaceCountIs18) {
	//	simpleTest<ExMVLSLFoldableFlapCPEnumeration<>, 73819>(18);
	//}

	//TEST_F(FlapCPEnumerationTest, testPlaceCountIs20) {
	//	simpleTest<ExMVLSLFoldableFlapCPEnumeration<>, 438795>(20);
	//}

	//TEST_F(FlapCPEnumerationTest, testPlaceCountIs22) {
	//	simpleTest<ExMVLSLFoldableFlapCPEnumeration<>, 2649555>(22);
	//}

	//TEST_F(FlapCPEnumerationTest, testPlaceCountIs16_Compare_linearMV) {
	//	answerComparisonTest<12676, FastMVFoldableFlapCPEnumeration<>, FoldableFlapCPEnumeration<> >(16);
	//}

	//TEST_F(FlapCPEnumerationTest, testPlaceCountIs16_CRIMP) {
	//	simpleTest<FoldableFlapCPAmortizedEnumeration<>, 12676>(16);
	//}

	//TEST_F(FlapCPEnumerationTest, testPlaceCountIs24_ExMVLSL) {
	//	countingTest<ExMVLSLFoldableFlapCPEnumeration<>, 16188915>(24);
	//}

}