#pragma once
#include "abbreviation.h"
#include "BitSet.hpp"
#include "searchtool.hpp"

namespace ppc {

	template <typename TSet_Assignment>
	class AbstractDuplicationDetecter {
		//unsigned long long int detectionCount;

	public:

		/**
		 * This method is expected to detect duplication of patterns using knownModificationExists().
		 * This is for flexible implementation of invserse function.
		 *
		 * knownModificationExists() should be called with given parameters and inverse functions of all possible f_i.
		 *
		 */
		virtual bool hasGenerated(const TSet_Assignment& pattern, const int elemEnd, const int prefixTail) const = 0;

		/**
		* Implementation of ppc extension test.
		* This method uses a function invert() for the test
		* instead of a set-to-set function f generating closure.
		* 
		* X < f(X) should hold lexicographically if X is a new pattern.
		*
		* pattern:       original set
		*
		* elemEnd :      the last index(of pattern) + 1.
		*
		* invert :    lambda(index) which returns the transformed index in the inverse of closure operation result.
		*
		* prefixTail:    prior of the last index in pattern.
		*/
		template <typename FuncIntToInt>
		bool knownModificationExists(const TSet_Assignment& pattern, const int elemEnd, const int prefixTail,
			const FuncIntToInt& invert) const {

			auto prefixEnd = prefixTail + 1;

			// PPC assumes X and f(X) has the same prefix.
			// The key idea is that the search can be pruned if f(X) < X holds lexicographically 
			// under an assumption that f transforms X to larger side.
			for (int modified = 0; modified < prefixEnd; modified++) {
				int i = invert(modified);

				// test: f(X) < X 
				// <=> a \notin X and a \in f(X)
				// <=> a \notin X and f^(-1)(a) \in X
				// search should be pruned.
				if (!pattern.contains(modified) && pattern.contains(i)) {
					return true;
				}

				// test: f(X) > X 
				// correct state of f(X).
				if (pattern.contains(modified) && !pattern.contains(i)) {
					return false;
				}

			}

			// test conventional PPC condition
			for (int modified = prefixEnd; modified < elemEnd; modified++) {
				int i = invert(modified);

				// test: f(X) < X 
				// search should be pruned.
				if (!pattern.contains(modified) && pattern.contains(i)) {
					return true;
				}
			}

			// identical pattern (no inverse)
			return false;
		}

		// provides iterative version in begin() end() manner. inverters should be a container of functions int->int.
		template<typename TInverters>
		bool knownModificationExistsFor(const TSet_Assignment& pattern, const int elemEnd, const int prefixTail, const TInverters& inverters) const {
			for (auto itr = inverters.begin(); itr != inverters.end(); ++itr) {
				if (knownModificationExists(pattern, elemEnd, prefixTail, *itr)) {
					return true;
				}
			}

			return false;
		}
	};

	template<typename TSet_Assignment>
	class NoDuplication : public AbstractDuplicationDetecter<TSet_Assignment> {
		virtual bool hasGenerated(const TSet_Assignment& pattern, const int elemEnd, const int prefixTail) const {
			return false;
		}
	};

	template<typename TSet_Assignment>
	class PPCSearchTool {
		const mylib::IPruningSuggester<TSet_Assignment>* pruningSuggester = NULL;
		mylib::IAnswerDetecter<TSet_Assignment>* answerDetecter = NULL;
		AbstractDuplicationDetecter<TSet_Assignment>* duplicationDetecter = NULL;

	public:
		void setPruning(const mylib::IPruningSuggester<TSet_Assignment>& p) {
			pruningSuggester = &p;
		}

		void setAnswer(mylib::IAnswerDetecter<TSet_Assignment>& a) {
			answerDetecter = &a;
		}

		void setDuplication(AbstractDuplicationDetecter<TSet_Assignment>& d) {
			duplicationDetecter = &d;
		}

		bool needPruning(const TSet_Assignment& pattern, int depth) const {
			if (pruningSuggester == NULL) {
				return false;
			}
			return pruningSuggester->needPruning(pattern, depth);
		}

		bool isAnswer(const TSet_Assignment& pattern) const {
			if (answerDetecter == NULL) {
				return true;
			}
			return answerDetecter->isAnswer(pattern);
		}

		bool hasGenerated(const TSet_Assignment& pattern, const int elemEnd, const int prefixTail) const {
			if (duplicationDetecter == NULL) {
				return false;
			}
			return duplicationDetecter->hasGenerated(pattern, elemEnd, prefixTail);
		}
	};

	/**
	 * This class implements an extended PPC search.
	 * PPC condition is relaxed as:
	 *
	 *  - we have some function f(X) which MAY NOT hold X \in f(X) while original LCM assumes X \in f(X).
	 *
	 *  - we can detect semantic duplication (such as symmetry among patterns)
	 *    by testing PPC condition between X and f^-1(X).
	 *
	 *  - There can be several f_i(X) if you want to reduce patterns by different relations.
	 *    If some f_i^-1(X) violates PPC condition, that is a duplication.
	 *
	 * TSet: A set presentation implementing mylib::ISet.
	*/
	template <typename TSet_Assignment, typename TOStream, bool needStats = true>
	class ExtendedPPCSearch {
		//mylib::BitSet garbages;
		TOStream& outStream;
		const PPCSearchTool<TSet_Assignment>& tool;
		mylib::EnumerationStats stats_;

	public:
		ExtendedPPCSearch(TOStream& os,
			const PPCSearchTool<TSet_Assignment>& tool) :
			outStream(os),
			tool(tool) {

		}

		const mylib::EnumerationStats& stats() {
			return stats_;
		}

		void enumerate(TSet_Assignment& current, int candBegin, int candEnd, int prefixTail, u_int depth) {

			if (needStats) {
				stats_.callCount++;
			}

			if (tool.hasGenerated(current, candBegin, prefixTail)) {
				return;
			}

			if (tool.needPruning(current, depth)) {
				return;
			}

			if (needStats) {
				stats_.validCallCount++;
			}

				if (tool.isAnswer(current)) {
					if (needStats) {
						stats_.answerCount++;
					}
					outStream << current;
				}

			auto nextPrefixTail = candBegin - 1;

			for (auto cand = candBegin; cand < candEnd; cand++) {
				current.add(cand);
				enumerate(current, cand + 1, candEnd, nextPrefixTail, depth + 1);
				current.remove(cand);
				//garbages.add(cand);
			}

			//garbages.remove(candBegin, candEnd);
		}
	};


}
