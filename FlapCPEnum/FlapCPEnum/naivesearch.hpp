#pragma once
#include "abbreviation.h"
#include "inverters.hpp"
#include <vector>

namespace naive {
	template<typename TSet_Assignment>
	class IPruningSuggester {
	public:
		virtual bool needPruning(const TSet_Assignment& pattern, int depth) const = 0;
	};

	template<typename TSet_Assignment>
	class IAnswerDetecter {
	public:
		virtual bool isAnswer(const TSet_Assignment& pattern) = 0;
	};


	template <typename TSet_Assignment>
	class AbstractDuplicationDetecter {

		template<typename FuncIntToInt>
		bool areSame(const TSet_Assignment& pattern, const TSet_Assignment& answer, const FuncIntToInt& invert) const {
			for (u_int i = 0; i < pattern.capacity(); i++) {
				auto inverse = invert(i);
				if (pattern.contains(i) != answer.contains(inverse)) {
					return false;
				}
			}
			return true;
		}

	protected:
		template <typename TIterator, typename FuncIntToInt>
		bool knownModificationExists(const TSet_Assignment& pattern,
			const TIterator& answersBegin, const TIterator& answersEnd, const FuncIntToInt& invert) const {

			for (auto answer = answersBegin; answer != answersEnd; answer++) {
				if (areSame(pattern, *answer, invert)) {
					return true;
				}
			}
			return false;
		}
	public:
		/**
		 * This method is expected to detect duplication of patterns using knownModificationExists().
		 * This is for flexible implementation of invserse function.
		 *
		 * knownModificationExists() should be called with given parameters and inverse functions of all possible f_i.
		 *
		 */
		template<typename TIterator>
		bool hasGenerated(const TSet_Assignment& pattern, const TIterator& answersBegin, const TIterator& answersEnd) const { return false; }


	};

	template<typename TSet_Assignment>
	class NoDuplication {
		template<typename TIterator>
		bool hasGenerated(const TSet_Assignment& pattern, const TIterator& foundsBegin, const TIterator& foundsEnd) const {
			return false;
		}
	};

	template<typename TSet_Assignment>
	class NoPruning : public IPruningSuggester<TSet_Assignment> {
	public:
		virtual bool needPruning(const TSet_Assignment& pattern, int depth) const {
			return false;
		}
	};


	template<typename TSet_Assignment>
	class EverythingIsAnswer : public IAnswerDetecter<TSet_Assignment> {
	public:
		virtual bool isAnswer(const TSet_Assignment& pattern) {
			return true;
		}
	};

	template<typename TSet_Assignment, typename TPruningSuggester, typename TAnswerDetecter, typename TDuplicationDetecter>
	class NaiveSearchTool {
		TPruningSuggester* pruningSuggester = NULL;
		TAnswerDetecter* answerDetecter = NULL;
		TDuplicationDetecter* duplicationDetecter = NULL;

	public:
		void setPruning(TPruningSuggester& p) {
			pruningSuggester = &p;
		}

		void setAnswer(TAnswerDetecter& a) {
			answerDetecter = &a;
		}

		void setDuplication(TDuplicationDetecter& d) {
			duplicationDetecter = &d;
		}

		bool needPruning(const TSet_Assignment& pattern, int depth) const {
			if (pruningSuggester == NULL) {
				return false;
			}
			return pruningSuggester->needPruning(pattern, depth);
		}

		bool isAnswer(const TSet_Assignment& pattern) {
			if (answerDetecter == NULL) {
				return true;
			}
			return answerDetecter->isAnswer(pattern);
		}

		template<typename TIterator>
		bool hasGenerated(const TSet_Assignment& pattern, const TIterator& foundsBegin, const TIterator& foundsEnd) const {
			if (duplicationDetecter == NULL) {
				return false;
			}
			return duplicationDetecter->hasGenerated(pattern, foundsBegin, foundsEnd);
		}
	};

	template <typename TSet_Assignment, typename TOStream, typename TSearchTool>
	class NaiveSearch {
	public:
	private:
		TOStream& outStream;
		TSearchTool& tool;
		std::vector<TSet_Assignment> answers;

	public:
		NaiveSearch(TOStream& os,
			TSearchTool& tool) :
			outStream(os),
			tool(tool) {

		}

		void enumerate(TSet_Assignment& current, int candBegin, int candEnd, u_int depth) {

			if (tool.hasGenerated(current, answers.begin(), answers.end())) {
				return;
			}

			if (tool.needPruning(current, depth)) {
				return;
			}

			if (tool.isAnswer(current)) {
				answers.push_back(current);
				outStream << current;
			}

			for (auto cand = candBegin; cand < candEnd; cand++) {
				current.add(cand);
				enumerate(current, cand + 1, candEnd, depth + 1);
				current.remove(cand);
			}
		}
	};
}