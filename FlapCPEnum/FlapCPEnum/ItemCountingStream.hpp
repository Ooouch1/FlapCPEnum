#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <sstream>
#include "ISet.hpp"

namespace enumeration {

	template <typename TKey, typename Value>
	struct Countable {
		const TKey counterKey;
		const Value value;

		Countable(const TKey& c, const Value& v) : counterKey(c), value(v) {

		}

		Countable(const Countable<TKey, Value>& source) : counterKey(source.counterKey), value(source.value) {}
	};

	template <typename TKey, typename Value>
	Countable<TKey, Value> createCountable(const TKey c, const Value& v) {
		return Countable<TKey, Value>(c, v);
	}




	// wrapper for counting outputs.
	// if some stream is given, this object passes input of <<() to the stream as well.
	template <typename TOStream = std::ostream>
	class ItemCountingStream {
	public:
		typedef long long unsigned int Count;

	private:
		std::vector<Count> counts;
		std::shared_ptr<TOStream> out;
	public:
		ItemCountingStream(int size) : counts(size, 0) {}
		ItemCountingStream(std::shared_ptr<TOStream>& os, int size) : counts(size, 0) {
			setStream(os);
		}

		void setStream(std::shared_ptr<TOStream>& os) {
			out = os;
		}

		template<typename TObject>
		ItemCountingStream<TOStream>& operator<<(const TObject& obj) {
			counts[0]++;
			if (out != nullptr)
				(*out) << obj;

			return *this;
		}

		template<typename Value>
		ItemCountingStream<TOStream>& operator<<(const mylib::ISet<Value>& aSet) {
			counts[aSet.count()]++;
			if (out != nullptr)
				(*out) << aSet.toString();

			return *this;
		}

		template<typename TKey, typename Value>
		ItemCountingStream<TOStream>& operator<<(const Countable<TKey, Value>& item) {
			counts[item.counterKey]++;
			if (out != nullptr)
				(*out) << item.value;

			return *this;
		}

		const Count& count(const int index = 0) {
			return counts[index];
		}

		std::string toString() {
			std::ostringstream stream;

			stream << total() << " (";

			std::for_each(counts.begin(), counts.end(),
				[&stream](const Count& v) {
				stream << v << "\t"; });

			stream << ")";

			return stream.str();
		}

		Count total() {
			Count total = 0;
			for (const auto& count : counts) {
				total += count;
			}
			return total;
		}
	};

	class CountOnlyStream : public ItemCountingStream<> {
	};
}
