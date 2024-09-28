#pragma once

#include "ppcsearch.hpp"
#include "inverters.hpp"

#include "FlapPattern.hpp"
#include "CircularAlgorithm.hpp"
#include <cstdlib>
#include <unordered_set>
#include "Trie.hpp"

namespace enumeration {
	namespace origami {


		class FlapPatternString {
			std::string values;
			int count;

			//char* values;
			//void allocate(const int& count) {
			//	this->count = count;

			//	delete[] values;
			//	values = new char[size()];
			//}

			void allocate(const int& count) {
				this->count = count;

				values.resize(size());
			}

			inline int lineIndex(const int& position) const {
				return ((position + count) % count) * 2;
			}
			inline int angleIndex(const int& position) const {
				return lineIndex(position) + 1;
			}

		public:
			// value is -1
			static const char MINOR = -1;

			// value is -2
			static const char MAJOR = -2;
			//static const char UNDEF = 0;

			FlapPatternString(const int& count) {
				allocate(count);
			}

			FlapPatternString(const std::string& source) : values(source) {
				count = source.size() / 2;
			}

			FlapPatternString(const FlapPatternString& source) {
				count = source.count;
				values = source.values;

			}

			//~FlapPatternString() {
			//	delete[] values;
			//}

			void setLineType(const int& position, const char& type) {
				values[lineIndex(position)] = type;
			}

			void setAngle(const int& position, const char& angle) {
				values[angleIndex(position)] = angle;
			}

			const char& getLineType(const int& position) const {
				return values[lineIndex(position)];
			}

			const char& getAngle(const int& position) const {
				return values[angleIndex(position)];
			}

			/*
			* access to string.
			*/
			char& operator[](const int& index) {
				return values[index];
			}

			/*
			* access to string.
			*/
			const char& operator[](const int& index) const {
				return values[index];
			}

			// size of value array.
			int size() const {
				return count * 2;
			}

			std::string& asString() {
				return values;
			}

			// returns a readable string
			std::string toString() {
				using namespace std;
				stringstream ss;

				for (int i = 0; i < size(); i++) {
					switch (values[i])
					{
					case MAJOR:
						ss << '+';
						break;
					case MINOR:
						ss << '-';
						break;
					default:
						ss << (int)values[i];
						break;
					}
				}

				return ss.str();
			}

			void shiftHead(const int positionShift) {
				const auto length = positionShift * 2;
				values.append(values, 0, length);
				values.erase(0, length);
			}

		};

		class FlapPatternStringFactory {
		public:
			// returns like: M1M2M1M1... line type is filled with MAJOR
			FlapPatternString createCircularString(const EncodablePatternBase& flap) {
				auto& lineCount = flap.count();
				auto& placeCount = flap.capacity();

				FlapPatternString circularString(lineCount);

				int lastLineIndex = 0;
				int baseLineIndex = 0;
				int shrinkedIndex = 0;
				while (!flap.contains(baseLineIndex)) baseLineIndex++;

				lastLineIndex = baseLineIndex;

				for (u_int i = 0; i < placeCount; i++) {
					auto index = (i + baseLineIndex + 1) % placeCount;

					if (!flap.contains(index))
						continue;

					circularString.setLineType(shrinkedIndex, FlapPatternString::MAJOR);

					auto angle = (placeCount + index - lastLineIndex) % placeCount;
					circularString.setAngle(shrinkedIndex, angle);

					shrinkedIndex++;
					lastLineIndex = index;
				}

				using namespace std;
				//cout << "create circular string: " << circularString.toString() << endl;

				return circularString;
			}
		};

		template<typename TOStream, typename TSet_Assignment, bool needStats = true>
		class IMVEnumeration {

		public:

			virtual const mylib::EnumerationStats& mvStats() = 0;

