#pragma once

#include <vector>
#include "CharVecFactory.hpp"
#include "IsFoldable.hpp"
#include "searchtool.hpp"

namespace enumeration {
	namespace origami {
		template<char FLAT, char MAJOR, char MINOR>
		struct KawasakiValues {
			int sum;
			int sign;
			int prevLineIndex;
			int headIndex;

			KawasakiValues() : sum(0), sign(1), prevLineIndex(0), headIndex(0) {

			}

			KawasakiValues(const KawasakiValues& source)
				: sum(source.sum), sign(source.sign), prevLineIndex(source.prevLineIndex), headIndex(source.headIndex)
			{}

			KawasakiValues& operator=(const KawasakiValues& right) {
				sum = right.sum;
				sign = right.sign;
				prevLineIndex = right.prevLineIndex;
				headIndex = right.headIndex;

				return *this;
			}

			template<typename TCharArray>
			KawasakiValues nextValues(const TCharArray& a, const int& t, const int& placeCount) const {
				if (a[t] == FLAT)
					return *this;

				int angle = 0;
				auto next = *this;

				if (next.headIndex == 0) {
					next.headIndex = t;
				}
				else {
					angle = computeAngle(t, placeCount);
				}

				next.sum += sign * angle;
				next.sign *= -1;
				next.prevLineIndex = t;

				return next;
			}

			bool theoremCanHold(const int& t, const int& placeCount) const {
				return abs(sum + sign * computeAngle(t, placeCount)) <= computeRemain(t, placeCount);
			}

			bool theoremHolds(const int& placeCount) const {
				return sum + sign * computeRemain(prevLineIndex, placeCount) == 0;
			}

			int computeRemain(const int& t, const int& placeCount) const {
				return placeCount - normalize(t, headIndex, placeCount);
			}

		private:
			int computeAngle(const int& t, const int& placeCount) const {
				return normalize(t, headIndex, placeCount) - normalize(prevLineIndex, headIndex, placeCount);
			}
			int normalize(const int& t, const int& head, const int& placeCount) const {
				return (placeCount + t - head) % placeCount;
			}
		};

		template<char FLAT, char MAJOR, char MINOR>
		struct MaekawaValues {
			int majorCount;
			int minorCount;

			MaekawaValues() : majorCount(0), minorCount(0) {}

			MaekawaValues(const MaekawaValues& source) : majorCount(source.majorCount), minorCount(source.minorCount) {}

			template<typename TCharArray>
			MaekawaValues nextValues(const TCharArray& a, const int& t) const {
				MaekawaValues next = *this;
				next.majorCount = majorCount + (a[t] == MAJOR);
				next.minorCount = minorCount + (a[t] == MINOR);

				return next;
			}

			bool theoremHolds() const {
				return majorCount - minorCount == 2;
			}

			bool theoremCanHold(const int& remain) const {
				if (majorCount < minorCount)
					return majorCount + remain - minorCount >= 2;
				return majorCount - 2 <= minorCount + remain;
			}

			MaekawaValues& operator=(const MaekawaValues& right) {
				majorCount = right.majorCount;
				minorCount = right.minorCount;

				return *this;
			}
		};

		// TCharVector : should have "TChar operator[](const int& index)".
		// TCharVectorFactory : should have "TCharVector create(int size)"
		template<typename TOStream, bool needStats = true, typename TCharVector = std::vector<char>, typename TCharVectorFactory = mylib::CharVectorFactory>
		class FlapCPDirectEnumeration {
			TOStream& outStream;
			TCharVectorFactory factory;
			static const char FLAT = 0;
			static const char MAJOR = 1;
			static const char MINOR = 2;

			typedef KawasakiValues<FLAT, MAJOR, MINOR> KawasakiValues;
			typedef MaekawaValues<FLAT, MAJOR, MINOR> MaekawaValues;

			mylib::EnumerationStats stats;

			class Proxy {
				TCharVector& a;
				char a0;

			public:

				Proxy(TCharVector& a) : a(a), a0(0) {}

				char& operator[](const int& index) {
					return (index == 0) ? a0 : a[index - 1];
				}
				const char& operator[](const int& index) const {
					return (index == 0) ? a0 : a[index - 1];
				}

				TCharVector& get() {
					return a;
				}
			};

			int N, K;
			void generateAssignments(TCharVector& a_raw,
				int t, int p, int r, int u, int v, bool RS, const KawasakiValues& kawasaki, const MaekawaValues& maekawa) {
				if (needStats) {
					stats.callCount++;
					stats.validCallCount++;
				}
				Proxy a(a_raw);

				if (t - 1 > (N - r) / 2 + r) {
					if (a[t - 1] > a[N - t + 2 + r]) RS = false;
					else if (a[t - 1] < a[N - t + 2 + r]) RS = true;
				}
				if (t > N) {
					if ((RS == false) && (N%p == 0)) {
						if (!kawasaki.theoremHolds(N) || !maekawa.theoremHolds()) {
							return;
						}

						IsFoldableStringLinear<TCharVector, FLAT, MAJOR, MINOR> detector(N);
						if (detector.isAnswer(a.get())) {
							if (needStats) stats.answerCount++;
							outStream << a.get();
						}
					}
					return;
				}

				a[t] = a[t - p];
				auto nextKawasaki = kawasaki.nextValues(a, t, N);
				auto nextMaekawa = maekawa.nextValues(a, t);

				if (a[t] == a[1]) v++;
				else v = 0;
				if ((u == -1) && (a[t - 1] != a[1])) u = r = t - 2;

				if ((u != -1) && (t == N) && (a[N] == a[1])) {}
				else if (!nextKawasaki.theoremCanHold(t + 1, N)) {}
				else if (!nextMaekawa.theoremCanHold(nextKawasaki.computeRemain(t + 1, N))) {}
				else if (u == v) {
					auto rev = CheckRev(a, t, u);
					if (rev == 0) generateAssignments(a.get(), t + 1, p, r, u, v, RS, nextKawasaki, nextMaekawa);
					if (rev == 1) generateAssignments(a.get(), t + 1, p, t, u, v, false, nextKawasaki, nextMaekawa);
				}
				else generateAssignments(a.get(), t + 1, p, r, u, v, RS, nextKawasaki, nextMaekawa);

				if (a[t] == FLAT) {
					a[t] = a[t - p] + 1;
					nextKawasaki = kawasaki.nextValues(a, t, N);
				}
				if (!nextKawasaki.theoremCanHold(t + 1, N)) {
					return;
				}

				for (int j = a[t - p] + 1; j <= K - 1; ++j) {
					a[t] = j;
					nextMaekawa = maekawa.nextValues(a, t);

					if (!nextMaekawa.theoremCanHold(nextKawasaki.computeRemain(t + 1, N))) {
						continue;
					}

					generateAssignments(a.get(), t + 1, t, r, u, 0, RS, nextKawasaki, nextMaekawa);
				}
			}

			int CheckRev(const Proxy& a, int t, int i) const {
				int j;

				for (j = i + 1; j <= (t + 1) / 2; j++) {
					if (a[j] < a[t - j + 1]) return(0);
					else if (a[j] > a[t - j + 1]) return(-1);
				}
				return(1);
			}

		public:
			FlapCPDirectEnumeration(TOStream& os) : outStream(os) {
			}

			mylib::EnumerationStats enumerate(u_int placeCount, u_int elementCount = 3) {
				N = placeCount;
				K = elementCount;

				stats.clear();
				TCharVector a = factory.create(placeCount);

				generateAssignments(a, 1, 1, 1, -1, 0, false, KawasakiValues(), MaekawaValues());

				return stats;
			}

		};

	}
}