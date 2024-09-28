#pragma once

#include <vector>
#include <map>
#include <algorithm>

namespace mylib {

	class DiffTool {

	public:
		template<typename Value, typename SeqName, typename ValueItr>
		static std::map<Value, SeqName> diffBetweenUniques(
			const ValueItr& begin_a, const ValueItr& end_a, const SeqName name_a,
			const ValueItr& begin_b, const ValueItr& end_b, const SeqName name_b) {
			
			using namespace std;
			map<Value, SeqName> diffs;

			auto diffAction = [&](const Value& val, const SeqName& name) {
				auto& key = val;
				auto& value = name;

				auto itr = diffs.find(key);
				if (itr == diffs.end()) {
					diffs[key] = value;
				}
				else {
					diffs.erase(key);
				}
			};

			for (auto itr = begin_a; itr != end_a; ++itr) {
				diffAction(*itr, name_a);
			}
			for (auto itr = begin_b; itr != end_b; ++itr) {
				diffAction(*itr, name_b);
			}

			return diffs;
		}

	};

}