			virtual mylib::EnumerationStats enumerate(const EncodablePatternBase& flap, TOStream& os,
				mylib::IAnswerDetecter<TSet_Assignment>& ansDetecter,
				const mylib::IPruningSuggester<TSet_Assignment>& pruning) = 0;


			// enumerates patterns satisfying maekawa theorem.
			virtual mylib::EnumerationStats enumerate(const EncodablePatternBase& flap, TOStream& os) = 0;
		};



		// key:   {0, 1, 2, ..., k}
		// value: items in the given set in increasing order.
		//        if i < j then map[i] < map[j]
		class LineIndexMapFactory {
		public:
			// return : a map from shrinked index to original index.
			template<typename TSet_Assignment>
			mylib::SharedArrayPointer<u_int> create(const TSet_Assignment& original) {
				auto placeCount = original.capacity();
				auto lineCount = original.count();

				mylib::SharedArrayPointer<u_int> map(lineCount);

				u_int keyIndex = 0;
				for (u_int i = 0; i < placeCount; i++) {
					if (original.contains(i)) {
						map[keyIndex] = i;
						keyIndex++;
					}
				}

				return map;
			}
		};

		template<typename TSet_Assignment>
		class MaekawaTheorem : public mylib::IAnswerDetecter<TSet_Assignment> {
		public:
			inline virtual bool isAnswer(const TSet_Assignment& pattern) {
				const int& lineCount = pattern.capacity();
				const int& minorCount = pattern.count();
				return minorCount == lineCount / 2 - 1;
			}
		};

		template<typename TSet_Assignment>
		class MaekawaPruning : public mylib::IPruningSuggester<TSet_Assignment> {
		public:
			inline virtual bool needPruning(const TSet_Assignment& pattern, int depth) const {

				const int& lineCount = pattern.capacity();

				return depth > lineCount / 2 - 1;

			}

		};


		template<typename TSet_Assignment>
		class MVSymmetryDetecter : public ppc::AbstractDuplicationDetecter<TSet_Assignment> {
			typedef enumeration::circular::RotationInverter RotationInverter;
			typedef enumeration::circular::MirrorInverter MirrorInverter;
			typedef enumeration::circular::MiddleMirrorInverter MiddleMirrorInverter;

			typedef enumeration::MappedIndexInverter<RotationInverter> MappedRotInverter;
			typedef enumeration::MappedIndexInverter<MirrorInverter> MappedMirInverter;
			typedef enumeration::MappedIndexInverter<MiddleMirrorInverter> MappedMidMirInverter;

			std::vector<MappedRotInverter> rotInverters;
			std::vector<MappedMirInverter > mirrorInverters;
			std::vector<MappedMidMirInverter > midMirrorInverters;

			// map mountains -> flap
			mylib::SharedArrayPointer<u_int> map;
			const u_int placeCount;
			const u_int lineCount;

			// selects meaningful inverters.
			void buildInverters(const EncodablePatternBase& flap) {

				for (u_int i = 1; i < placeCount; i++) {
					RotationInverter rotInv(i, placeCount);
					testAndAddMappedInverter(rotInv, rotInverters);
				}

				for (u_int i = 1; i < placeCount / 2 + 1; i++) {
					MirrorInverter mirrorInv(i, placeCount);
					testAndAddMappedInverter(mirrorInv, mirrorInverters);

					MiddleMirrorInverter midMirrorInv(i - 1, placeCount);
					testAndAddMappedInverter(midMirrorInv, midMirrorInverters);
				}
			}

			template <typename TInverter, typename TVector>
			inline void testAndAddMappedInverter(const TInverter& inv, TVector& inverters) {
				enumeration::MappedIndexInverter<TInverter> mappedInv(inv, placeCount, map, lineCount);
				if (mappedInv.canInvert()) {
					inverters.push_back(mappedInv);
				}
			}

