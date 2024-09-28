#include "gtest/gtest.h"
#include "Trie.hpp"
#include<string>

namespace {

	class TrieTest : public ::testing::Test {
	};
	class PatriciaTrieTest : public ::testing::Test {
	};
	class MapTrieTest: public ::testing::Test{
	};

	TEST(TrieTest, testInsertAndFind) {
		std::string keys[] = { "the", "a", "there",
			"answer", "any", "by",
			"bye", "their" };
		int n = sizeof(keys) / sizeof(keys[0]);

		mylib::Trie trie(26, 'a'); // assume English text
		
		// Construct trie
		for (int i = 0; i < n; i++)
			trie.insert(keys[i]);

		// Search for different keys
		ASSERT_TRUE(trie.contains("the"));
		ASSERT_FALSE(trie.contains("these"));
	}

	TEST(MapTrieTest, testInsertAndFind) {
		std::string keys[] = { "the", "a", "there",
			"answer", "any", "by",
			"bye", "their" };
		int n = sizeof(keys) / sizeof(keys[0]);

		mylib::MapTrie trie(26, 'a'); // assume English text

								   // Construct trie
		for (int i = 0; i < n; i++)
			trie.insert(keys[i]);

		// Search for different keys
		ASSERT_TRUE(trie.contains("the"));
		ASSERT_FALSE(trie.contains("these"));
	}

	TEST(PatriciaTrieTest, testInsertAndFind) {
		std::string keys[] = { "the", "there", "by",
			"bye", "their" };
		int n = sizeof(keys) / sizeof(keys[0]);

		mylib::PatriciaTrie trie(26, 'a'); // assume English text

									  // Construct trie
		for (int i = 0; i < n; i++)
			trie.insert(keys[i]);

		// Search for different keys
		ASSERT_TRUE(trie.contains("there"));
		ASSERT_TRUE(trie.contains("the"));

		ASSERT_FALSE(trie.contains("these"));

		ASSERT_TRUE(trie.contains("by"));

		ASSERT_FALSE(trie.contains("c"));
	}

	TEST(PatriciaTrieTest, testLongerWordInsertedThenShorter) {
		std::string keys[] = { 
			"as", "at", 
			"answer", "any", "assume", "a"
		};
		int n = sizeof(keys) / sizeof(keys[0]);

		mylib::PatriciaTrie trie(26, 'a'); // assume English text

										   // Construct trie
		for (int i = 0; i < n; i++)
			trie.insert(keys[i]);

		// Search for different keys
		ASSERT_TRUE(trie.contains("a"));
		ASSERT_TRUE(trie.contains("any"));
		ASSERT_TRUE(trie.contains("as"));
		ASSERT_TRUE(trie.contains("answer"));
		ASSERT_TRUE(trie.contains("assume"));
		ASSERT_TRUE(trie.contains("at"));

		ASSERT_FALSE(trie.contains("an"));
		ASSERT_FALSE(trie.contains("aswer"));

	}
}
