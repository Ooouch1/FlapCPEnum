#pragma once
#include <memory>

namespace mylib {
	template<typename Value>
	class SharedArrayPointer {
		std::shared_ptr<Value> shared;

	public:

		SharedArrayPointer() {}

		SharedArrayPointer(const SharedArrayPointer<Value>& source) {
			shared = source.shared;
		}

		SharedArrayPointer(const unsigned int size) : shared(new Value[size], std::default_delete<Value[]>()) {
		}

		Value& operator[] (const unsigned int index) {
			return shared.get()[index];
		}

		const Value& operator[] (const unsigned int index) const {
			return shared.get()[index];
		}

		Value* getRawArray() {
			return shared.get();
		}

		u_int useCount() {
			return shared.use_count();
		}
	};
}