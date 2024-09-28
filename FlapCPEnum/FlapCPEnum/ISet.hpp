#pragma once
#include "abbreviation.h"
#include <string>

namespace mylib {

	template<typename Value>
	class ISet {
	public:
		virtual ~ISet() {}
		virtual bool contains(const Value& item) const = 0;

		virtual void add(const Value& item) = 0;

		virtual void remove(const Value& item) = 0;

		/**
		 * count of items in this set.
		 */
		virtual const u_int& count() const = 0;

		/**
		 * max size of this set, which is different from std usage.
		 * this is for fast computation.
		 * if TValue is integer, capacity will be max value + 1.
		 */
		virtual const u_int& capacity() const = 0;

		virtual std::string toString() const = 0;
	};

}