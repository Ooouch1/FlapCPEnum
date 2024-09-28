#pragma once
#include "MVEnumeration.hpp"
#include "FlapPattern.hpp"

namespace enumeration {
	namespace origami {

		template<typename TSet_Assignment>
		class AbstractFlapCPAnswerDetecter : public mylib::IAnswerDetecter<TSet_Assignment> {

			unsigned long long int maekawaValidCount_;
			MaekawaTheorem<TSet_Assignment> maekawa;


		public:
			AbstractFlapCPAnswerDetecter(): maekawaValidCount_(0ULL) {}
			virtual ~AbstractFlapCPAnswerDetecter() {}

			virtual bool isAnswer(const TSet_Assignment& pattern) = 0;
			

			const unsigned long long int& maekawaValidCount() {
				return maekawaValidCount_;
			}

		protected:

			bool maekawaTheoremHolds(const TSet_Assignment& pattern) {
				if (maekawa.isAnswer(pattern)) {
					maekawaValidCount_++;
					return true;
				}

				return false;
			}


		};

		template<typename TSet_Assignment>
		class IFlapCPAnswerDetecterFactory {
		public:
			virtual ~IFlapCPAnswerDetecterFactory() {}
			virtual AbstractFlapCPAnswerDetecter<TSet_Assignment>* create(const EncodablePatternBase& flap) const = 0;
		};



	}
}