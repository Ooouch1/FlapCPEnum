#pragma once
#pragma once
#include "abbreviation.h"
#include "ppcsearch.hpp"
#include "CircularPattern.hpp"
#include "ISet.hpp"
#include "inverters.hpp"
#include "naivesearch.hpp"
#include <vector>

namespace enumeration {

	namespace circular {
		//====================== Using Naive search ======================

		template <typename TSet_Assignment>
		class NotEmptyIsAnswerNaive : naive::IAnswerDetecter<TSet_Assignment> {
		public:
			virtual bool isAnswer(const TSet_Assignment& pattern) {
				return pattern.count() > 0;
			}
		};

		template <typename TSet_Assignment>
		class SymmetryDetecterNaive : public naive::AbstractDuplicationDetecter<TSet_Assignment> {
			std::vector<RotationInverter> rotInverters;
			std::vector<MirrorInverter> mirrorInverters;
			std::vector<MiddleMirrorInverter> midMirrorInverters;

			const u_int placeCount;

		public:
			SymmetryDetecterNaive(u_int placeCount) : placeCount(placeCount) {
				// build inverse functions
				for (u_int i = 1; i < placeCount; i++) {
					RotationInverter rotInv(i, placeCount);
					rotInverters.push_back(rotInv);
				}

				for (u_int i = 1; i < placeCount / 2 + 1; i++) {
					MirrorInverter mirrorInv(i, placeCount);
					mirrorInverters.push_back(mirrorInv);

					MiddleMirrorInverter midMirrorInv(i - 1, placeCount);
					midMirrorInverters.push_back(midMirrorInv);
				}
			}

			template <typename TIterator>
			bool hasGenerated(const TSet_Assignment& pattern,
				const TIterator& answersBegin, const TIterator& answersEnd) const {

				// rotation
				for (auto itr = rotInverters.begin(); itr != rotInverters.end(); itr++) {
					if (knownModificationExists(pattern, answersBegin, answersEnd, *itr)) {
						return true;
					}
				}
				// mirror
				for (auto itr = mirrorInverters.begin(); itr != mirrorInverters.end(); itr++) {
					if (knownModificationExists(pattern, answersBegin, answersEnd, *itr)) {
						return true;
					}

				}

				if (placeCount % 2 != 0) {
					return false;
				}

				// middle mirror
				for (auto itr = midMirrorInverters.begin(); itr != midMirrorInverters.end(); itr++) {
					if (knownModificationExists(pattern, answersBegin, answersEnd, *itr)) {
						return true;
					}
				}
				return false;
			}
		};

		template<typename TOStream>
		class AsymmetryEnumerationNaive {

		public:
			template<typename TSet_Assignment, typename TAnswerDetecter, typename TPruningSuggester>
			void enumerate(const u_int placeCount, TOStream& os,
				TAnswerDetecter& ansDetecter,
				TPruningSuggester& pruning) {

				SymmetryDetecterNaive<TSet_Assignment> symmDetecter(placeCount);
				typedef naive::NaiveSearchTool<TSet_Assignment, TPruningSuggester, TAnswerDetecter, SymmetryDetecterNaive<TSet_Assignment> > SearchTool;
				SearchTool tool;

				tool.setDuplication(symmDetecter);
				tool.setAnswer(ansDetecter);
				tool.setPruning(pruning);

				naive::NaiveSearch<TSet_Assignment, TOStream, SearchTool> search(os, tool);

				TSet_Assignment seed(placeCount);
				//seed.add(0);

				search.enumerate(seed, 0, placeCount, 0);
			}

			template<typename TSet_Assignment>
			void enumerate(u_int placeCount, TOStream& os) {
				NotEmptyIsAnswerNaive<TSet_Assignment> ansDetecter;
				naive::NoPruning<TSet_Assignment> pruning;

				this->enumerate<TSet_Assignment>(placeCount, os, ansDetecter, pruning);
			}


		};

	}

}