		public:
			MVSymmetryDetecter(const EncodablePatternBase& flap) : lineCount(flap.count()), placeCount(flap.capacity()) {
				LineIndexMapFactory mapFactory;
				map = mapFactory.create(flap);
				buildInverters(flap);
			}

			virtual ~MVSymmetryDetecter() {
			}

			inline virtual bool hasGenerated(const TSet_Assignment& pattern, const int elemEnd, const int prefixTail) const {
				if (this->knownModificationExistsFor(pattern, elemEnd, prefixTail, rotInverters)) {
					return true;
				}

				if (this->knownModificationExistsFor(pattern, elemEnd, prefixTail, mirrorInverters)) {
					return true;
				}

				if (this->knownModificationExistsFor(pattern, elemEnd, prefixTail, midMirrorInverters)) {
					return true;
				}

				return false;
			}

			std::vector<enumeration::IInverter*> createInverterReferences() {
				using namespace std;

				vector<enumeration::IInverter*> references;
				references.reserve(rotInverters.size() + mirrorInverters.size() + midMirrorInverters.size());

				auto refCopy = [&](auto& inverters) {
					for (auto& inv : inverters) {
						references.push_back(&inv);
					}
				};

				refCopy(rotInverters);
				refCopy(mirrorInverters);
				refCopy(midMirrorInverters);

				return references;
			}
		};

//-----------------------------------------------------------------------------------------------------------------------------

		template<typename TOStream, typename TSet_Assignment, bool needStats = true>
		class MVLSLEnumeration : public IMVEnumeration<TOStream, TSet_Assignment, needStats> {
			mylib::EnumerationStats stats;

			class Implementation {
				mylib::EnumerationStats stats_;
				const ppc::PPCSearchTool<TSet_Assignment>& tool;

			public:
				Implementation(
					const ppc::PPCSearchTool<TSet_Assignment>& tool)
					:tool(tool)
				{}

				void enumerate(FlapPatternString& circularString, TSet_Assignment& assignments,
					const int candBegin, const int candEnd, int prefixTail, const int depth, TOStream& os) {

					using namespace std;
					//cout << "enumerate: " << circularString.toString() << endl;

					if (tool.hasGenerated(assignments, candBegin, prefixTail))
						return;

					if (tool.needPruning(assignments, depth))
						return;

					if (needStats)
						stats_.validCallCount++;

					if (tool.isAnswer(assignments)) {
						if (needStats)
							stats_.answerCount++;

						// cout << "ANSWER!" << endl;
						os << assignments;
						
						return;
					}

					auto nextPrefixTail = candBegin - 1;

					for (auto cand = candBegin; cand < candEnd; cand++) {
						// Large-Small-Large theorem
						// MINOR: V, MAJOR: M
						auto prev = cand - 1;
						auto prevprev = cand -2;
						if (circularString.getAngle(prevprev) > circularString.getAngle(prev) &&
							circularString.getAngle(cand) > circularString.getAngle(prev)) {

							// theorem holds: fix as VM.
							if (circularString.getLineType(prev) == FlapPatternString::MINOR) {
								continue;
							}
						}

						// thoerem does not hold or trying MV
						assignments.add(cand);
						circularString.setLineType(cand, FlapPatternString::MINOR);

						enumerate(circularString, assignments, cand + 1, candEnd,
							nextPrefixTail, depth + 1, os);

						assignments.remove(cand);
						circularString.setLineType(cand, FlapPatternString::MAJOR);
					}
				}

				mylib::EnumerationStats stats() {
					return stats_;
				}


			};


		public:
			MVLSLEnumeration() {}

			virtual const mylib::EnumerationStats& mvStats() {
				return stats;
			}

