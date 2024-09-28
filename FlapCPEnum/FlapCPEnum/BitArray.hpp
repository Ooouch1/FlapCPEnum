#pragma once

#include <algorithm>
#include <string>
#include <vector>

#ifdef DEBUG_BitMap
#include <iostream>
#endif

#include <sstream>
#include "abbreviation.h"

#include <mpi.h>


namespace mylib {

	class BitArray {
	public:
		typedef u_int BitBlock;

	private:
		static const BitBlock ALL_ONE_BITS;
		static const BitBlock ZERO_BITS;
		static const BitBlock ONE_BITS;

		BitBlock *blocks = NULL;
		u_int blockLength_ = 0;
		u_int bitLength_ = 0;

		static const u_int OneBlockBitLength = 8 * sizeof(BitBlock);

		inline u_int blockIndex(u_int bitIndex) const {
			return bitIndex / OneBlockBitLength;
		}

		inline u_int localBitIndex(u_int bitIndex) const {
			return bitIndex % OneBlockBitLength;
		}

		void substitute(const BitArray& right) {
			// adjust blocks
			if (blockLength() == right.blockLength()) {
				clear();
				bitLength_ = right.bitLength();
			}
			else {
				delete[] blocks; // note: ""delete NULL" does nothing.
				allocate(right.bitLength());
			}

			std::copy(right.blocks, right.blocks + right.blockLength(), blocks);
		}

		void allocate(u_int bitLength) {
			bitLength_ = bitLength;

			blockLength_ = blockIndex(bitLength);
			if (localBitIndex(bitLength) != 0) blockLength_++;

			blocks = new BitBlock[blockLength()];
			//std::fill(blocks, blocks + blockLength(), ZERO_BITS);

			clear();
		}

	public:
		BitArray(const BitArray& bits) {
			substitute(bits);
		}

		BitArray() {
		}


		BitArray(u_int bitLength) {
			allocate(bitLength);
		}


		~BitArray() {
			delete[] blocks;
		}

		//============================================================================
		// BIT DETECTION

		inline bool isOne(u_int bitIndex) const {
			BitBlock extracted = blocks[blockIndex(bitIndex)]
				& (ONE_BITS << localBitIndex(bitIndex));
			return  extracted != ZERO_BITS;
		}

		bool areAllOne(u_int bitIndexFrom, u_int bitIndexEnd) const {
			u_int blockIndexFrom = blockIndex(bitIndexFrom);
			u_int localIndexFrom = localBitIndex(bitIndexFrom);

			u_int bitIndexTail = bitIndexEnd - 1;
			u_int blockIndexTail = blockIndex(bitIndexTail);
			u_int localIndexTail = localBitIndex(bitIndexTail);

			// 11...100...0
			const BitBlock lowCutMask = (ALL_ONE_BITS << localIndexFrom);

			// << (localIndexTail+1) fails because
			// recent CPU doesn't seem to interpret
			// int x >> 32 (or << 32) as 0. ughhh!!!

			// 00...011...1
			const BitBlock highCutMask = ~((ALL_ONE_BITS << localIndexTail) << 1);

			if (blockIndexFrom == blockIndexTail) {
				BitBlock mask = lowCutMask & highCutMask;
				return (blocks[blockIndexFrom] & mask) == mask;
			}

			if ((blocks[blockIndexFrom] & lowCutMask) != lowCutMask) {
				return false;
			}


			for (u_int i = blockIndexFrom + 1; i < blockIndexTail; i++) {
				if (blocks[i] != ALL_ONE_BITS) {
					return false;
				}
			}

			return (blocks[blockIndex(bitIndexTail)] & highCutMask) == highCutMask;
		}

		bool areAllZero(u_int bitIndexFrom, u_int bitIndexEnd) const {
			u_int blockIndexFrom = blockIndex(bitIndexFrom);
			u_int localIndexFrom = localBitIndex(bitIndexFrom);

			u_int bitIndexTail = bitIndexEnd - 1;
			u_int blockIndexTail = blockIndex(bitIndexTail);
			u_int localIndexTail = localBitIndex(bitIndexTail);

			// 11...100...0
			const BitBlock lowCutMask = (ALL_ONE_BITS << localIndexFrom);

			// << (localIndexTail+1) fails because
			// recent CPU doesn't seem to interpret
			// int x >> 32 (or << 32) as 0. ughhh!!!

			// 00...011...1
			const BitBlock highCutMask = ~((ALL_ONE_BITS << localIndexTail) << 1);

			if (blockIndexFrom == blockIndexTail) {
				BitBlock mask = lowCutMask & highCutMask;
				return (blocks[blockIndexFrom] & mask) == ZERO_BITS;
			}

			if ((blocks[blockIndexFrom] & lowCutMask) != ZERO_BITS) {
				return false;
			}


			for (u_int i = blockIndexFrom + 1; i < blockIndexTail; i++) {
				if (blocks[i] != ZERO_BITS) {
					return false;
				}
			}

			return (blocks[blockIndex(bitIndexTail)] & highCutMask) == ZERO_BITS;
		}

