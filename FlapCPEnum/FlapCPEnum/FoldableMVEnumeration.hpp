#pragma once

#include "foldabilityhelpers.hpp"
#include "BitSet.hpp"
#include "MVEnumeration.hpp"
#include "RingList.hpp"
#include "searchtool.hpp"

namespace enumeration {
	namespace origami {

		struct Candidate {
			u_int angleToNext;
			u_int lineIndex, nextIndex;

			Candidate(const u_int& angle, const u_int& index, const u_int& nextIndex)
				: angleToNext(angle), lineIndex(index), nextIndex(nextIndex) {}
			Candidate(const Candidate& source)
				: angleToNext(source.angleToNext), lineIndex(source.lineIndex), nextIndex(source.nextIndex) {}

			bool operator<(const Candidate& right) const {
				if (angleToNext == right.angleToNext) {
					return lineIndex < right.lineIndex;
				}
				return angleToNext < right.angleToNext;
			}
			bool operator>(const Candidate& right) const {
				if (angleToNext == right.angleToNext) {
					return lineIndex > right.lineIndex;
				}
				return angleToNext > right.angleToNext;
			}

			bool operator==(const Candidate& right) const {
				return angleToNext == right.angleToNext && lineIndex == right.lineIndex;
			}
		};

		// FAILED. it works correctly but too slow.
		template<typename TOStream, typename TSet_Assignment = mylib::BitSet, bool needStats = true>
		class FoldableMVEnumerationImpl {
			mylib::SharedArrayPointer<u_int> map;

			FoldabilityRingArrayHelper helper;

			TOStream& outStream;


			mylib::EnumerationStats stats;
			const mylib::IPruningSuggester<TSet_Assignment>& pruning;
			mylib::IAnswerDetecter<TSet_Assignment>& ansDetecter;

			MinimalityChecker orderChecker;

			mylib::RingArrayList<LineGap> sourceRing;


			template <typename TCrimpStack>
			TCrimpStack createCandidateCrimps(mylib::RingArrayList<LineGap>& ring) {
				TCrimpStack candidates;
				candidates.reserve(ring.count());

				auto index = ring.peekHeadIndex();

				for (u_int i = 0; i < ring.count(); i++) {
					auto& gap = ring[index];
					CrimpHelper crimpHelper;
					crimpHelper.pushBackCrimpIfMinimal(ring, index, false, candidates);

					index = ring.nextIndexOf(index);
				}

				sort(candidates.begin(), candidates.end());

				return candidates;

			}

			TSet_Assignment convert(const std::string& assignments) const {
				TSet_Assignment bits(assignments.size());
				for (u_int i = 0; i < bits.capacity(); i++) {
					if (assignments[i] == Crease::MINOR)
						bits.add(i);
				}
				return bits;
			}

			void outputAnswer(const std::string& assignments) {
				using namespace std;

				TSet_Assignment bits = convert(assignments);
				if (ansDetecter.isAnswer(bits)) {
					if (needStats)
						stats.answerCount++;

					//cout << "ANSWER!" << endl << endl;
					outStream << bits;
				}

			}

			std::vector<Crimp> answerCrimps;
			template<typename InverterCollection>
			void enumerate(std::string& assignments, mylib::RingArrayList<LineGap>& ring,
				InverterCollection& inverters) {
				using namespace std;

				if (needStats) {
					stats.callCount++;
				}

				//cout << "assgn:" << assignments << endl
				//	<< "ring:" << ring.toString() << endl;


				// How can i detect symmetric duplication?
				helper.assignToRing(assignments, sourceRing);
				if (!orderChecker.isMinimalCrimpSequence(answerCrimps, sourceRing, inverters))
					return;

				//cout << "VALID!" << endl;

				//if (pruning.needPruning(bits, countOfMinors)) {
				//	return;
				//}

				if (needStats) {
					stats.validCallCount++;
				}

				if (ring.count() == 2) {
					if (ring.head() == ring.tail()) {
						outputAnswer(assignments);
					}
					return;
				}

				// fix all lines if #line = 4
				if (ring.count() == 4 && assignments.find(Crease::UNDEF) == string::npos) {
					auto nextRing = ring;
					const auto& index = answerCrimps.back().centerLineIndex;
					const auto& nextIndex = answerCrimps.back().nextLineIndex;

					helper.foldPartially(nextRing, index);

					// answer?
					if (nextRing.head() == nextRing.tail()) {
						outputAnswer(assignments);
					}

					return;
				}

				// TODO: build a open node list of indices whose crease is to be reversed.
				// + Indices have to be unique.
				// + Choosing crimp folds to be done is also considerable for obtaining distinct patterns.

				// construct next candidates
				auto candidates = createCandidateCrimps<vector<Crimp> >(ring);

				for (auto& candidateCrimp : candidates) {
					auto nextRing = ring;

					if (!helper.isMinimalAngle(nextRing, candidateCrimp.centerLineIndex)) {
						continue;
					}

					if (nextRing.count() == 4) {
						// determine fold

						auto index = nextRing.peekHeadIndex();

						// not-crimped lines
						vector<u_int> assignedIndices;
						for (u_int i = 0; i < nextRing.count(); i++) {
							if (index == candidateCrimp.centerLineIndex || index == candidateCrimp.nextLineIndex) {}
							else {
								nextRing[index].lineType = Crease::MAJOR;
								assignments[index] = Crease::MAJOR;
								assignedIndices.push_back(index);
							}
							index = nextRing.nextIndexOf(index);
						}

						// crimped lines
						callNextBoth(assignments, ring, candidateCrimp, nextRing, inverters);

						// unfold for #line = 2
						assignments[assignedIndices[0]] = Crease::UNDEF;
						assignments[assignedIndices[1]] = Crease::UNDEF;
					}
					else {
						// erase the center and next angles
						helper.foldPartially(nextRing, candidateCrimp.centerLineIndex);

						// determine folds
						callNextBoth(assignments, ring, candidateCrimp, nextRing, inverters);
					}
				}
			}

