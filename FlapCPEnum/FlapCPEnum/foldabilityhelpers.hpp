#pragma once
#include <algorithm>
#include <vector>
#include <deque>
#include <sstream>
#include <set>

#include "RingList.hpp"
#include "BitSet.hpp"
#include "SharedArrayPointer.hpp"

namespace enumeration {
	namespace origami {

		class LineGap {
		private:
			void substitute(const LineGap& source) {
				angleToNext = source.angleToNext;
				lineType = source.lineType;
				//lineID = source.lineID;
			}

		public:
			u_int angleToNext;
			char lineType;
			//int lineID;

			LineGap() {}
			LineGap(const u_int& angle, const char& lineType)
				:angleToNext(angle), lineType(lineType)
			{}

			LineGap(const LineGap& source) {
				substitute(source);
			}

			LineGap& operator=(const LineGap& right) {
				substitute(right);
				return *this;
			}

			inline bool operator==(const LineGap& right) const {
				return angleToNext == right.angleToNext && lineType == right.lineType;
			}

			// line type first.
			bool operator<(const LineGap& right) const {
				if (lineType != right.lineType) {
					return lineType < right.lineType;
				}
				return angleToNext < right.angleToNext;
			}

			std::string toString() {
				using namespace std;

				stringstream ss;
				ss << lineType << angleToNext;

				return ss.str();
			}
		};

		struct Crease {
			static const char MAJOR = 'M';
			static const char MINOR = 'V';
			static const char UNDEF = '@';

			static int createMVPairIndex(const char& center, const char& next) {
				if (center == next || center == UNDEF || next == UNDEF)
					return -1;
				//throw std::invalid_argument(std::string("given values are: ") + center + ", " + next);

				if (center == Crease::MAJOR && next == Crease::MINOR)
					return 0;

				return 1;
			}

			static int toIndex(const char& crease) {
				if (crease == UNDEF)
					return 2;
				if (crease == MAJOR)
					return 0;

				// assuming MINOR
				return 1;
			}
		};

		class FoldabilityRingArrayHelper {

		public:
			bool isMinimalAngle(const LineGap& prev, const LineGap& center, const LineGap& next) const {
				return prev.angleToNext >= center.angleToNext && next.angleToNext >= center.angleToNext;
			}

			bool isMinimalToBeFolded(const LineGap& prev, const LineGap& center, const LineGap& next) const {
				return isMinimalAngle(prev, center, next) && center.lineType != next.lineType;
			}

			bool isMinimalToBeFolded(const mylib::RingArrayList<LineGap>& ring, const u_int& index) const {
				auto& prev = ring.prevOf(index);
				auto& next = ring.nextOf(index);
				auto& center = ring[index];

				return isMinimalToBeFolded(prev, center, next);
			}

			bool isMinimalAngle(const mylib::RingArrayList<LineGap>& ring, const u_int& index) const {
				auto& prev = ring.prevOf(index);
				auto& next = ring.nextOf(index);
				auto& center = ring[index];

				return isMinimalAngle(prev, center, next);
			}

			template <typename TIntStack>
			TIntStack findMinimalIndices(const mylib::RingArrayList<LineGap>& ring, bool angleOnly = false) const {
				TIntStack minimals;

				int index = ring.peekHeadIndex();

				for (u_int i = 0; i < ring.count(); i++) {
					if (angleOnly) {
						if (isMinimalAngle(ring, index)) {
							minimals.push_back(index);
						}
					}
					else if (isMinimalToBeFolded(ring, index)) {
						minimals.push_back(index);
					}
					index = ring.nextIndexOf(index);
				}
				return minimals;
			}

			// return : prev. index which holds merged value.
			// this operation DOES NOT check the line type condition.
			int foldPartially(mylib::RingArrayList<LineGap>& ring, const int& index) const {

				auto& center = ring[index];
				auto& next = ring.nextOf(index);

				// prev. node remains in the ring
				auto prevIndex = ring.prevIndexOf(index);
				// remove folded area
				ring.remove(ring.nextIndexOf(index));
				ring.remove(index);


				// merge the folded area to prev. node
				ring[prevIndex].angleToNext += next.angleToNext - center.angleToNext;

				return prevIndex;
			}