			virtual mylib::EnumerationStats enumerate(const EncodablePatternBase& flap, TOStream& os,
				mylib::IAnswerDetecter<TSet_Assignment>& ansDetecter,
				const mylib::IPruningSuggester<TSet_Assignment>& pruning = MaekawaPruning<TSet_Assignment>()) {

				MVSymmetryDetecter<TSet_Assignment> symmDetecter(flap);
				ppc::PPCSearchTool<TSet_Assignment> tool;

				tool.setDuplication(symmDetecter);
				tool.setAnswer(ansDetecter);
				tool.setPruning(pruning);

				const int lineCount = flap.count();

				FlapPatternStringFactory stringFactory;
				auto circularString = stringFactory.createCircularString(flap);

				TSet_Assignment seed(lineCount);

				Implementation search(tool);

				search.enumerate(circularString, seed, 0, lineCount, -1, 0, os);

				stats = search.stats();

				return stats;
			}

			virtual mylib::EnumerationStats enumerate(const EncodablePatternBase& flap, TOStream& os) {
				MaekawaTheorem<TSet_Assignment> ansDetecter;

				return this->enumerate(flap, os, ansDetecter);
			}
		};


//-----------------------------------------------------------------------------------------------------------------------------

		template<typename TOStream, typename TSet_Assignment, bool needStats = true>
		class ExtendedMVLSLEnumeration : public IMVEnumeration<TOStream, TSet_Assignment, needStats> {
			mylib::EnumerationStats stats;

			class Implementation {
				mylib::EnumerationStats stats_;
				const ppc::PPCSearchTool<TSet_Assignment>& tool;
				const std::vector<std::pair<int, int> >& equalAngleIntervals;

			public:
				Implementation(
					const ppc::PPCSearchTool<TSet_Assignment>& tool, const std::vector<std::pair<int, int> >& intervals)
					:tool(tool), equalAngleIntervals(intervals)
				{}

				void enumerate(FlapPatternString& circularString, TSet_Assignment& assignments,
					const int candBegin, const int candEnd, int prefixTail, const int depth, TOStream& os) {

					using namespace std;
					//cout << "enumerate: " << circularString.toString() << endl;

					if (tool.hasGenerated(assignments, candBegin, prefixTail))
						return;

					if (tool.needPruning(assignments, depth))
						return;

					if (needStats)
						stats_.validCallCount++;

					if (tool.isAnswer(assignments)) {
						if (needStats)
							stats_.answerCount++;

						// cout << "ANSWER!" << endl;
						os << assignments;

						return;
					}

					auto nextPrefixTail = candBegin - 1;

					for (auto cand = candBegin; cand < candEnd; cand++) {
						// Extended Large-Small-Large theorem
						// MINOR: V, MAJOR: M

						int eqAngleHead = equalAngleIntervals[cand].first;
						int eqAngleEnd = equalAngleIntervals[cand].second;

						if (eqAngleEnd != -1) {
							int mvCount = 0;
							for (int i = eqAngleHead; i < eqAngleEnd; i++) {
								mvCount += (circularString.getLineType(i) == FlapPatternString::MINOR) ? 1: -1;
							}


							if ((eqAngleEnd - eqAngleHead) % 2 == 0) {
								if (mvCount == 0) {
									continue;
								}
							}
							else if (mvCount == 1) {
								continue;
							}
						}

						// theorem does not hold
						assignments.add(cand);
						circularString.setLineType(cand, FlapPatternString::MINOR);

						enumerate(circularString, assignments, cand + 1, candEnd,
							nextPrefixTail, depth + 1, os);

						assignments.remove(cand);
						circularString.setLineType(cand, FlapPatternString::MAJOR);
					}
				}

				mylib::EnumerationStats stats() {
					return stats_;
				}


			};

