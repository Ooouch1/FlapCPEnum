#pragma once
#include "gtest/gtest.h"
#include "BitSet.hpp"

class SetTestBase : public ::testing::Test {
protected:

	template<typename TSet_Assignment>
	void assertContains(const TSet_Assignment& aSet, int item, const std::string message = "") {
		ASSERT_TRUE(aSet.contains(item)) << message;
	}

	template<typename TSet_Assignment>
	void assertNotContains(const TSet_Assignment& aSet, int item) {
		ASSERT_FALSE(aSet.contains(item));
	}

	template<typename TSet_Assignment>
	void assertContainsNothing(const TSet_Assignment& aSet, int begin, int end) {
		ASSERT_TRUE(aSet.containsNothing(begin, end));
	}

	template<typename TSet_Assignment>
	void assertContainsAll(const TSet_Assignment& aSet, int begin, int end) {
		ASSERT_TRUE(aSet.containsAll(begin, end));
	}
};