			void reverseCrease(mylib::RingArrayList<LineGap>& ring, const int& index) const {
				auto& crease = ring[index].lineType;
				if (crease == Crease::MAJOR)
					crease = Crease::MINOR;
				else crease = Crease::MAJOR;
			}

			template<typename TSet>
			void reverseCrease(TSet& assignments, const int& index) const {
				if (assignments.contains(index))
					assignments.remove(index);
				else
					assignments.add(index);
			}

			void reverseCrease(char& crease) const {
				if (crease == Crease::MAJOR)
					crease = Crease::MINOR;
				else crease = Crease::MAJOR;
			}

			template<typename TSet>
			void reverseCrease(mylib::RingArrayList<LineGap>& ring, TSet& assignments, const int& index) const {
				reverseCrease(assignments, index);
				reverseCrease(ring, index);
			}

			mylib::RingArrayList<LineGap> createRingArray(
				const u_int& placeCount,
				const u_int& lineCount,
				const mylib::SharedArrayPointer<u_int>& map) const {

				mylib::RingArrayList<LineGap> ring(placeCount);

				for (u_int i = 0; i < lineCount; i++) {
					LineGap lineGap;
					lineGap.lineType = Crease::UNDEF;
					lineGap.angleToNext = (placeCount + map[(i + 1) % lineCount] - map[i]) % placeCount;

					ring.set(i, lineGap);
				}

				ring.makeLinks();

				return ring;

			}

			template<typename TSet_Assignment>
			mylib::RingArrayList<LineGap> createRingArray(
				const u_int& placeCount,
				const u_int& lineCount,
				const TSet_Assignment& assignments,
				const mylib::SharedArrayPointer<u_int>& map) const {

				mylib::RingArrayList<LineGap> ring(placeCount);

				for (u_int i = 0; i < lineCount; i++) {
					LineGap lineGap;
					lineGap.lineType = (assignments.contains(i)) ? Crease::MAJOR : Crease::MINOR;
					lineGap.angleToNext = (placeCount + map[(i + 1) % lineCount] - map[i]) % placeCount;

					ring.set(i, lineGap);
				}

				ring.makeLinks();

				return ring;

			}

			template<char FLAT, char MAJOR, char MINOR, typename TCharArray>
			mylib::RingArrayList<LineGap> createRingArray(const TCharArray& assignments, const u_int& placeCount){

				mylib::RingArrayList<LineGap> ring(placeCount);

				// find the first line.
				u_int firstLineIndex = 0;
				while (assignments[firstLineIndex] == FLAT) 
					firstLineIndex++;

				u_int i_ring = 0;
				for (u_int i = 0; i < placeCount;) {
					auto makeIndex = [&](const u_int& i) {
						return (firstLineIndex + i) % placeCount; 
					};

					// create index values
					auto index = makeIndex(i);
					u_int angle = 1;
					while (assignments[makeIndex(i + angle)] == FLAT)
						angle++;
					
					// set to ring
					LineGap lineGap;
					lineGap.lineType = (assignments[index] == MAJOR) ? Crease::MAJOR : Crease::MINOR;
					lineGap.angleToNext = angle;

					ring.set(i_ring, lineGap);

					i_ring++;
					i += angle;
				}

				ring.makeLinks();

				return ring;
			}

			void assignToRing(const std::string& assignments, mylib::RingArrayList<LineGap>& ring) {
				for (u_int i = 0; i < assignments.size(); i++) {
					ring[i].lineType = assignments[i];
				}
			}

