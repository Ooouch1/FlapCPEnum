#pragma once
#include "abbreviation.h"
#include "BitSet.hpp"
#include "ISet.hpp"
#include <sstream>
#include <ios>
#include <cmath>
#include <string>
#include <vector>

namespace enumeration {
	namespace origami {

		class EncodablePatternBase : public mylib::ISet<u_int> {

			u_int lineIndex(const u_int& i) const {
				return (i + peekBasePoint()) % capacity();
			}

			template <typename GetLineType>
			std::string encodePrivate(const GetLineType& getType) const {
				using namespace std;
				stringstream ss;
				u_int shrinkedIndex = 0;

				for (u_int i = 0; i < capacity(); ) {
					if (contains(lineIndex(i))) {
						ss << getType(lineIndex(i), shrinkedIndex);
						shrinkedIndex++;
						i++;
					}
					else {
						int emptyCount = 0;
						while (!contains(lineIndex(i)) && i < capacity()) {
							emptyCount++;
							i++;
						}
						//ss << 'E';
						ss << emptyCount;
					}
				}
				return ss.str();
			}

		public:
			/**
			* crease pattern string;
			*
			* CP string is composed of '+', '-', "E[0-9]+".
			* '+' means a major line.
			* '-' means a minor line.
			* "E[0-9]+" means sequential empty places (run-length compression).
			*/
			template <typename TBitSet>
			std::string encode(const TBitSet& minors) const {
				return encodePrivate([&minors](const int i, const int shrinked) {
					return (minors.contains(shrinked)) ? '-' : '+';	});
			}

			std::string encode() const {
				return encodePrivate([](const int i, const int shrinked) {return '+'; });
			}

			std::string toString() const {
				std::stringstream ss;
				for (u_int i = 0; i < capacity(); i++) {
					if (contains(i)) {
						ss << i << " ";
					}
				}

				return ss.str();
			}

			virtual u_int peekBasePoint() const = 0;

		};


		/**
		 * This class should be used as a stack to compute kawasaki theorem values.
		 * THIS IS OLD IMPLEMENTATION AND SHOULD NOT BE USED.
		 * TODO: swap existing references of this class to the new one.
		 */
		class FlapPattern : public EncodablePatternBase {
			mylib::BitSet bits;
			int kawasakiSign = 1;
			int prefixKawasakiCount = 0;

			int lastDiff = 0;
			u_int lastItem = 0;

			std::vector<u_int> itemHistory;
			std::vector<int> diffHistory;

		public:

			FlapPattern(const u_int& capacity) : bits(capacity) {
				itemHistory.reserve(capacity);
				diffHistory.reserve(capacity);
			}

			FlapPattern(const FlapPattern& source) : bits(source.bits) {
				kawasakiSign = source.kawasakiSign;
				prefixKawasakiCount = source.prefixKawasakiCount;

				lastDiff = source.lastDiff;
				lastItem = source.lastItem;

				//itemHistory = source.itemHistory;
				//diffHistory = source.diffHistory;
			}

			virtual bool contains(const u_int& item) const {
				return bits.contains(item);
			}

			virtual void add(const u_int& item) {
				bits.add(item);

				//if (item > lastItem) {
				kawasakiSign *= -1;

				int diff = kawasakiSign * (item - lastItem);
				prefixKawasakiCount += diff;

				lastItem = item;
				lastDiff = diff;

				itemHistory.push_back(lastItem);
				diffHistory.push_back(lastDiff);

				// DEBUG
				// std::cout << "add " << item << ", kawasaki count = " << kawasakiCount << std::endl;
				//}

			}

			virtual void remove(const u_int& item) {
				bits.remove(item);

				//if (item == lastItem) {

				lastItem -= abs(lastDiff);
				itemHistory.pop_back();
				diffHistory.pop_back();

				kawasakiSign *= -1;

				if (count() == 0) {
					prefixKawasakiCount = 0;
					lastDiff = 0;

					itemHistory.clear();
					diffHistory.clear();
				}
				else {
					prefixKawasakiCount -= lastDiff;
					int lastlast = *(++(itemHistory.rbegin()));

					// change to constant-time operation using stacks
					lastDiff = kawasakiSign * (lastItem - lastlast);

					//for (int lastlast = (int)lastItem - 1; lastlast >= 0; lastlast--) {
					//	if (contains(lastlast)) {
					//		lastDiff = kawasakiSign * (lastItem - lastlast);
					//		break;
					//	}
				}



				//DEBUG
				// std::cout << "remove " << item << ", kawasaki count = " << kawasakiCount << std::endl;
				//}
			}

