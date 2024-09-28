#pragma once

#include <string>
#include <map>
#include <algorithm>

namespace mylib {
	class Trie {
		const int ALPHABET_SIZE;

		// trie node
		struct TrieNode
		{
			struct TrieNode **children;
			int maxIndex;

			// isEndOfWord is true if the node represents
			// end of a word
			bool isEndOfWord;

			TrieNode(): maxIndex(-1) {}

			~TrieNode() {
				for (int i = 0; i < maxIndex; i++)
					delete children[i];
				delete[] children;
			}
		};

		// Returns new trie node (initialized to NULLs)
		struct TrieNode *createNode(void)
		{
			struct TrieNode *pNode = new TrieNode;
			pNode->children = new TrieNode*[ALPHABET_SIZE];
			pNode->isEndOfWord = false;

			for (int i = 0; i < ALPHABET_SIZE; i++)
				pNode->children[i] = NULL;

			return pNode;
		}

		// If not present, inserts key into trie
		// If the key is prefix of trie node, just
		// marks leaf node
		void insert(struct TrieNode *root, const std::string &key)
		{
			struct TrieNode *pCrawl = root;

			for (int i = 0; i < key.length(); i++)
			{
				int index = key[i] - minValue;
				
				if (index > pCrawl->maxIndex)
					pCrawl->maxIndex = index;

				if (!pCrawl->children[index])
					pCrawl->children[index] = createNode();

				pCrawl = pCrawl->children[index];
			}

			// mark last node as leaf
			pCrawl->isEndOfWord = true;
		}

		// Returns true if key presents in trie, else
		// false
		bool search(struct TrieNode *root, const std::string &key) const
		{
			struct TrieNode *pCrawl = root;

			for (int i = 0; i < key.length(); i++)
			{
				char index = key[i] - minValue;
				if (!pCrawl->children[index])
					return false;

				pCrawl = pCrawl->children[index];
			}

			return (pCrawl != NULL && pCrawl->isEndOfWord);
		}

		TrieNode *root;
		const char minValue;
	public:

		Trie(int alphabetSize = 64, char minVal = 0): ALPHABET_SIZE(alphabetSize), minValue(minVal) {
			root = createNode();
		}

		~Trie() {
			delete root;
		}

		bool contains(const std::string  &key) const {
			return search(root, key);
		}

		void insert(const std::string &key) {
			insert(root, key);
		}
	};

	class PatriciaTrie {
		const int ALPHABET_SIZE;

		// trie node
		struct TrieNode
		{
			std::string label;

			struct TrieNode **children;
			int maxIndex;

			// isEndOfWord is true if the node represents
			// end of a word
			bool isEndOfWord;

			TrieNode() : maxIndex(-1) , label("") {}

			~TrieNode()  {
				for (int i = 0; i < maxIndex; i++)
					delete children[i];
				delete[] children;
			}
		};

		// Returns new trie node (initialized to NULLs)
		struct TrieNode *createNode(void)
		{
			struct TrieNode *pNode = new TrieNode;
			pNode->children = new TrieNode*[ALPHABET_SIZE];
			pNode->isEndOfWord = false;

			for (int i = 0; i < ALPHABET_SIZE; i++)
				pNode->children[i] = NULL;

			return pNode;
		}

		std::string readableLabel(const std::string& s) const {
			std::string readable(s);
			//for (auto& c : readable) {
			//	c = (char)(c - minValue + 'a');
			//}
			return readable;
		}

		TrieNode* createSuffixNode(const std::string& key, int keyHead) {
			auto pChild = createNode();
			pChild->label = key.substr(keyHead);
			pChild->isEndOfWord = true;

			return pChild;
		}

		void visitChild(struct TrieNode *pCrawl, const int& index, const std::string& key, int keyHead) {

			pCrawl->maxIndex = std::max(pCrawl->maxIndex, index);

			auto pChild = pCrawl->children[index];
			if (pChild == NULL) {
				pChild = createSuffixNode(key, keyHead);
				pCrawl->children[index] = pChild;

				//std::cout << "new label: " << readableLabel(pChild->label) 
				//	<< " under " << readableLabel(pCrawl->label) << std::endl;
				return;
			}

			insert(pChild, key, keyHead);
			
		}

		void splitNode(TrieNode *pCrawl, int prefixLength) {
			auto suf = createSuffixNode(pCrawl->label, prefixLength);
			pCrawl->label.resize(prefixLength);
			pCrawl->isEndOfWord = false;

			int index = suf->label[0] - minValue;
			pCrawl->maxIndex = std::max(pCrawl->maxIndex, index);

			// push down chidren
			std::swap(suf->children, pCrawl->children);
			pCrawl->children[index] = suf;
		}

		int countPrefix(const std::string& label, const std::string& key, const int& keyHead) const {
			int prefixLength = 0;
			auto minLen = std::min(label.length(), key.length() - keyHead);
			while (label[prefixLength] == key[keyHead + prefixLength] &&
				prefixLength < minLen) {
				prefixLength++;
			}
			return prefixLength;
		}

