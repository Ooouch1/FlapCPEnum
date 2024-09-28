#pragma once

#include "AbstractFlapCPAnswerDetecter.hpp"

namespace enumeration {
	namespace origami {
		template<typename TSet_Assignment>
		class StatsMaekawaTheorem : public AbstractFlapCPAnswerDetecter<TSet_Assignment> {

		public:
			StatsMaekawaTheorem() : AbstractFlapCPAnswerDetecter<TSet_Assignment>() {
			}
			virtual ~StatsMaekawaTheorem() {}


			virtual bool isAnswer(const TSet_Assignment& pattern) {
				return this->maekawaTheoremHolds(pattern);
			}

		};


		template<typename TSet_Assignment>
		class MaekawaTheoremFactory : public IFlapCPAnswerDetecterFactory<TSet_Assignment> {
		public:
			virtual ~MaekawaTheoremFactory() {}

			virtual StatsMaekawaTheorem<TSet_Assignment>* create(const EncodablePatternBase& flap) const {
				return new StatsMaekawaTheorem<TSet_Assignment>();
			}
		};


	}
}