			/* return: index (>=0) of Mountain/Valley line if such line exists.
			 *         if there is no Mountain/Valley, this function returns the head index whose line type is UNDEF.
			*/
			u_int findFirstMVLineIndex(const mylib::RingArrayList<LineGap>& ring) const {
				auto index = ring.peekHeadIndex();
				for (u_int i = 0; i < ring.count(); i++) {
					const auto& item = ring[index];
					if (item.lineType != Crease::UNDEF)
						break;
					index = ring.nextIndexOf(index);
				}
				return index;
			}

			bool reduceUnassignedLines(mylib::RingArrayList<LineGap>& ring) const {

				// build a MV ring
				// - find the first M/V line
				auto index = findFirstMVLineIndex(ring);
				if (ring[index].lineType == Crease::UNDEF) {
					return false;
				}

				// - pick up M/V, merging angles of UNDEF
				int lineCount = ring.count();
				for (int i = 0; i < lineCount;) {
					auto targetIndex = index;
					auto& targetLine = ring[index];

					index = ring.nextIndexOf(index);
					i++;

					u_int angle = 0;
					while (ring[index].lineType == Crease::UNDEF && i < lineCount) {
						angle += ring[index].angleToNext;
						auto nextIndex = ring.nextIndexOf(index);
						ring.remove(index);
						index = nextIndex;
						i++;
					}
					targetLine.angleToNext += angle;
				}

				return true;
			}

			template<typename TInverter>
			mylib::RingArrayList<LineGap> invertRing(
				const mylib::RingArrayList<LineGap>& sourceRing, const TInverter& inverter) const {

				mylib::RingArrayList<LineGap> ring(sourceRing.size());
				auto index = sourceRing.peekHeadIndex();

				for (u_int i = 0; i < sourceRing.count();) {
					const auto inv_index = inverter(index);

					//if (item.lineType != Crease::UNDEF)
					ring.set(inv_index, sourceRing[index]);

					if (inverter.reversesOrder()) {
						ring[inv_index].angleToNext = sourceRing.prevOf(index).angleToNext;
					}

					index = sourceRing.nextIndexOf(index);
					i++;

				}
				ring.makeLinks();

				return ring;
			}

		};


		class MinimalAngleIndexManager {
			std::deque<int> minimals;
			mylib::BitSet isInMinimals;
			FoldabilityRingArrayHelper helper;

		public:

			MinimalAngleIndexManager(
				const mylib::RingArrayList<LineGap>& ring, bool angleOnly = false) : isInMinimals(ring.count()) {

				minimals = helper.findMinimalIndices<std::deque<int> >(ring, angleOnly);

				//initialize
				for (auto& index : minimals) {
					isInMinimals.add(index);
				}
			}

			void push(const int& index) {
				minimals.push_back(index);
				isInMinimals.add(index);
			}

			bool pushIfMinimalAngle(mylib::RingArrayList<LineGap>& ring, const int& index) {

				if (helper.isMinimalAngle(ring, index) && !contains(index)) {
					push(index);
					return true;
				}

				return false;
			}

			bool pushIfMinimalToBeFolded(mylib::RingArrayList<LineGap>& ring, const int& index) {

				if (helper.isMinimalToBeFolded(ring, index) && !contains(index)) {
					push(index);
					return true;
				}

				return false;
			}


			int pop() {
				auto index = minimals.back();
				minimals.pop_back();
				isInMinimals.remove(index);

				return index;
			}

			bool empty() const {
				return minimals.empty();
			}

			bool contains(const int& index) const {
				return isInMinimals.contains(index);
			}

			MinimalAngleIndexManager& operator=(const MinimalAngleIndexManager& right) {
				minimals = right.minimals;
				isInMinimals = right.isInMinimals;
				return *this;
			}

			std::string toString() {
				using namespace std;
				stringstream ss;

				for (auto& index : minimals) {
					ss << index << "(" << isInMinimals.contains(index) << ") ";
				}
				return ss.str();
			}
		};



		class Crimp {
		public:
			u_int centerLineIndex, nextLineIndex;
			u_int angle;
			int mvIndex;

