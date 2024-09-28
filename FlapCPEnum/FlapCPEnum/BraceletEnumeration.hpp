#pragma once

#include <vector>
#include "CharVecFactory.hpp"
#include "searchtool.hpp"
#include "abbreviation.h"

namespace enumeration {

	namespace circular {

		/**
		 * Algorithm from "GENERATING BRACELETS IN CONSTANT AMORTIZED TIME (Joe Sawada, 2001, SIAM J. COMPUT, Vol. 31, No. 1, pp.259-268)".
		 * This is a slight modification of http://skeeter.socs.uoguelph.ca/~sawada/prog/necklaces.c
		 *
		 * Thank you Joe!
		 *
		 */

		template<typename TSet_Assignment, typename TOStream, bool needStats = true>
		class BinaryBraceletEnumeration {

			TOStream& outStream;

			mylib::EnumerationStats stats;
			mylib::IAnswerDetecter<TSet_Assignment> *ansDetecter;
			const mylib::IPruningSuggester<TSet_Assignment> *pruningSuggester;


			class SetProxy {
				int a0;
				TSet_Assignment& a;

			public:
				static const int ADDED = 1;
				static const int NONE = 0;
				static const int REMOVED = -1;

				SetProxy(TSet_Assignment& a) : a(a), a0(0) {}

				TSet_Assignment& get() {
					return a;
				}

				void add(int item) {
					if (item == 0) a0 = 1;
					else a.add(item - 1);
				}

				void remove(int item) {
					if (item == 0) a0 = 0;
					else a.remove(item - 1);
				}

				int ref(int item) const {
					if (item == 0) return a0;
					return (a.contains(item - 1) ? 1 : 0);
				}

				/**
				 * return:
				 *  1: added (ADDED)
				 *  0: no changes (NONE)
				 * -1: removed (REMOVED)
				 */
				int set(u_int item, int val) {
					if (ref(item) == 1 && val == 0) {
						remove(item);
						return REMOVED;
					}
					else if (ref(item) == 0 && val == 1) {
						add(item);
						return ADDED;
					}

					return NONE;
				}

			};


			int CheckRev(const SetProxy& a, int t, int i) {
				for (int j = i + 1; j <= (t + 1) / 2; j++) {
					if (a.ref(j) < a.ref(t - j + 1)) return(0);
					else if (a.ref(j) > a.ref(t - j + 1)) return(-1);
				}
				return(1);
			}

			int N;
			template<typename TSet_Assignment_>
			void GenB(TSet_Assignment_ &a_raw, int t, int p, int r, int u, int v, bool RS) {
				SetProxy a(a_raw);

				if (needStats) {
					stats.callCount++;
				}
				if (pruningSuggester->needPruning(a.get(), t - 1)) {
					return;
				}
				if (needStats) {
					stats.validCallCount++;
				}

				if (t - 1 > (N - r) / 2 + r) {
					if (a.ref(t - 1) > a.ref(N - t + 2 + r)) RS = false;
					else if (a.ref(t - 1) < a.ref(N - t + 2 + r)) RS = true;
				}

				// answer?
				if (t > N) {
					if ((RS == false) && (N%p == 0)) {
						if (!ansDetecter->isAnswer(a.get())) return;

						if (needStats) {
							stats.answerCount++;
						}
						outStream << a.get();
					}
					return;
				}

				auto setResult = a.set(t, a.ref(t - p)); // a[t] = a[t - p];

				if (a.ref(t) == a.ref(1)) v++;
				else v = 0;
				if ((u == -1) && (a.ref(t - 1) != a.ref(1))) u = r = t - 2;

				if ((u != -1) && (t == N) && (a.ref(N) == a.ref(1))) {}
				else if (u == v) {
					int rev = CheckRev(a, t, u);
					if (rev == 0) GenB(a, t + 1, p, r, u, v, RS);
					if (rev == 1) GenB(a, t + 1, p, t, u, v, false);
				}
				else GenB(a.get(), t + 1, p, r, u, v, RS);

				if (setResult == SetProxy::ADDED) {
					a.remove(t);
				}
				else if (setResult == SetProxy::REMOVED) {
					a.add(t);
				}

				//				for (int j = a.ref(t - p) + 1; j <= 1; ++j) {
				if (a.ref(t - p) == 0) {
					a.add(t);
					//a.set(t, j);//a.add(t)
					GenB(a.get(), t + 1, t, r, u, 0, RS);

					a.remove(t);
				}
			}


		public:
			BinaryBraceletEnumeration(TOStream& os) : outStream(os) {
			}
			/**
			* return: call count of recursive function.
			*/
			mylib::EnumerationStats enumerate(const u_int placeCount,
				mylib::IAnswerDetecter<TSet_Assignment>& ansDetecter,
				const mylib::IPruningSuggester<TSet_Assignment>& pruning) {

				this->ansDetecter = &ansDetecter;
				this->pruningSuggester = &pruning;

				TSet_Assignment seed(placeCount);
				//seed.add(0);

				N = placeCount;
				GenB(seed, 1, 1, 1, -1, 0, false);

				return stats;
			}

			mylib::EnumerationStats enumerate(u_int placeCount) {
				mylib::EverythingIsAnswer<TSet_Assignment> ansDetecter;
				mylib::NoPruning<TSet_Assignment> pruning;

				return this->enumerate(placeCount, ansDetecter, pruning);
			}


		};




		// TCharVector : should have "TChar operator[](const int& index)".
		// TCharVectorFactory : should have "TCharVector create(int size)"
		template<typename TOStream, typename TCharVector = std::vector<char>, 
			typename TCharVectorFactory = mylib::CharVectorFactory>
		class BraceletEnumeration {
			TOStream& outStream;
			TCharVectorFactory factory;

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
			void GenB(TCharVector& a_raw, int t, int p, int r, int u, int v, int RS) {
				Proxy a(a_raw);

				if (t - 1 > (N - r) / 2 + r) {
					if (a[t - 1] > a[N - t + 2 + r]) RS = false;
					else if (a[t - 1] < a[N - t + 2 + r]) RS = true;
				}
				if (t > N) {
					if ((RS == false) && (N%p == 0)) outStream << a.get();

					return;
				}

				a[t] = a[t - p];
				if (a[t] == a[1]) v++;
				else v = 0;
				if ((u == -1) && (a[t - 1] != a[1])) u = r = t - 2;

				if ((u != -1) && (t == N) && (a[N] == a[1])) {}
				else if (u == v) {
					auto rev = CheckRev(a, t, u);
					if (rev == 0) GenB(a.get(), t + 1, p, r, u, v, RS);
					if (rev == 1) GenB(a.get(), t + 1, p, t, u, v, false);
				}
				else GenB(a.get(), t + 1, p, r, u, v, RS);
				for (int j = a[t - p] + 1; j <= K - 1; ++j) {
					a[t] = j;
					GenB(a.get(), t + 1, t, r, u, 0, RS);
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
			BraceletEnumeration(TOStream& os) : outStream(os) {
			}

			void enumerate(u_int placeCount, u_int elementCount) {
				N = placeCount;
				K = elementCount;

				TCharVector a = factory.create(placeCount);

				GenB(a, 1, 1, 1, -1, 0, false);
			}

		};

	}
}