			// returnedHash[i] = <begin() of equal angle interval, end() of equal angle interval> where i belongs to. 
			std::vector<std::pair<int, int> > createEqualAngleIntervalHash(
				const FlapPatternString& circularString, const int lineCount) {

				std::vector<std::pair<int, int> > intervalHash(lineCount, std::make_pair(0,-1));

				for (int i = 0; i < lineCount; i++) {
					if (circularString.getAngle(i - 1) <= circularString.getAngle(i)) {
						continue;
					}
					int j = i + 1;
					for (; j < lineCount; j++) {
						if (circularString.getAngle(j - 1) != circularString.getAngle(j)) {
							break;
						}
					}
					if (circularString.getAngle(j) > circularString.getAngle(j - 1)) {
						const auto interval = std::make_pair(i, j + 1);
						for (int cand = interval.first; cand < interval.second; cand++) {
							intervalHash[cand] = interval;
						}
					}
				}

				return intervalHash;
			}

		public:
			ExtendedMVLSLEnumeration() {}

			virtual const mylib::EnumerationStats& mvStats() {
				return stats;
			}

			virtual mylib::EnumerationStats enumerate(const EncodablePatternBase& flap, TOStream& os,
				mylib::IAnswerDetecter<TSet_Assignment>& ansDetecter,
				const mylib::IPruningSuggester<TSet_Assignment>& pruning = MaekawaPruning<TSet_Assignment>()) {

				MVSymmetryDetecter<TSet_Assignment> symmDetecter(flap);
				ppc::PPCSearchTool<TSet_Assignment> tool;

				tool.setDuplication(symmDetecter);
				tool.setAnswer(ansDetecter);
				tool.setPruning(pruning);

				const int lineCount = flap.count();

				FlapPatternStringFactory stringFactory;
				auto circularString = stringFactory.createCircularString(flap);

				auto eqAngleIntervals = createEqualAngleIntervalHash(circularString, lineCount);

				TSet_Assignment seed(lineCount);

				Implementation search(tool, eqAngleIntervals);

				search.enumerate(circularString, seed, 0, lineCount, -1, 0, os);

				stats = search.stats();

				return stats;
			}

			virtual mylib::EnumerationStats enumerate(const EncodablePatternBase& flap, TOStream& os) {
				MaekawaTheorem<TSet_Assignment> ansDetecter;

				return this->enumerate(flap, os, ansDetecter);
			}
		};

//-----------------------------------------------------------------------------------------------------------------------------

		template<typename TOStream, typename TSet_Assignment, bool needStats = true>
		class MVEnumeration : public IMVEnumeration<TOStream, TSet_Assignment, needStats> {
			mylib::EnumerationStats stats;

		public:
			MVEnumeration() {}

			virtual const mylib::EnumerationStats& mvStats() {
				return stats;
			}

			virtual mylib::EnumerationStats enumerate(const EncodablePatternBase& flap, TOStream& os,
				mylib::IAnswerDetecter<TSet_Assignment>& ansDetecter,
				const mylib::IPruningSuggester<TSet_Assignment>& pruning = MaekawaPruning<TSet_Assignment>()) {

				MVSymmetryDetecter<TSet_Assignment> symmDetecter(flap);
				ppc::PPCSearchTool<TSet_Assignment> tool;

				tool.setDuplication(symmDetecter);
				tool.setAnswer(ansDetecter);
				tool.setPruning(pruning);

				const int lineCount = flap.count();
				TSet_Assignment seed(lineCount);
				ppc::ExtendedPPCSearch<TSet_Assignment, TOStream, needStats> search(os, tool);

				search.enumerate(seed, 0, lineCount, -1, 0);

				stats = search.stats();

				return stats;
			}

			virtual mylib::EnumerationStats enumerate(const EncodablePatternBase& flap, TOStream& os) {
				MaekawaTheorem<TSet_Assignment> ansDetecter;

				return this->enumerate(flap, os, ansDetecter);
			}
		};

//-----------------------------------------------------------------------------------------------------------------------------

		// cache uses too much memory.
		template<typename TOStream, typename TSet_Assignment, bool needStats = true>
		class LinearMVEnumeration : public IMVEnumeration<TOStream, TSet_Assignment, needStats> {
			mylib::EnumerationStats stats;

