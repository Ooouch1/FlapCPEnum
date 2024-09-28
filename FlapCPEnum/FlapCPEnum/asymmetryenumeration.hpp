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

		//====================== Using PPC search ======================

		template <typename TSet_Assignment>
		class SymmetryDetecter : public ppc::AbstractDuplicationDetecter<TSet_Assignment> {
			std::vector<RotationInverter> rotInverters;
			std::vector<MirrorInverter> mirrorInverters;
			std::vector<MiddleMirrorInverter> midMirrorInverters;

			const u_int placeCount;

		public:
			SymmetryDetecter(u_int placeCount) : placeCount(placeCount) {
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

			virtual bool hasGenerated(const TSet_Assignment& pattern, const int elemEnd, const int prefixTail) const {
				// rotation
				if (this->knownModificationExistsFor(pattern, elemEnd, prefixTail, rotInverters)) {
					return true;
				}

				// mirror
				if (this->knownModificationExistsFor(pattern, elemEnd, prefixTail, mirrorInverters)) {
					return true;
				}

				if (placeCount % 2 != 0) {
					return false;
				}

				// middle mirror
				if (this->knownModificationExistsFor(pattern, elemEnd, prefixTail, midMirrorInverters)) {
					return true;
				}

				return false;
			}
		};


		template<typename TOStream, bool needStats = true>
		class AsymmetryEnumeration {

		public:
			/**
			 * return: call count of recursive function.
			 */
			template<typename TSet_Assignment>
			mylib::EnumerationStats enumerate(const u_int placeCount, TOStream& os,
				mylib::IAnswerDetecter<TSet_Assignment>& ansDetecter,
				mylib::IPruningSuggester<TSet_Assignment>& pruning) {

				SymmetryDetecter<TSet_Assignment> symmDetecter(placeCount);
				ppc::PPCSearchTool<TSet_Assignment> tool;

				tool.setDuplication(symmDetecter);
				tool.setAnswer(ansDetecter);
				tool.setPruning(pruning);

				ppc::ExtendedPPCSearch<TSet_Assignment, TOStream, needStats> search(os, tool);

				TSet_Assignment seed(placeCount);
				//seed.add(0);

				search.enumerate(seed, 0, placeCount, -1, 0);

				return search.stats();
			}

			template<typename TSet_Assignment>
			mylib::EnumerationStats enumerate(u_int placeCount, TOStream& os) {
				mylib::NotEmptyIsAnswer<TSet_Assignment> ansDetecter;
				mylib::NoPruning<TSet_Assignment> pruning;

				return this->enumerate<TSet_Assignment>(placeCount, os, ansDetecter, pruning);
			}


		};

	}
}
