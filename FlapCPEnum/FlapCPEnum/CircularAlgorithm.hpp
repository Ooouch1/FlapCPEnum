#pragma once
#include "abbreviation.h"
#include <string>
#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace mylib {
	template<typename Value, bool debug = false>
	class CircularAlgorithm {

	public:

		template<typename ValueArray>
		int findFirstIndexOfLeastCircular(const ValueArray& values) const {
			return findFirstIndexOfLeastCircular(values, values.size());
		}

		/*
		 * find the first index of the least circular string of given array in range of index = 0 ... lengh-1.
		 * template:
		 *    ValueArray: should implement operator[]; typically std::vector<>.
		 *
		 * this algorithm is from "LEXICOGRAPHlCALLYL EAST CIRCULARS UBSTRINGS" by Kellogg S, BOOTH, pp240--241 Volume 10, number 4,5 INFORMATION PROCESSING LETTERS 5 July 1980.
		 */
		template<typename ValueArray>
		int findFirstIndexOfLeastCircular(const ValueArray& values, const int& length) const {
			using namespace std;
			const int NIL_INDEX = -1;

			const int doubledLength = 2 * length;

			vector<int> table(doubledLength, NIL_INDEX);

			table[0] = NIL_INDEX;

			//cout << "pattern = " << values << endl;
			int k = 0;
			for (int j = 1; j < doubledLength; j++) {
				// causes WRONG answer ALTHOUGH this "if" statement is in the paper...
				// A killer case is "0001011010001011"
				// see the python code @ https://en.wikipedia.org/wiki/Lexicographically_minimal_string_rotation
				//if (j - k >= length) {
				//	if (debug) cout << "return " << k << endl;
				//	return k;
				//}

				int i = table[j - k - 1];

				const auto& v_j = values[j % length];
				auto ki_mod = [&]() {return (k + i + 1) % length; };


				while (v_j != values[ki_mod()] && i != NIL_INDEX) {
					if (v_j < values[ki_mod()]) {
						k = j - i - 1;
						if (debug) cout << "while loop updates k: " << "i=" << i << ", j = " << j << ", k=" << k << endl;
					}
					i = table[i];
					if (debug) cout << "while loop updates i: " << "i=" << i << ", j = " << j << ", k=" << k << endl;
				}
				if (v_j != values[ki_mod()]) {
					if (v_j < values[k]) {
						k = j;
						if (debug) cout << "i==NILL updates k: " << "i=" << i << ", j = " << j << ", k=" << k << endl;
					}
					table[j - k] = NIL_INDEX;
					if (debug) cout << "i==NILL table[" << j - k << "] = " << NIL_INDEX << endl;
				}
				else {
					if (debug) cout << "updates table[" << j - k << "] = " << i + 1 << endl;
					table[j - k] = i + 1;
				}

				if(debug){
					cout << "table: " << endl;
					for (auto& v : table)
						cout << v << " ";
					cout << endl;
				}
			}

			return k;
		}

		template <typename Array>
		class ReverseProxy {
			const Array& values;
			const int length;
		public:
			ReverseProxy(const Array& values, const int& length): values(values), length(length) {
			}

			//Value& operator[](const int& index) {
			//	return values[length - 1 - index];
			//}
			const Value& operator[](const int& index) const {
				return values[length - 1 - index];
			}
		};

		template<typename ValueArray_left, typename ValueArray_right>
		bool lessThanEqual(const ValueArray_left& left, const ValueArray_right& right,
			const int& length,
			const int& leftStartIndex = 0, const int& rightStartIndex = 0) const {

			auto modIndex = [&length](const auto& firstIndex, const auto& offset) {
				return (firstIndex + offset) % length; };

			for (int i = 0; i < length; i++) {
				if (right[modIndex(rightStartIndex, i)] == left[modIndex(leftStartIndex, i)])
					continue;
				else if (right[modIndex(rightStartIndex, i)] < left[modIndex(leftStartIndex, i)])
					return false;
				else break;
			}

			return true;
		}

		template<typename ValueArray_left, typename ValueArray_right>
		bool lessThan(const ValueArray_left& left, const ValueArray_right& right,
			const int& length,
			const int& leftStartIndex = 0, const int& rightStartIndex = 0) const {

			auto modIndex = [&length](const auto& firstIndex, const auto& offset) {
				return (firstIndex + offset) % length; };
			int i;
			for (i = 0; i < length; i++) {
				if (right[modIndex(rightStartIndex, i)] == left[modIndex(leftStartIndex, i)])
					continue;
				else if (right[modIndex(rightStartIndex, i)] < left[modIndex(leftStartIndex, i)])
					return false;
				else break;
			}

			return i < length;
		}

		template<typename Array>
		ReverseProxy<Array> createReverse(Array& values, const int& length) {
			ReverseProxy<Array> r(values, length);
			return r;
		}

		template<typename ValueArray>
		bool isCanonicalOnSymmetry(const ValueArray& values, const int& length) const {
			using namespace std;

			auto firstIndex = findFirstIndexOfLeastCircular(values, length);
			if (firstIndex != 0)
			//if (!(values[firstIndex] == values[0]))
				return false;

			//auto reversed = creaseReverse<vector<Value> >(values, length);
			ReverseProxy<ValueArray> reversed(values, length);
			auto rev_firstIndex = findFirstIndexOfLeastCircular(reversed, length);

			if (lessThan(reversed, values, length, rev_firstIndex))
				return false;

			if (lessThan(values, values, length, firstIndex))
				return false;

			return true;
		}

		template<typename CharArray, bool reverseOnly = false>
		std::string createCanonicalOnSymmetry(const CharArray& values) {
			return createCanonicalOnSymmetry(values, values.size());
		}

		template<typename CharArray, typename CharArray_Out, bool reverseOnly = false>
		void createCanonicalOnSymmetry(const CharArray& values, CharArray_Out& canonical, const int& length) {
			using namespace std;
			auto firstIndex = (reverseOnly) ? 0 : findFirstIndexOfLeastCircular(values, length);

			ReverseProxy<CharArray> reversed(values, length);
			auto rev_firstIndex = findFirstIndexOfLeastCircular(reversed, length);

			// select the least one from {least_reversed, least}

			if (lessThan(reversed, values, length, rev_firstIndex, firstIndex)) {
				createShiftedString(reversed, rev_firstIndex, canonical, length);
				return;
			}

			createShiftedString(values, firstIndex, canonical, length);
		}

		template<typename CharArray, bool reverseOnly = false>
		std::string createCanonicalOnSymmetry(const CharArray& values, const int& length) {
			using namespace std;
			string canonical(length, 0);
			createCanonicalOnSymmetry(values, canonical, length);
			return canonical;
		}


		template<typename CharArray>
		std::string createShiftedString(const CharArray& base, const int& firstIndex, const int& length) const {
			using namespace std;
			string shifted(length, 0);
			createShiftedString(base, firstIndex, shifted, length);			
			return shifted;
		}


		template<typename CharArray, typename CharArray_Out>
		void createShiftedString(const CharArray& base, const int& firstIndex, CharArray_Out& shifted, const int& length) const {
			using namespace std;
			
			for (int i = 0; i < length; i++) {
				shifted[i] = base[(i + firstIndex) % length];
			}
		}

	};
}
