#pragma once


namespace mylib {

	// for analyzing performance
	struct EnumerationStats {
		unsigned long long int callCount, validCallCount, answerCount;

		EnumerationStats() : callCount(0ULL), validCallCount(0ULL), answerCount(0ULL) {}

		EnumerationStats operator+(const EnumerationStats& right) const {
			auto result = *this;

			result.callCount += right.callCount;
			result.validCallCount += right.validCallCount;
			result.answerCount += right.answerCount;

			return result;
		}

		void operator+=(const EnumerationStats& right) {
			callCount += right.callCount;
			validCallCount += right.validCallCount;
			answerCount += right.answerCount;
		}

		long double searchEfficiency() const {
			return (long double)answerCount / validCallCount;
		}

		long double pruningRate() const {
			return (long double)validCallCount / callCount;
		}

		void clear() {
			validCallCount = 0ULL;
			callCount = 0ULL; 
			answerCount = 0ULL;
		}
	};



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

	template<typename TSet_Assignment>
	class NoPruning : public IPruningSuggester<TSet_Assignment> {
	public:
		virtual bool needPruning(const TSet_Assignment& pattern, int depth) const {
			return false;
		}
	};


	// should use "using" in C++11 for aliasing
	template<typename TSet_Assignment>
	class EverythingIsAnswer : public IAnswerDetecter<TSet_Assignment> {
	public:
		virtual bool isAnswer(const TSet_Assignment& pattern) {
			return true;
		}
	};

	template <typename TSet_Assignment>
	class NotEmptyIsAnswer : public IAnswerDetecter<TSet_Assignment> {
	public:
		virtual bool isAnswer(const TSet_Assignment& pattern) {
			return pattern.count() > 0;
		}
	};

	//template<typename TSet>
	//class SearchTool {
	//	const IPruningSuggester<TSet>* pruningSuggester = NULL;
	//	IAnswerDetecter<TSet>* answerDetecter = NULL;
	//	AbstractDuplicationDetecter<TSet>* duplicationDetecter = NULL;

	//public:
	//	void setPruning(const IPruningSuggester<TSet>& p) {
	//		pruningSuggester = &p;
	//	}

	//	void setAnswer(IAnswerDetecter<TSet>& a) {
	//		answerDetecter = &a;
	//	}

	//	void setDuplication(AbstractDuplicationDetecter<TSet>& d) {
	//		duplicationDetecter = &d;
	//	}

	//	bool needPruning(const TSet& pattern, int depth) const {
	//		if (pruningSuggester == NULL) {
	//			return false;
	//		}
	//		return pruningSuggester->needPruning(pattern, depth);
	//	}

	//	bool isAnswer(const TSet& pattern) const {
	//		if (answerDetecter == NULL) {
	//			return true;
	//		}
	//		return answerDetecter->isAnswer(pattern);
	//	}
	//};

}