			Crimp() {}

			Crimp(const Crimp& source) :
				angle(source.angle),
				centerLineIndex(source.centerLineIndex), nextLineIndex(source.nextLineIndex),
				mvIndex(source.mvIndex)
			{
			}

			Crimp(
				const u_int& angle,
				const u_int& centerIndex, const u_int& nextIndex, const int& mvIndex
			) :
				angle(angle), centerLineIndex(centerIndex), nextLineIndex(nextIndex), mvIndex(mvIndex)
			{
			}



			bool operator<(const Crimp& right) const {
				if (angle != right.angle)
					return angle < right.angle;

				if (centerLineIndex != right.centerLineIndex)
					return centerLineIndex < right.centerLineIndex;

				if (nextLineIndex != right.nextLineIndex)
					return nextLineIndex < right.nextLineIndex;

				return mvIndex < right.mvIndex;
			}
			bool operator>(const Crimp& right) const {
				if (angle != right.angle)
					return angle > right.angle;

				if (centerLineIndex != right.centerLineIndex)
					return centerLineIndex > right.centerLineIndex;

				if (nextLineIndex != right.nextLineIndex)
					return nextLineIndex > right.nextLineIndex;

				return mvIndex > right.mvIndex;
			}

			bool operator==(const Crimp& right) const {
				return angle == right.angle &&
					centerLineIndex == right.centerLineIndex &&
					nextLineIndex == right.nextLineIndex &&
					mvIndex == right.mvIndex
					;
			}

			std::string toString() const {
				using namespace std;
				stringstream ss;

				ss << "(l1:" << centerLineIndex << ", l2:" << nextLineIndex << ",mv:" << mvIndex
					<< ")";

				return ss.str();
			}
		};

		class CrimpHelper {
		public:
			Crimp createCrimp(const mylib::RingArrayList<LineGap>& ring, const u_int& index) {
				const auto& nextIndex = ring.nextIndexOf(index);
				return Crimp(
					ring[index].angleToNext,
					index,
					nextIndex,
					Crease::createMVPairIndex(ring[index].lineType, ring[nextIndex].lineType));
			}

			template<typename TInverter>
			Crimp createCrimp(const mylib::RingArrayList<LineGap>& ring, const u_int& index, const TInverter& inverter) {
				auto inv_index = inverter(index);
				auto inv_nextIndex = inverter(ring.nextIndexOf(index));

				if (inverter.reversesOrder())
					std::swap(inv_index, inv_nextIndex);

				return Crimp(
					ring[index].angleToNext,
					inv_index,
					inv_nextIndex,
					Crease::createMVPairIndex(ring[inv_index].lineType, ring[inv_nextIndex].lineType));
			}


			bool insertCrimpIfMinimal(const mylib::RingArrayList<LineGap>& ring, const Crimp& crimp,
				const bool& checkLineType,
				std::set<Crimp>& crimps) {
				FoldabilityRingArrayHelper helper;

				const auto& index = crimp.centerLineIndex;
				const auto& nextIndex = crimp.nextLineIndex;
				if ((checkLineType && helper.isMinimalToBeFolded(ring, index)) ||
					(!checkLineType && helper.isMinimalAngle(ring, index))) {

					crimps.insert(crimp);

					return true;
				}
				return false;

			}

			template<typename TInverter>
			bool insertCrimpByIndexIfMinimal(const mylib::RingArrayList<LineGap>& ring, const u_int& index,
				const bool& checkLineType, const TInverter& inverter,
				std::set<Crimp>& crimps) {

				return insertCrimpIfMinimal(ring, createCrimp(ring, index, inverter), checkLineType, crimps);
			}
			bool insertCrimpByIndexIfMinimal(const mylib::RingArrayList<LineGap>& ring, const u_int& index,
				const bool& checkLineType,
				std::set<Crimp>& crimps) {

				return insertCrimpIfMinimal(ring, createCrimp(ring, index), checkLineType, crimps);
			}

