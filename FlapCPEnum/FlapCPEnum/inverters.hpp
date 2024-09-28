#pragma once

#include "abbreviation.h"
#include "CircularPattern.hpp"
#include <limits>
#include <string>
#include "SharedArrayPointer.hpp"

namespace enumeration {
	class IInverter {
	public:
		virtual u_int operator() (const u_int& index) const = 0;
		virtual u_int operator() (const int& index) const = 0;

		virtual std::string toString() const = 0;

		virtual bool reversesOrder() const = 0;
	};

	namespace circular {

		class RotationInverter : public IInverter {
			const u_int amount;
			const u_int placeCount;

		public:
			RotationInverter(u_int amount, u_int placeCount) : amount(amount), placeCount(placeCount) {}

			RotationInverter(const RotationInverter& source) : amount(source.amount), placeCount(source.placeCount) {}

			virtual u_int operator() (const u_int& index) const {
				return (index + placeCount - amount) % placeCount;
			}
			virtual u_int operator() (const int& index) const {
				return (*this)((u_int) index);
			}

			template <typename TSet>
			TSet operator()(const TSet& pattern) const {
				TSet inverse(pattern.capacity());

				for (u_int i = 0; i < placeCount; i++) {
					if (pattern.contains(i)) {
						inverse.add((*this)(i));
					}
				}

				return inverse;
			}

			virtual bool reversesOrder() const {
				return false;
			}
			//template<typename TSet>
			//TSet operator()(const TSet& pattern) const {
			//	return pattern.createBitRotationToLower();
			//}

			virtual std::string toString() const {
				using namespace std;

				stringstream ss;
				ss << "rotate: counter-clockwise amount=" << amount;

				return ss.str();
			}
		};


		class MirrorInverter : public IInverter {
			const u_int axis;
			const u_int placeCount;
		public:
			MirrorInverter(u_int axis, u_int placeCount) : axis(axis), placeCount(placeCount) {}
			MirrorInverter(const MirrorInverter& source) : axis(source.axis), placeCount(source.placeCount) {}


			virtual u_int operator() (const u_int& index) const {
				return (2 * axis + placeCount - index) % placeCount;
			}
			virtual u_int operator() (const int& index) const {
				return (*this)((u_int)index);
			}

			template <typename TSet>
			TSet operator()(const TSet& pattern) const {
				TSet inverse(pattern.capacity());

				for (u_int i = 0; i < placeCount; i++) {
					if (pattern.contains(i)) {
						inverse.add((*this)(i));
					}
				}

				return inverse;
			}

			virtual bool reversesOrder() const {
				return true;
			}

			virtual std::string toString() const {
				using namespace std;

				stringstream ss;
				ss << "mirror: axis=" << axis;

				return ss.str();
			}

		};

		class MiddleMirrorInverter :public IInverter {
			const u_int priorOfAxis;
			const u_int placeCount;
		public:
			MiddleMirrorInverter(const u_int leftOfAxis, const u_int placeCount) : priorOfAxis(leftOfAxis), placeCount(placeCount) {}
			MiddleMirrorInverter(const MiddleMirrorInverter& source) : priorOfAxis(source.priorOfAxis), placeCount(source.placeCount) {}


			virtual u_int operator() (const u_int& index) const {
				return (2 * priorOfAxis + placeCount - index + 1) % placeCount;
			}
			virtual u_int operator() (const int& index) const {
				return (*this)((u_int)index);
			}

			template <typename TSet>
			TSet operator()(const TSet& pattern) const {
				TSet inverse(pattern.capacity());

				for (u_int i = 0; i < placeCount; i++) {
					if (pattern.contains(i)) {
						inverse.add((*this)(i));
					}
				}

				return inverse;
			}

			virtual bool reversesOrder() const {
				return true;
			}

			virtual std::string toString() const {
				using namespace std;

				stringstream ss;
				ss << "mid-mirror: priorOfaxis=" << priorOfAxis;

				return ss.str();
			}

		};
	}

	class DummyInverter :public IInverter {
	public:


		virtual u_int operator() (const u_int& index) const {
			return index;
		}
		virtual u_int operator() (const int& index) const {
			return index;
		}

		template <typename TSet>
		TSet operator()(const TSet& pattern) const {
			return pattern;
		}

		virtual bool reversesOrder() const {
			return false;
		}

		virtual std::string toString() const {
			using namespace std;

			stringstream ss;
			ss << "dummy inverter";

			return ss.str();
		}

	};


	/**
	* Inverts index in mapped world.
	* This class uses a map X->Y and a function invert(Y)->Y.
	* if invert(y) does not have a key in the map, it is failure.
	*/
	template<typename TInverter>
	class MappedIndexInverter : public IInverter {
		const TInverter inverter;
		mylib::SharedArrayPointer<u_int> indexMap;
		
		const u_int mapSize;
		const u_int valueSize;

		mylib::SharedArrayPointer<u_int> reverseMap;
		//u_int *reverseMap = NULL;

	public:
		const u_int FAILED = std::numeric_limits<u_int>::max();

		MappedIndexInverter(const MappedIndexInverter& source) : 
			inverter(source.inverter), mapSize(source.valueSize), valueSize(source.valueSize), reverseMap(source.reverseMap) {
			indexMap = source.indexMap;

			//reverseMap = new u_int[valueSize];
			//std::copy_n(source.reverseMap, valueSize, reverseMap);

		}

		/**
		 * inv: a function x \in X -> y \in Y
		 * placeCount: count of places after mapping (== |Y|).
		 * indexMap: a map x -> y
		 * mapSize: size of indexMap, or |X|
		 */
		MappedIndexInverter(const TInverter& inv, u_int placeCount,  mylib::SharedArrayPointer<u_int>& indexMap, u_int keyCount) :
			inverter(inv), indexMap(indexMap), mapSize(keyCount), valueSize(placeCount), reverseMap(placeCount) {
			//reverseMap = new u_int[valueSize];

			std::fill_n(reverseMap.getRawArray(), valueSize, FAILED);
			for (u_int i = 0; i < keyCount; i++) {
				reverseMap[indexMap[i]] = i;
			}
		}

		bool canInvert() {
			for (u_int i = 0; i < mapSize; i++) {
				if ((*this)(i) == FAILED) {
					return false;
				}
			}

			return true;
		}

		virtual u_int operator() (const u_int& index) const {
			return reverseMap[inverter(indexMap[index])];
		}
		virtual u_int operator() (const int& index) const {
			return (*this)((u_int)index);
		}

		template <typename TSet_Assignment>
		TSet_Assignment operator()(const TSet_Assignment& pattern) const {
			TSet_Assignment inverse(pattern.capacity());

			for (u_int i = 0; i < pattern.capacity(); i++) {
				if (pattern.contains(i)) {
					inverse.add((*this)(i));
				}
			}

			return inverse;
		}

		virtual bool reversesOrder() const {
			return inverter.reversesOrder();
		}

		virtual std::string toString() const {
			using namespace std;

			stringstream ss;
			ss << "mapper: " << inverter.toString();

			return ss.str();
		}


	};


}