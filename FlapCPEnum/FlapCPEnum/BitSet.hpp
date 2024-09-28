#pragma once
#include "abbreviation.h"
#include "ISet.hpp"
#include "BitArray.hpp"
#include <stdexcept>
#include <sstream>

namespace mylib {
	/**
	 * Wrapper for set presentation.
	*/
	class BitSet : public ISet<u_int> {
		mylib::BitArray bits;
		u_int count_ = 0;

		void substitute(const BitSet& source) {
			bits = source.bits;
			count_ = source.count_;
		}

	public:
		BitSet(const u_int& size) : bits(size) {}

		BitSet(const BitSet& source) {
			substitute(source);
		}

		virtual~BitSet() {}

		virtual bool contains(const u_int& item) const {
			return bits.isOne(item);
		}

		virtual bool containsAll(const u_int& begin, const u_int& end) const {
			return bits.areAllOne(begin, end);
		}

		virtual bool containsNothing(const u_int& begin, const u_int& end) const {
			return bits.areAllZero(begin, end);
		}

		virtual void add(const u_int& item) {
			count_++;
			bits.setOne(item);
		}

		virtual void remove(const u_int& item) {
			count_--;
			bits.setZero(item);
		}

		void remove(const u_int& begin, const u_int& end) {
			//if (! containsAll(begin, end)) {
			//	throw std::invalid_argument("0 exists in the sequence.");
			//}
			count_ -= end - begin;
			bits.fillZero(begin, end);
		}

		virtual const u_int& count() const {
			return count_;
		}

		void rotateToLower(const u_int& count) {
			bits.rotateToLower(count);
		}

		BitSet& operator=(const BitSet& right) {
			substitute(right);
			return *this;
		}

		BitSet operator&(const BitSet& right) const {
			BitSet result(bits.bitLength());

			result.bits = bits & right.bits;

			return result;
		}

		bool operator==(const BitSet& right) const {
			return bits == right.bits;
		}

		BitSet exclusiveOr(const BitSet& right) const {
			BitSet result(bits.bitLength());

			result.bits = bits ^ right.bits;

			return result;
		}

		BitSet extractValuesLowerThan(const u_int& value) const {
			BitSet result(*this);

			result.bits.cutHigherBits(value);

			return result;
		}

		virtual const u_int& capacity() const {
			return bits.bitLength();
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

		std::string toBString() const {
			std::stringstream ss;
			for (u_int i = 0; i < capacity(); i++) {
				auto bit = (contains(i)) ? '1' : '0';
					ss << bit;
			}
			return ss.str();
		}

		void MPISend(int destID, int tag)  const {
			MPI_Send(&count_, 1, MPI_UNSIGNED_LONG, destID, tag, MPI_COMM_WORLD);
			bits.MPISend(destID, tag);
		}

		void MPIReceive(int sourceID, int tag) {
			MPI_Status status;
			MPI_Recv(&count_, 1, MPI_UNSIGNED_LONG, sourceID, tag, MPI_COMM_WORLD, &status);
			bits.MPIReceive(sourceID, tag);
		}
	};

}