			class Implementation {
				mylib::EnumerationStats stats_;
				mylib::CircularAlgorithm<char> circularAlgorithm;
				mylib::Trie cache;
				//mylib::PatriciaTrie cache;

				mylib::IAnswerDetecter<TSet_Assignment>& ansDetecter;
				const mylib::IPruningSuggester<TSet_Assignment>& pruning;

				std::string canonicalTemporary;

			public:
				Implementation(
					mylib::IAnswerDetecter<TSet_Assignment>& ansDetecter,
					const mylib::IPruningSuggester<TSet_Assignment>& pruning,
					const int& length, const int& maxAlphabet)
					:ansDetecter(ansDetecter), pruning(pruning), canonicalTemporary(length, 0), 
					cache(maxAlphabet + 1 + 2, -2)
				{}

				void enumerate(FlapPatternString& circularString, TSet_Assignment& assignments,
					const int& candBegin, const int& candEnd, 
					const int& depth, TOStream& os) {

					using namespace std;
					//cout << "enumerate: " << circularString.toString() << endl;

					circularAlgorithm.createCanonicalOnSymmetry(
						circularString, canonicalTemporary, canonicalTemporary.size());
					if (cache.contains(canonicalTemporary)) {
						return;
					}
					cache.insert(canonicalTemporary);

					if (pruning.needPruning(assignments, depth)) {
						return;
					}

					if (needStats)
						stats_.validCallCount++;

					if (ansDetecter.isAnswer(assignments)) {
						if (needStats)
							stats_.answerCount++;

						// cout << "ANSWER!" << endl;
						os << assignments;
						return;
					}

					for (int cand = candBegin; cand < candEnd; cand++) {
						// Large-Small-Large theorem
						auto prev = cand - 1;
						auto prevprev = cand - 2;
						if (circularString.getAngle(prevprev) > circularString.getAngle(prev) &&
							circularString.getAngle(cand) > circularString.getAngle(prev)) {

							// theorem holds: fix as VM.
							if (circularString.getLineType(prev) == FlapPatternString::MINOR) {
								continue;
							}
						}

						// thoerem does not hold or trying MV
						assignments.add(cand);
						circularString.setLineType(cand, FlapPatternString::MINOR);
							
						enumerate(circularString, assignments, cand + 1, candEnd,
						depth + 1, os);

						assignments.remove(cand);
						circularString.setLineType(cand, FlapPatternString::MAJOR);
					}
				}

				mylib::EnumerationStats stats() {
					return stats_;
				}


			};



		public:
			LinearMVEnumeration() {}

			virtual const mylib::EnumerationStats& mvStats() {
				return stats;
			}

			// run algorithm
			virtual mylib::EnumerationStats enumerate(
				const EncodablePatternBase& flap, TOStream& os,
				mylib::IAnswerDetecter<TSet_Assignment>& ansDetecter,
				const mylib::IPruningSuggester<TSet_Assignment>& pruning = MaekawaPruning<TSet_Assignment>()) {

				FlapPatternStringFactory stringFactory;
				auto circularString = stringFactory.createCircularString(flap);

				TSet_Assignment seed(flap.count());
				u_int candEnd = seed.capacity();

				char maxAlphabet = -2;
				//char maxAlphabet = flap.capacity() / 2;
				for (int i = 0; i < circularString.size(); i++) {
					maxAlphabet = std::max(maxAlphabet, circularString[i]);
				}

				Implementation search(ansDetecter, pruning, circularString.size(), maxAlphabet);
				search.enumerate(circularString, seed, 0, seed.capacity(), 0, os);
				stats = search.stats();
				return stats;
			}

			virtual mylib::EnumerationStats enumerate(const EncodablePatternBase& flap, TOStream& os) {
				MaekawaTheorem<TSet_Assignment> ansDetecter;

				return this->enumerate(flap, os, ansDetecter);
			}
		};


	}
}