			virtual const u_int& count() const {
				return bits.count();
			}

			virtual const u_int& capacity() const {
				return bits.capacity();
			}

			bool kawasakiCountCanBeZero() const {
				return (u_int)(abs(prefixKawasakiCount)) <= capacity() - lastItem;
			}

			bool kawasakiCountIsZero() const {
				// fill tail and compare to zero
				return prefixKawasakiCount - kawasakiSign * (capacity() - lastItem) == 0;
			}

			virtual u_int peekBasePoint() const {
				return 0;
			}

		};


		/**
		* This class should be used as a stack to compute kawasaki theorem values.
		*/
		class FlapPatternForBraceletEnum : public EncodablePatternBase {
			mylib::BitSet bits;

			// Imagine a circle with indices on its circumference clockwisely.
			// You stand on the center of the circle and you are watching the item added last time.
			// the leftGap is on your left and so do for rightGap.

			int leftKawasakiCount = 0;

			int leftGap = 0;
			int rightGap = 0;

			std::vector<u_int> itemHistory;
			std::vector<int> leftGapHistory;

			template <typename Value>
			Value sign(const Value& v) {
				return (v >= 0) ? 1 : -1;
			}

			int normalizedItem(int item) const {
				return (capacity() + item - itemHistory.front()) % capacity();
			}

		public:
			FlapPatternForBraceletEnum() : bits(0) {}

			FlapPatternForBraceletEnum(const u_int& capacity) : bits(capacity) {
				itemHistory.reserve(capacity);
				leftGapHistory.reserve(capacity);
			}

			FlapPatternForBraceletEnum(const FlapPatternForBraceletEnum& source) : bits(source.bits) {
				leftKawasakiCount = source.leftKawasakiCount;
				rightGap = source.rightGap;

				//itemHistory = source.itemHistory;
				//diffHistory = source.diffHistory;
			}

			virtual bool contains(const u_int& item) const {
				return bits.contains(item);
			}


			virtual void add(const u_int& item) {

				if (bits.count() == 0) {
					leftGap = 0;
				}
				else {
					leftGap = sign(rightGap) * (normalizedItem(item) - normalizedItem(itemHistory.back()));
				}
				bits.add(item);
				itemHistory.push_back(item);

				rightGap = -sign(rightGap) * (capacity() - normalizedItem(item));
				leftKawasakiCount += leftGap;

				leftGapHistory.push_back(leftGap);

				// DEBUG
				// std::cout << "add " << item << ", kawasaki count = " << kawasakiCount << std::endl;
				//}

			}

			virtual void remove(const u_int& item) {
				if (item != itemHistory.back()) {
					throw "error!";
				}

				bits.remove(item);
				itemHistory.pop_back();

				if (count() == 0) {
					leftKawasakiCount = 0;
					leftGap = 0;
					rightGap = 0;
					leftGapHistory.clear();
				}
				else {
					rightGap = leftGap - rightGap;
					leftKawasakiCount -= leftGap;

					leftGapHistory.pop_back();
					leftGap = leftGapHistory.back();
				}

				//DEBUG
				// std::cout << "remove " << item << ", kawasaki count = " << kawasakiCount << std::endl;
				//}
			}

			virtual const u_int& count() const {
				return bits.count();
			}

			virtual const u_int& capacity() const {
				return bits.capacity();
			}

			bool kawasakiCountCanBeZero() const {
				auto left = abs(leftKawasakiCount);
				auto right = abs(rightGap);

				return left <= right;
			}

			bool kawasakiCountIsZero() const {
				using namespace std;
				//cout << bits.toBString() << " " 
				//	<< " last=" << itemHistory.back()
				//	<< " count=" << leftKawasakiCount 
				//	<< " left=" << leftGap 
				//	<< " right=" << rightGap << endl;

				//if (leftKawasakiCount + rightGap != 0) {
				//	cout << "not kawasaki" << endl;
				//}
				// fill tail and compare to zero
				return leftKawasakiCount + rightGap == 0;
			}


			const int& peekLeftkawasakiCount() {
				return leftKawasakiCount;
			}

			const int& peekLeftGap() {
				return leftGap;
			}

			const int& peekRightGap() {
				return rightGap;
			}

			virtual u_int peekBasePoint() const {
				if (itemHistory.empty())
					return 0;
				return itemHistory.front();
			}


			void MPISendAsSet(int destID, int tag) const {
				bits.MPISend(destID, tag);
			}

			void MPIReceiveAsSet(int sourceID, int tag) {
				bits.MPIReceive(sourceID, tag);
			}
		};

	}
}