			bool pushBackCrimpIfMinimal(const mylib::RingArrayList<LineGap>& ring, const Crimp& crimp,
				const bool& checkLineType,
				std::vector<Crimp>& crimps) {
				FoldabilityRingArrayHelper helper;

				const auto& index = crimp.centerLineIndex;
				const auto& nextIndex = ring.nextIndexOf(index);

				if (checkLineType && helper.isMinimalToBeFolded(ring, index) ||
					!checkLineType && helper.isMinimalAngle(ring, index)) {
					crimps.push_back(crimp);
					return true;
				}
				return false;
			}

			bool pushBackCrimpIfMinimal(const mylib::RingArrayList<LineGap>& ring, const u_int& index,
				const bool& checkLineType,
				std::vector<Crimp>& crimps) {

				return pushBackCrimpIfMinimal(ring, createCrimp(ring, index), checkLineType, crimps);
			}
		};

		class MinimalityChecker {

		public:

			template <typename CrimpCollection, typename FuncIntToInt >
			bool equivalenceHasCreated(const std::string& assignments, const CrimpCollection& finishedCrimps, const FuncIntToInt& inverter) const {
				for (const Crimp& crimp : finishedCrimps) {
					auto centerIndex = inverter(crimp.centerLineIndex);
					auto nextIndex = inverter(crimp.nextLineIndex);
					auto mvIndex = crimp.mvIndex;

					if (centerIndex == crimp.centerLineIndex && nextIndex == crimp.nextLineIndex ||
						centerIndex == crimp.nextLineIndex && nextIndex == crimp.centerLineIndex) {
						continue;
					}

					//if (inverter.reversesOrder())
					//	std::swap(centerIndex, nextIndex);

					if (hasCreated(assignments, centerIndex, nextIndex, mvIndex))
						return true;
				}
				return false;
			}

			template <typename CrimpCollection, typename FuncIntToIntCollection >
			bool hasCreatedFor(const std::string& assignments,
				const CrimpCollection& finishedCrimps, const FuncIntToIntCollection& inverters) const {

				if (hasCreated(assignments, finishedCrimps))
					return true;

				for (auto& inverter : inverters) {
					using namespace std;
					//cout << "test duplication: " << inverter->toString() << endl;
					if (equivalenceHasCreated(assignments, finishedCrimps, *inverter))
						return true;
				}

				return false;
			}

			//==============================================================================
			// test crimps

			template<typename CrimpCollection, typename InverterCollection>
			bool isMinimalCrimpSequence(const CrimpCollection& crimps, const mylib::RingArrayList<LineGap>& r,
				const InverterCollection& inverters) const {

				enumeration::DummyInverter dummy;
				if (!isLessThanEqual(crimps, r, dummy))
					return false;

				for (const auto& inverter : inverters) {
					if (!isLessThanEqual(crimps, r, *inverter))
						return false;
				}

				return true;
			}

			bool ringContainsCrimp(const mylib::RingArrayList<LineGap>& ring, const Crimp& crimp) const {
				return ring.exists(crimp.centerLineIndex) && ring.exists(crimp.nextLineIndex)
					&& ring[crimp.centerLineIndex].angleToNext == crimp.angle;
			}


