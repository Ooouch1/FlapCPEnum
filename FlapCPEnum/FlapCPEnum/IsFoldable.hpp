#pragma once
#include "RingList.hpp"
#include "foldabilityhelpers.hpp"
#include "AbstractFlapCPAnswerDetecter.hpp"
#include "searchtool.hpp"

namespace enumeration {
	namespace origami {

		// Naive implementation. Answer is: a pattern which can be folded into plane.
		template<typename TSet_Assignment>
		class IsFoldable : public AbstractFlapCPAnswerDetecter<TSet_Assignment> {
			mylib::SharedArrayPointer<u_int> map;

			// temporary collection to compute foldability.
			// defined as instance variable in order to reduce memory allocation.
			mylib::RingArrayList<LineGap> ring;
			FoldabilityRingArrayHelper helper;

			const u_int lineCount;
			const u_int placeCount;

			inline int findMinimalInRing(int startIndex) {
				int index = startIndex;

				for (u_int i = 0; i < ring.count(); i++) {
					if (helper.isMinimalToBeFolded(ring, index)) {
						return index;
					}
					index = ring.nextIndexOf(index);
				}

				return -1;
			}

			bool test(const TSet_Assignment& pattern) {
				auto index = ring.peekHeadIndex();

				while (ring.count() > 2) {
					index = findMinimalInRing(index);
					if (index < 0)
						return false;

					index = helper.foldPartially(ring, index);
				}
				if (ring.count() == 0)
					return true;

				if (ring.count() == 2 && (ring.head() == ring.tail()))
					return true;

				return false;
			}

			IsFoldable() {}
		public:

			IsFoldable(const EncodablePatternBase &flap) : lineCount(flap.count()), placeCount(flap.capacity()) {
				LineIndexMapFactory mapFactory;
				map = mapFactory.create(flap);

			}

			virtual ~IsFoldable() {
			}

			virtual bool isAnswer(const TSet_Assignment& assignments) {

				if (!this->maekawaTheoremHolds(assignments)) {
					return false;
				}

				ring = helper.createRingArray(placeCount, lineCount, assignments, map);

				return test(assignments);
			}

		};


		// Answer is: a pattern which can be folded into a flat plane.
		class FoldabiiltyTesterLinear {

		public:
			FoldabiiltyTesterLinear() {}

			// an implemntation according to erik demeine's book
			// "Geometric Folding Algorithms - Linkages, Origami, Polyhedra".
			// really linear???
			bool isFoldable(mylib::RingArrayList<LineGap>& ring) {
				MinimalAngleIndexManager minimals(ring);

				while (ring.count() > 2) {
					FoldabilityRingArrayHelper helper;

					int minimalIndex;
					//!!! the book lacks this check.
					// ignore old items.
					do {
						do {
							if (minimals.empty()) return false;
							minimalIndex = minimals.pop();
						} while (!ring.exists(minimalIndex));
					} while (!helper.isMinimalToBeFolded(ring, minimalIndex));

					minimalIndex = helper.foldPartially(ring, minimalIndex);

					//!!! the book's explanation is too ambiguous.
					// we have to check 3 angles: merged angle and its surroundings!

					minimals.pushIfMinimalToBeFolded(ring, minimalIndex);
					minimals.pushIfMinimalToBeFolded(ring, ring.nextIndexOf(minimalIndex));
					minimals.pushIfMinimalToBeFolded(ring, ring.prevIndexOf(minimalIndex));
				}

				if (ring.count() == 0)
					return true;

				if (ring.count() == 2 && (ring.head() == ring.tail()))
					return true;

				return false;
			}
		};

		template<typename TCharArray, char FLAT, char MAJOR, char MINOR>
		class IsFoldableStringLinear : public mylib::IAnswerDetecter<TCharArray> {
			const u_int placeCount;
		public:
			IsFoldableStringLinear(const u_int& placeCount) : placeCount(placeCount) {

			}

			virtual ~IsFoldableStringLinear() {
			}

			virtual bool isAnswer(const TCharArray& assignments) {
				FoldabilityRingArrayHelper helper;
				auto ring = helper.createRingArray<FLAT, MAJOR, MINOR>(assignments, placeCount);

				FoldabiiltyTesterLinear tester;
				return tester.isFoldable(ring);
			}
		};

		// Answer is: a pattern which can be folded into a flat plane.
		template<typename TSet_Assignment>
		class IsFoldableLinear : public AbstractFlapCPAnswerDetecter<TSet_Assignment> {
			mylib::SharedArrayPointer<u_int> map;

			mylib::RingArrayList<LineGap> ring;
			const u_int lineCount;
			const u_int placeCount;
			FoldabilityRingArrayHelper helper;

			IsFoldableLinear() {}

		public:

			IsFoldableLinear(const EncodablePatternBase &flap) :
				ring(flap.capacity()), lineCount(flap.count()), placeCount(flap.capacity()) {

				LineIndexMapFactory mapFactory;
				map = mapFactory.create(flap);

			}

			virtual ~IsFoldableLinear() {
			}

			virtual bool isAnswer(const TSet_Assignment& assignments) {

				if (!this->maekawaTheoremHolds(assignments)) {
					return false;
				}

				ring = helper.createRingArray(placeCount, lineCount, assignments, map);

				FoldabiiltyTesterLinear tester;

				auto l = tester.isFoldable(ring);


//#ifdef DEBUG_FOLDABILITY_COMPARE
				//IsFoldable quadratic;
				//auto q = quadratic.isAnswer(assignments);
				//if (q != l)
				//	throw std::runtime_error("linear: " + l + ", quadratic: " + q);
//#endif
				return l;
			}

		};


		template<typename TSet_Assignment>
		class IsFoldableDebugByCompare : public AbstractFlapCPAnswerDetecter<TSet_Assignment> {
			IsFoldable<TSet_Assignment> quadratic;
			IsFoldableLinear<TSet_Assignment> linear;
		public:

			IsFoldableDebugByCompare(const EncodablePatternBase &flap): quadratic(flap), linear(flap) {
			}

			virtual bool isAnswer(const TSet_Assignment& assignments) {
				auto q = quadratic.isAnswer(assignments);
				auto l = linear.isAnswer(assignments);

				if (q != l)
					throw std::runtime_error("linear: " + l + ", quadratic: " + q);

				return q;
			}
		};

		template<typename TSet_Assignment>
		class FoldabilityDetecterFactory : public IFlapCPAnswerDetecterFactory<TSet_Assignment> {
		public:
			virtual ~FoldabilityDetecterFactory() {}
			virtual AbstractFlapCPAnswerDetecter<TSet_Assignment>* create(const EncodablePatternBase& flap) const {
				return new IsFoldable<TSet_Assignment>(flap);
			}
		};

		template<typename TSet_Assignment>
		class FoldabilityLinearDetecterFactory : public IFlapCPAnswerDetecterFactory<TSet_Assignment> {
		public:
			virtual ~FoldabilityLinearDetecterFactory() {}
			virtual AbstractFlapCPAnswerDetecter<TSet_Assignment>* create(const EncodablePatternBase& flap) const {
				return new IsFoldableLinear<TSet_Assignment>(flap);
			}
		};

	}

}