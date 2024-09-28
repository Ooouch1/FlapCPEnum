#pragma once
#include <vector>

namespace test {
	template<typename TSet_Assignment>
	class OutputReceiver {
	public:
		std::vector<TSet_Assignment> answers;

		OutputReceiver() {}

		OutputReceiver(const OutputReceiver<TSet_Assignment>& source) {
			answers = source.answers;
		}

		void clear() {
			answers.clear();
		}

		OutputReceiver<TSet_Assignment>& operator<<(const TSet_Assignment& p) {
			answers.push_back(p);

#ifdef DEBUG_PRINT_ANSWER
			std::cout << p.toString();
			std::cout << "-> answer!" << std::endl;
//			std::cout << std::endl;
#endif
			return *this;
		}
	};



}