			template<typename InverterCollection>
			void callNextBoth(std::string& assignments,
				mylib::RingArrayList<LineGap>& ring,
				Crimp& candidateCrimp,
				mylib::RingArrayList<LineGap>& nextRing, const InverterCollection& inverters) {

				using namespace std;

				auto centerIndex = candidateCrimp.centerLineIndex;
				auto nextIndex = ring.nextIndexOf(centerIndex);

				//cout << "try -+ assignment (" << centerIndex << "," << nextIndex << ")" << endl;
				callNext(assignments, candidateCrimp, Crease::MINOR, Crease::MAJOR, nextRing, inverters);

				//cout << "try +- assignment (" << centerIndex << "," << nextIndex << ")" << endl;
				callNext(assignments, candidateCrimp, Crease::MAJOR, Crease::MINOR, nextRing, inverters);

				// unfold
				assignments[centerIndex] = Crease::UNDEF;
				assignments[nextIndex] = Crease::UNDEF;
			}

			template<typename InverterCollection>
			void callNext(std::string& assignments,
				Crimp& candidateCrimp,
				const char& centerType, const char& nextType,
				mylib::RingArrayList<LineGap>& nextRing, const InverterCollection& inverters) {

				assignments[candidateCrimp.centerLineIndex] = centerType;
				assignments[candidateCrimp.nextLineIndex] = nextType;
				candidateCrimp.mvIndex = Crease::createMVPairIndex(centerType, nextType);

				answerCrimps.push_back(candidateCrimp);
				enumerate(assignments, nextRing, inverters);
				answerCrimps.pop_back();
			}

		public:
			FoldableMVEnumerationImpl(TOStream& os,
				mylib::IAnswerDetecter<TSet_Assignment>& ansDetecter,
				const mylib::IPruningSuggester<TSet_Assignment>& pruning)
				: outStream(os), ansDetecter(ansDetecter), pruning(pruning) {
			}

			~FoldableMVEnumerationImpl() {
			}

			mylib::EnumerationStats enumerate(const EncodablePatternBase& flap) {

				// setup enumeration bases

				const int lineCount = flap.count();
				const int placeCount = flap.capacity();

				LineIndexMapFactory mapFactory;
				map = mapFactory.create(flap);

				MVSymmetryDetecter<TSet_Assignment> symmetryDetecter(flap);
				auto inverters = symmetryDetecter.createInverterReferences();

				// setup foldability test
				using namespace std;
				sourceRing = helper.createRingArray(placeCount, lineCount, map);
				string assignments(lineCount, Crease::UNDEF);

				stats.clear();

				enumerate(assignments, sourceRing, inverters);

				return stats;
			}


		};

		//=====================================================================================

		template<
			typename TOStream,
			typename TSet_Assignment,
			bool needStats = true>
			class FoldableMVEnumeration : IMVEnumeration<TOStream, TSet_Assignment, needStats> {

			mylib::EnumerationStats stats;
			public:

				virtual const mylib::EnumerationStats& mvStats() {
					return stats;
				}

				virtual mylib::EnumerationStats enumerate(const EncodablePatternBase& flap, TOStream& os,
					mylib::IAnswerDetecter<TSet_Assignment>& ansDetecter,
					const mylib::IPruningSuggester<TSet_Assignment>& pruning = MaekawaPruning<TSet_Assignment>()) {

					FoldableMVEnumerationImpl<TOStream, TSet_Assignment, needStats> enumeration(os, ansDetecter, pruning);

					stats = enumeration.enumerate(flap);
					return stats;
				}


				// enumerates foldable cp assignments.
				virtual mylib::EnumerationStats enumerate(const EncodablePatternBase& flap, TOStream& os) {
					MaekawaTheorem<TSet_Assignment> ansDetecter;
					return this->enumerate(flap, os, ansDetecter);
				}

		};


	}
}