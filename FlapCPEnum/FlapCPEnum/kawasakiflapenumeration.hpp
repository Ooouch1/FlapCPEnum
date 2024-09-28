#pragma once
#include "FlapPattern.hpp"
#include "AsymmetryEnumeration.hpp"
#include "BraceletEnumeration.hpp"


namespace enumeration {
	namespace origami {

		// for implementing enumeration.
		// user doesn't have to care this class.
		template<typename TFlapPattern>
		class IsKawasakiFlap :public mylib::IAnswerDetecter<TFlapPattern> {
		public:
			virtual bool isAnswer(const TFlapPattern& pattern) {
				return pattern.kawasakiCountIsZero() && (pattern.count() % 2 == 0) && pattern.count() > 0;
			}

		};

		// for implementing enumeration.
		// user doesn't have to care this class.
		template<typename TFlapPattern>
		class KawasakiTheoremPruning : public mylib::IPruningSuggester<TFlapPattern> {
			virtual bool needPruning(const TFlapPattern& pattern, int depth) const {
				return !pattern.kawasakiCountCanBeZero();
			}
		};

		// for implementing enumeration.
		// user doesn't have to care this class.
		template<typename TFlapPattern>
		class NoKawasakiPruning : public mylib::IPruningSuggester<TFlapPattern> {
			virtual bool needPruning(const TFlapPattern& pattern, int depth) const {
				return false;
			}
		};

		template <bool needStats = true>
		class KawasakiFlapEnumeration {
			mylib::EnumerationStats stats;

		public:
			KawasakiFlapEnumeration() {}

			const mylib::EnumerationStats& kawasakiStats() {
				return stats;
			}

			// fast enumeration using bracelets
			template<typename TOStream>
			mylib::EnumerationStats enumerate(u_int placeCount, TOStream& os) {
				enumeration::circular::BinaryBraceletEnumeration<FlapPatternForBraceletEnum, TOStream, needStats> enumerator(os);
				IsKawasakiFlap<FlapPatternForBraceletEnum> answer;
				KawasakiTheoremPruning<FlapPatternForBraceletEnum> pruning;
				//mylib::NoPruning<FlapPatternForBraceletEnum> pruning;

				stats = enumerator.enumerate(placeCount, answer, pruning);

				return stats;

			}


			template<typename TOStream>
			mylib::EnumerationStats enumerateSlowly(u_int placeCount, TOStream& os) {
				enumeration::circular::AsymmetryEnumeration<TOStream, needStats> asymmetry;
				IsKawasakiFlap<FlapPatternForBraceletEnum> answer;
				KawasakiTheoremPruning<FlapPatternForBraceletEnum> pruning;

				stats = asymmetry.enumerate(placeCount, os, answer, pruning);

				return stats;
			}

			template<typename TOStream>
			mylib::EnumerationStats enumerateNoPruning(u_int placeCount, TOStream& os) {
				enumeration::circular::AsymmetryEnumeration<TOStream, needStats> asymmetry;
				IsKawasakiFlap<FlapPatternForBraceletEnum> answer;
				NoKawasakiPruning<FlapPatternForBraceletEnum> pruning;

				stats = asymmetry.enumerate(placeCount, os, answer, pruning);

				return stats;
			}

		};
	}
}
