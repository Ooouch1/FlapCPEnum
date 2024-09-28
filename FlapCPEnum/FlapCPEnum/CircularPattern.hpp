#pragma once
#include "BitSet.hpp"

namespace enumeration {
	namespace circular {

		class CircularPattern : public mylib::ISet<u_int> {
			mylib::BitSet bits;
		public:

			CircularPattern(const u_int& capacity) : bits(capacity) {
			}

			CircularPattern(const CircularPattern& source) : bits(source.bits) {
			}

			virtual bool contains(const u_int& item) const {
				return bits.contains(item);
			}

			virtual void add(const u_int& item) {
				bits.add(item);
			}

			virtual void remove(const u_int& item) {
				bits.remove(item);
			}

			const mylib::BitSet& asBitSet() const {
				return bits;
			}

			virtual const u_int& count() const {
				return bits.count();
			}

			virtual const u_int& capacity() const {
				return bits.capacity();
			}

			mylib::BitSet createBitRotationToLower(const u_int& amount) const {
				auto rotated = bits;
				rotated.rotateToLower(amount);

				return rotated;
			}

			mylib::BitSet extractValuesLowerThan(const u_int& value) const {
				auto lowers = bits.extractValuesLowerThan(value);
			}

			std::string toString() const {
				return bits.toString();
			}

			bool operator==(const CircularPattern& right) const {
				return bits == right.bits;
			}
		};
	}
}