		//============================================================================
		// BIT MODIFICATION

		inline void setOne(u_int bitIndex) {
#ifdef DEBUG_BitMap
			std::cout << blockIndex(bitIndex) << " "
				<< localBitIndex(bitIndex) << " "
				<< "mask:" << std::hex
				<< (1 << localBitIndex(bitIndex)) << std::dec << std::endl;
#endif
			blocks[blockIndex(bitIndex)] |= (ONE_BITS << localBitIndex(bitIndex));

#ifdef DEBUG_BitMap
			std::cout << "modified block: " << std::hex
				<< blocks[blockIndex(bitIndex)] << std::dec << std::endl;
#endif
		}

		inline void setZero(u_int bitIndex) {
			blocks[blockIndex(bitIndex)] &= ~(ONE_BITS << localBitIndex(bitIndex));
		}

		inline void fillOne(u_int bitIndexFrom, u_int bitIndexEnd) {
			u_int blockIndexFrom = blockIndex(bitIndexFrom);
			u_int localIndexFrom = localBitIndex(bitIndexFrom);

			u_int bitIndexTail = bitIndexEnd - 1;
			u_int blockIndexTail = blockIndex(bitIndexTail);
			u_int localIndexTail = localBitIndex(bitIndexTail);

			// 11...100...0
			const BitBlock lowCutMask = (ALL_ONE_BITS << localIndexFrom);

			// some strange 1-bit shift is needed because
			// recent CPU doesn't seem to interpret
			// int x >> 32 (or << 32) as 0. ughhh!!!

			// 00...011...1
			const BitBlock highCutMask = ~((ALL_ONE_BITS << localIndexTail) << 1);

			if (blockIndexFrom == blockIndexTail) {
				blocks[blockIndexFrom] |= lowCutMask & highCutMask;
				return;
			}

			blocks[blockIndexFrom] |= lowCutMask;

			std::fill_n(blocks + blockIndexFrom + 1,
				blockIndexTail - blockIndexFrom - 1, ALL_ONE_BITS);

			blocks[blockIndex(bitIndexTail)] |= highCutMask;

		}

		inline void fillZero(u_int bitIndexFrom, u_int bitIndexEnd) {
			u_int blockIndexFrom = blockIndex(bitIndexFrom);
			u_int localIndexFrom = localBitIndex(bitIndexFrom);

			u_int bitIndexTail = bitIndexEnd - 1;
			u_int blockIndexTail = blockIndex(bitIndexTail);
			u_int localIndexTail = localBitIndex(bitIndexTail);

			// 00...011...1
			const BitBlock lowPassMask = ~(ALL_ONE_BITS << localIndexFrom);
			// 11...100...0
			const BitBlock highPassMask = ((ALL_ONE_BITS << localIndexTail) << 1);

			if (blockIndexFrom == blockIndexTail) {
				blocks[blockIndexFrom] &= lowPassMask | highPassMask;
				return;
			}

#ifdef DEBUG_BitMap
			std::cout << "from:" << bitIndexFrom << " length:" << length << std::endl;
			std::cout << "blk_from:" << blockIndexFrom << " local_from:"
				<< localIndexFrom << std::endl;
#endif
			blocks[blockIndexFrom] &= lowPassMask;

#ifdef DEBUG_BitMap
			std::cout << toString() << std::endl;
#endif

			std::fill_n(blocks + blockIndexFrom + 1,
				blockIndexTail - blockIndexFrom - 1, ZERO_BITS);

			blocks[blockIndexTail] &= highPassMask;

		}