			std::vector<Crimp> createMinimalSequence(const mylib::RingArrayList<LineGap>& sourceRing) const {
				using namespace std;
				set<Crimp> possibleCrimps;
				CrimpHelper crimpHelper;
				FoldabilityRingArrayHelper ringHelper;

				vector<Crimp> minSequence;

				auto ring = sourceRing;

				//convert to possible crimps
				auto index = ring.peekHeadIndex();
				for (u_int i = 0; i < ring.count(); i++) {
					crimpHelper.insertCrimpByIndexIfMinimal(ring, index, true, possibleCrimps);
					index = ring.nextIndexOf(index);
				}

				//for (auto& c : possibleCrimps)
				//	cout << "initial possible crimp: " << c.toString() << endl;


				int i = 0;
				while (ring.count() > 2 && !possibleCrimps.empty()) {
					// pick up the smallest crimp
					const auto minCrimp = *(possibleCrimps.begin());
					possibleCrimps.erase(possibleCrimps.begin());

					index = minCrimp.centerLineIndex;

					// cout << "try crimp: " << minCrimp.toString() << endl;


					if (!ringContainsCrimp(ring, minCrimp))
						continue;

					if (ringHelper.isMinimalToBeFolded(ring, index)) {
						//cout << "do crimp: " << minCrimp.toString() << endl;
					}
					else continue;

					minSequence.push_back(minCrimp);

					auto mergedIndex = ringHelper.foldPartially(ring, index);

					// update the possible actions
					crimpHelper.insertCrimpByIndexIfMinimal(ring, mergedIndex, true, possibleCrimps);
					crimpHelper.insertCrimpByIndexIfMinimal(ring, ring.nextIndexOf(mergedIndex), true, possibleCrimps);
					crimpHelper.insertCrimpByIndexIfMinimal(ring, ring.prevIndexOf(mergedIndex), true, possibleCrimps);

					i++;
				}

				//cout << "this is canonical!" << endl;
				return minSequence;
			}


			template<typename CrimpCollection, typename TInverter>
			bool isLessThanEqual(const CrimpCollection& crimps, const mylib::RingArrayList<LineGap>& sourceRing,
				const TInverter& inverter) const {
				using namespace std;
				set<Crimp> possibleCrimps;
				CrimpHelper crimpHelper;
				FoldabilityRingArrayHelper ringHelper;


				//cout << "canonical crimps?" << endl;

				// build a MV ring
				mylib::RingArrayList<LineGap> ring(sourceRing);
				if (!ringHelper.reduceUnassignedLines(ring)) // MV line exists?
					return true;
				// invert the ring
				ring = ringHelper.invertRing(ring, inverter);

				//cout << "without UNDEF (" << inverter.toString() << "):" << endl
				//	<< ring.toString() << endl;

				//convert to possible crimps
				auto index = ring.peekHeadIndex();
				for (u_int i = 0; i < ring.count(); i++) {
					crimpHelper.insertCrimpByIndexIfMinimal(ring, index, true, possibleCrimps);
					index = ring.nextIndexOf(index);
				}

				// conpare given seq. to the minimal seq. for the ring
				//auto minCrimps = createMinimalSequence(ring);
				//if (minCrimps.size() != crimps.size())
				//	return false;
				//for (int i = 0; i < crimps.size(); i++) {
				//		if (crimps[i] > minCrimps[i])
				//			return false;
				//		if (crimps[i] <  minCrimps[i])
				//			break;
				//}

				//------
				// a little fast but hard to test

				int i = 0;
				while (ring.count() > 2 && !possibleCrimps.empty()) {
					// pick up the smallest crimp
					const auto minCrimp = *(possibleCrimps.begin());
					possibleCrimps.erase(possibleCrimps.begin());

					index = minCrimp.centerLineIndex;

					if (!ringContainsCrimp(ring, minCrimp))
						continue;

					if (!ringHelper.isMinimalToBeFolded(ring, index))
						continue;


					if (crimps[i] > minCrimp)
						return false;
					if (crimps[i] < minCrimp)
						break;

					auto mergedIndex = ringHelper.foldPartially(ring, index);

					// update the possible actions
					crimpHelper.insertCrimpByIndexIfMinimal(ring, mergedIndex, true, possibleCrimps);
					crimpHelper.insertCrimpByIndexIfMinimal(ring, ring.nextIndexOf(mergedIndex), true, possibleCrimps);
					crimpHelper.insertCrimpByIndexIfMinimal(ring, ring.prevIndexOf(mergedIndex), true, possibleCrimps);

					i++;
				}

				//-----

				//cout << "this is canonical!" << endl;
				
				
				return true;
			}

		};


	}
}