		// If not present, inserts key into trie
		// If the key is prefix of trie node, just
		// marks leaf node
		void insert(struct TrieNode *root, const std::string &key, const int keyHead = 0)
		{
			TrieNode *pCrawl = root;

			//std::cout << "(insert) visit label:" << readableLabel(pCrawl->label) 
			//	<< ", isEnd:" << pCrawl->isEndOfWord << ", key:" << readableLabel(key.substr(keyHead)) << std::endl;

			int prefixLength = 0;
			if (pCrawl->label.empty()) {
			}
			{
				// prefix matching
				prefixLength = countPrefix(pCrawl->label, key, keyHead);

				// existing node represents the given key
				if (prefixLength == pCrawl->label.length() && prefixLength == (key.length() - keyHead)) {
					pCrawl->isEndOfWord = true;
					return;
				}

				//std::cout  << "prefixLength:" << prefixLength
				//	<< ", label:" << readableLabel(pCrawl->label)
				//	<< ", key:" << readableLabel(key.substr(keyHead)) << std::endl;

				// put label suffix to leaf
				if (prefixLength < pCrawl->label.length()) {
					splitNode(pCrawl, prefixLength);
				}
			}

			// put key suffix to leaf
			//std::cout << "cut key " << readableLabel(key.substr(keyHead)) 
			//	<< ", label:" << readableLabel(pCrawl->label)
			//	<< ", prefixLength:" << prefixLength << std::endl;
			int index = key[keyHead + prefixLength] - minValue;
			visitChild(pCrawl, index, key, keyHead + prefixLength);
		}

		// Returns true if key presents in trie, else
		// false
		bool search(struct TrieNode *root, const std::string &key, int keyHead = 0) const
		{
			TrieNode *pCrawl = root;

			//std::cout << "(search) visit label:" << readableLabel(pCrawl->label)
			//	<< ", isEnd:" << pCrawl->isEndOfWord << ", key:" << readableLabel(key.substr(keyHead)) << std::endl;

			// prefix matching
			int prefixLength = countPrefix(pCrawl->label, key, keyHead);

			// perfect match
			if (prefixLength == pCrawl->label.length() && prefixLength == (key.length() - keyHead))
				return pCrawl->isEndOfWord;

			// matching failed
			if (prefixLength < pCrawl->label.length())
				return false;

			char index = key[keyHead + prefixLength] - minValue;
			if (!pCrawl->children[index])
				return false;

			// visit child
			return search(pCrawl->children[index], key, keyHead + prefixLength);

		}

		TrieNode *root;
		const char minValue;
	public:

		PatriciaTrie(int alphabetSize = 64, char minVal = 0) : ALPHABET_SIZE(alphabetSize), minValue(minVal) {
			root = createNode();
		}

		~PatriciaTrie() {
			delete root;
		}

		bool contains(const std::string  &key) const {
			return search(root, key);
		}

		void insert(const std::string &key) {
			insert(root, key);
		}
	};


	class MapTrie {
		const int ALPHABET_SIZE;

		// trie node
		struct TrieNode
		{
			std::map<char, struct TrieNode *> children;

			// isEndOfWord is true if the node represents
			// end of a word
			bool isEndOfWord;

			~TrieNode() {
				for (auto &child : children) {
					delete child.second;
				}
			}
		};

		// Returns new trie node (initialized to NULLs)
		struct TrieNode *createNode(void)
		{
			struct TrieNode *pNode = new TrieNode;
			pNode->isEndOfWord = false;

			return pNode;
		}

		// If not present, inserts key into trie
		// If the key is prefix of trie node, just
		// marks leaf node
		void insert(struct TrieNode *root, const std::string &key)
		{
			struct TrieNode *pCrawl = root;

			for (int i = 0; i < key.length(); i++)
			{
				char index = key[i] - minValue;
				auto itrChild = pCrawl->children.find(index);

				if (itrChild == pCrawl->children.end()) {
					auto child = createNode();
					pCrawl->children.insert(std::make_pair(index, child));
					pCrawl = child;
				}
				else {
					pCrawl = itrChild->second;
				}
			}

			// mark last node as leaf
			pCrawl->isEndOfWord = true;
		}

		// Returns true if key presents in trie, else
		// false
		bool search(struct TrieNode *root, const std::string &key)
		{
			struct TrieNode *pCrawl = root;

			for (int i = 0; i < key.length(); i++)
			{
				int index = key[i] - minValue;
				auto itrChild = pCrawl->children.find(index);
				if (itrChild == pCrawl->children.end())
					return false;

				pCrawl = itrChild->second;
			}

			return (pCrawl != NULL && pCrawl->isEndOfWord);
		}

		TrieNode *root;
		const char minValue;
	public:

		MapTrie(int alphabetSize = 64, char minVal = 0) : ALPHABET_SIZE(alphabetSize), minValue(minVal) {
			root = createNode();
		}

		~MapTrie() {
			delete root;
		}

		bool contains(const std::string  &key) {
			return search(root, key);
		}

		void insert(const std::string &key) {
			insert(root, key);
		}
	};

}