		void rotateToLower(const u_int amount) {
			u_int amount_ = amount % bitLength();

			u_int rotatedBlockLength = blockIndex(amount_);
			u_int rotatedBitLength = localBitIndex(amount_);


			if (blockLength() == 1) {
				BitBlock exceeding = blocks[0] << (bitLength() - rotatedBitLength);
				blocks[0] >>= rotatedBitLength;
				blocks[0] |= exceeding;
				return;
			}


			// shift blocks (= size * n bits)
			if (rotatedBlockLength > 0) {
				std::reverse(blocks, blocks + blockLength());
				std::reverse(blocks + blockLength() - rotatedBlockLength, blocks + blockLength());
				std::reverse(blocks, blocks + blockLength() - rotatedBlockLength);
			}

			// shift bits
			BitBlock firstExceeding = blocks[0] << (OneBlockBitLength - rotatedBitLength);
			for (u_int i = 0; i < blockLength() - 1; i++) {
				BitBlock exceeding = blocks[i + 1] << (OneBlockBitLength - rotatedBitLength);
				blocks[i] >>= rotatedBitLength;
				blocks[i] |= exceeding;
			}
			blocks[blockLength() - 1] >>= rotatedBitLength;
			blocks[blockLength() - 1] |= firstExceeding;
		}


		void clear() {
			std::fill(blocks, blocks + blockLength(), 0);
		}

		void cutHigherBits(u_int bitIndexFrom) {
			u_int blockIndexFrom = blockIndex(bitIndexFrom);
			u_int localIndexFrom = localBitIndex(bitIndexFrom);

			const BitBlock highCutMask = ~((ALL_ONE_BITS << localIndexFrom) << 1);
			blocks[blockIndexFrom] &= highCutMask;

			for (u_int blockIndex = blockIndexFrom + 1; blockIndex < blockLength(); blockIndex++) {
				blocks[blockIndex] = ZERO_BITS;
			}
		}

		//============================================================================
		// MISC

		/**
		 * Count of bits
		 */
		const u_int& bitLength() const {
			return bitLength_;
		}

		/**
		 * Count of blocks
		 */
		const u_int& blockLength() const {
			return blockLength_;
		}


		std::string toString() const {
			std::ostringstream stream;

			u_int blockCount = 0;

			for (u_int i = 0; i < bitLength(); i++) {

				if (localBitIndex(i) == 0) {
					if (blockCount > 0 && blockCount % 2 == 0) {
						stream << std::endl;
					}
					blockCount++;
					stream << " ";
				}

				if (isOne(i)) {
					stream << "1";
				}
				else {
					stream << "0";
				}
			}
			return stream.str();
		}

		void MPISend(int destID, int tag) const {
			MPI_Send(&bitLength_, 1, MPI_UNSIGNED_LONG, destID, tag, MPI_COMM_WORLD);
			MPI_Send(&blockLength_, 1, MPI_UNSIGNED_LONG, destID, tag, MPI_COMM_WORLD);
			MPI_Send(blocks, blockLength_ * sizeof(BitBlock), MPI_BYTE, destID, tag, MPI_COMM_WORLD);

		}

		void MPIReceive(int sourceID, int tag) {
			MPI_Status status;
			MPI_Recv(&bitLength_, 1, MPI_UNSIGNED_LONG, sourceID, tag, MPI_COMM_WORLD, &status);
			MPI_Recv(&blockLength_, 1, MPI_UNSIGNED_LONG, sourceID, tag, MPI_COMM_WORLD, &status);
			allocate(bitLength_);
			MPI_Recv(blocks, blockLength_ * sizeof(BitBlock), MPI_BYTE, sourceID, tag, MPI_COMM_WORLD, &status);
		}

		//============================================================================
		// OPERATORS


		BitArray& operator=(const BitArray& right) {
			substitute(right);
			return *this;
		}

		BitArray operator&(const BitArray& right) const {
			BitArray result(bitLength());

			for (u_int i = 0; i < blockLength(); i++) {
				result.blocks[i] = blocks[i] & right.blocks[i];
			}

			return result;
		}

		BitArray& operator&=(const BitArray& right) {
			for (u_int i = 0; i < blockLength(); i++) {
				blocks[i] &= right.blocks[i];
			}

			return *this;
		}

		BitArray operator^(const BitArray& right) const {
			BitArray result(bitLength());

			for (u_int i = 0; i < blockLength(); i++) {
				result.blocks[i] = blocks[i] ^ right.blocks[i];
			}

			return result;
		}

		bool operator==(const BitArray& right) const {
			for (u_int i = 0; i < blockLength(); i++) {
				if (blocks[i] != right.blocks[i]) {
					return false;
				}
			}
			return true;
		}


	};


}