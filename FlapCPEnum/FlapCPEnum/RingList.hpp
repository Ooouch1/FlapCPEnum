#pragma once

#include "ListProperty.hpp"
#include "abbreviation.h"

#include <memory>
#include <vector>
#include <stdexcept>
#include <sstream>

namespace mylib {

	// For implementing RingList.
	// user doesn't have to care this class.
	template<typename Value>
	class RingListNode {
		//public:
		//	typedef std::shared_ptr<RingListNode<TValue> > SharedNodePointer;

	private:
		ListProperty<RingListNode<Value>>& parent;
		Value item;


	public:
		RingListNode<Value> *prevNode;
		RingListNode<Value> *nextNode;

		RingListNode(ListProperty<RingListNode<Value>>& property) : parent(property) {
		}

		RingListNode(const Value& value,
			ListProperty<RingListNode<Value>>& property) : item(value), parent(property) {
		}

		void set(const Value& value) {
			item = value;
		}

		Value& operator*() {
			return item;
		}

		~RingListNode() {
		}

		// insert given item as a new node
		// between this instance and its next node.
		//
		// item: the item of the new node
		void insertNext(RingListNode<Value> * const newNext) {
			auto oldNext = nextNode;

			// set relation for new node
			newNext->prevNode = this;
			newNext->nextNode = nextNode;

			// update oldies
			nextNode = newNext;
			oldNext->prevNode = newNext;

			// update tail
			if (parent.isTail(this)) {
				parent.setTail(newNext);
			}

			parent.countUp();
		}

		// returns next node
		void leave() {
			const auto prev_ptr = prevNode;
			const auto next_ptr = nextNode;

			// release shared_ptr
			//nextNode = NULL;
			//prevNode = NULL;

			if (parent.count() == 1) {
				// clear list
				parent.setHead(NULL);
				parent.setTail(NULL);
			}
			else {
				if (parent.count() == 2) {
					// make self-loop of next node
					next_ptr->nextNode = next_ptr;
					next_ptr->prevNode = next_ptr;
				}
				else {
					// connect prev and next
					prev_ptr->nextNode = next_ptr;
					next_ptr->prevNode = prev_ptr;
				}

				// shift head
				if (parent.isHead(this)) {
					parent.setHead(next_ptr);
				}
				// shift tail
				else if (parent.isTail(this)) {
					parent.setTail(prev_ptr);
				}
			}

			parent.countDown();
		}
	};

	template<typename TNode>
	class NodePool {
		TNode **pool;
		TNode *head;

		int size;

		void substitute(const NodePool<TNode>& right) {
			pool = right.pool;
			size = right.size;
			head = right.head;
		}
	public:
		NodePool(ListProperty<TNode>& prop, const int size) : size(size) {
			pool = new TNode*[size];

			for (int i = 0; i < size; i++) pool[i] = new TNode(prop);

			clear();

		}

		NodePool(NodePool<TNode>& source) {
			substitute(source);
		}

		~NodePool() {
			for (int i = 0; i < size; i++) delete pool[i];

			delete[] pool;
		}


		TNode* pop() {
			auto toBeUsed = head;
			head = head->nextNode;

			return toBeUsed;
		}

		void add(TNode *node) {
			node->nextNode = head;
			head = node;
		}

		void clear() {
			head = pool[0];
			for (int i = 1; i < size; i++) pool[i - 1]->nextNode = pool[i];
		}

		// narrow copy
		NodePool<TNode>& operator=(const NodePool<TNode>& right) {
			substitute(right);

			return *this;
		}
	};

	// A list that is linked as ring.
	// This class provides "head" node and "tail" node, where
	// the ring consists of[head, ..., tail].
	// Note that the prior of head is tail and the next of tail is head.
	// You can traverse the ring by Iterator
	// and access the item of the node by *Iterator.
	template<typename Value>
	class RingList {
		typedef RingListNode<Value> TNode;
		ListProperty<TNode> property;

		const u_int capacity;

		NodePool<TNode> nodePool;

		template<typename TNode>
		class Iterator {
			TNode *node;
			NodePool<TNode>& pool;
		public:
			Iterator(const Iterator<TNode>& itr) : node(itr.node), pool(itr.pool) {
			}

			Iterator(TNode* node, NodePool<TNode>& pool) : node(node), pool(pool) {

			}

			Iterator& operator++() {
				node = (*node).nextNode;

				return *this;
			}

			Iterator operator++(int) {
				Iterator old = *this;
				++(*this);
				return old;
			}

			Iterator& operator--() {
				node = (*node).prevNode;

				return *this;
			}

			Iterator operator--(int) {
				Iterator old = *this;
				--(*this);
				return old;
			}

			Iterator& operator=(const Iterator& right) {
				node = right.node;
				pool = right.pool;
				return *this;
			}

			// the item pointed by this iterator will be removed from the ring.
			// after removing, this iterator will point the next of current item.
			// returns next iterator or *this.
			Iterator& remove() {
				auto next = (*node).nextNode;
				node->leave();

				pool.add(node);
				node = next;

				return *this;
			}

			Value& operator*() const {
				return **node;
			}

			Value* operator->() const {
				return &(**node);
			}

			bool operator==(const Iterator& right) const {
				return node == right.node;
			}

			Value& nextValue() const {
				return **(node->nextNode);
			}
			Value& prevValue() const {
				return **(node->prevNode);
			}

		};


	public:
		RingList(const u_int capacity) : capacity(capacity), nodePool(property, capacity) {
		}

		void add(const Value& value) {
			TNode *node = nodePool.pop();
			**node = value;

			if (count() == 0) {
				(*node).nextNode = node;
				(*node).prevNode = node;
				property.setHead(node);
				property.setTail(node);
				property.countUp();
				return;
			}

			property.getTail()->insertNext(node);
		}

		u_int count() const {
			return property.count();
		}


		typedef Iterator<TNode> NodeIterator;

		NodeIterator head() {
			NodeIterator itr(property.getHead(), nodePool);
			return itr;
		}

		NodeIterator tail() {
			NodeIterator itr(property.getTail(), nodePool);
			return itr;
		}

		void clear() {
			property.clear();
			nodePool.clear();
		}


	};

	template<typename Value>
	class RingArrayList {
		u_int size_, count_;
		int headIndex = -1;

		class Element {
			int prevIndex;
			int nextIndex;
			Value value;
			bool isEmpty;

			void substitute(const Element& source) {
				prevIndex = source.prevIndex;
				nextIndex = source.nextIndex;
				value = source.value;
				isEmpty = source.isEmpty;
			}
		public:

			Element() {
				kill();
			}

			Element(const Element& source) {
				substitute(source);
			}

			void set(const Value& v) {
				value = v;
				isEmpty = false;
			}

			Value& get() {
				return value;
			}

			bool hasValue() {
				return !isEmpty;
			}

			void setNextIndex(const int& next) {
				nextIndex = next;
			}
			void setPrevIndex(const int& prev) {
				prevIndex = prev;
			}

			const int& peekNextIndex() const {
				return nextIndex;
			}

			const int& peekPrevIndex() const {
				return prevIndex;
			}

			void kill() {
				nextIndex = -1;
				prevIndex = -1;
				isEmpty = true;
			}

			Element operator=(const Element& right) {
				substitute(right);
				return *this;
			}
		};

		Element *elements = nullptr;

		void connect(int prevIndex, int nextIndex) {
			elements[prevIndex].setNextIndex(nextIndex);
			elements[nextIndex].setPrevIndex(prevIndex);
		}

		void kill(int index) {
			elements[index].kill();
		}
		Element& nextElement(const int& index) {
			return elements[nextIndexOf(index)];
		}
		Element& prevElement(const int& index) {
			return elements[elements[index].peekPrevIndex()];
		}


		void substitute(const RingArrayList<Value>& source) {
			allocate(source.size());

			for (u_int i = 0; i < source.size(); i++) {
				elements[i] = source.elements[i];
			}

			//std::copy(source.elements, source.elements + source.size(), elements);
			count_ = source.count();
			headIndex = source.peekHeadIndex();

		}

		// existing values will be deleted.
		void allocate(const u_int& newSize) {
			if (elements != nullptr){
				delete[] elements;
			}
			elements = new Element[newSize];

			size_ = newSize;
		}


	public:
		RingArrayList() : size_(0), count_(0) {
			elements = nullptr;
		}

		RingArrayList(int size) : size_(size), count_(0) {
			elements = new Element[size];
		}

		RingArrayList(const RingArrayList<Value>& source) {
			substitute(source);
		}


		~RingArrayList() {
			delete[] elements;
		}

		void set(const int& index, const Value& v) {
			elements[index].set(v);
		}

		Value& get(const int& index) {
			if (!exists(index)) {
				std::ostringstream ss("RingArrayList: ");
				ss << "The " << index << "-th element is out of ring!: #elements=" << count();
				throw std::invalid_argument(ss.str());
			}

			return elements[index].get();
		}
		const Value& get(const int& index) const {
			if (!exists(index)) {
				std::ostringstream ss("RingArrayList: ");
				ss << "The " << index << "-th element is out of ring!: #elements=" << count();
				throw std::invalid_argument(ss.str());
			}

			return elements[index].get();
		}

		void makeLinks() {
			int lastIndex = -1;

			count_ = 0;
			for (u_int i = 0; i < size_; i++) {
				if (elements[i].hasValue()) {
					if (lastIndex == -1) {
						headIndex = i;
					}
					else {
						connect(lastIndex, i);
					}
					lastIndex = i;

					count_++;
				}
			}
			connect(lastIndex, headIndex);
		}

		int nextIndexOf(const int& index) const {
			return elements[index].peekNextIndex();
		}
		int prevIndexOf(const int& index) const {
			return elements[index].peekPrevIndex();
		}

		Value& nextOf(const int& index) {
			return get(nextIndexOf(index));
		}
		const Value& nextOf(const int& index) const {
			return get(nextIndexOf(index));
		}

		Value& prevOf(const int& index) {
			return get(prevIndexOf(index));
		}
		const Value& prevOf(const int& index) const {
			return get(prevIndexOf(index));
		}

		Value& head() {
			return get(headIndex);
		}

		Value& tail() {
			return prevOf(headIndex);
		}

		bool exists(u_int index) const {
			return (0 <= index && index < size()) && elements[index].hasValue();
		}

		void insert(const int& index, const Value& value) {
			if (this->exists(index))
				throw std::invalid_argument("already exists");

			set(index, value);
			count_++;
			for (u_int i = 0; i < size_; i++) {
				int prevIndex = (index - i - 1) % size_;
				if (elements[prevIndex].hasValue()) {
					connect(prevIndex, index);
					break;
				}
			}
			for (u_int i = 0; i < size_; i++) {
				int nextIndex = (index + i + 1) % size_;
				if (elements[nextIndex].hasValue()) {
					connect(index, nextIndex);
					break;
				}
			}

		}

		void remove(const int& index) {
			auto& target = elements[index];

			if (count_ == 0) {
				throw std::out_of_range("no item");
			}

			if (count_ == 1) {
				headIndex = -1;
				kill(index);
			}
			else {
				if (headIndex == index) {
					headIndex = nextIndexOf(index);
				}
				connect(prevIndexOf(index), nextIndexOf(index));
				kill(index);
			}
			count_--;
		}

		int peekHeadIndex() const {
			return headIndex;
		}

		int peekTailIndex() const {
			return prevIndexOf(headIndex);
		}

		const u_int& count() const {
			return count_;
		}

		const u_int& size() const {
			return size_;
		}

		bool empty() {
			return count() <= 0;
		}

		void clear() {
			for (u_int i = 0; i < size_; i++) {
				elements[i].kill();
			}
			count_ = 0;
			headIndex = -1;
		}

		//int gapToNext(const int& index) const {
		//	return (size_ + nextIndexOf(index) - index) % size_;
		//}

		Value& operator[](const int& index) {
			return get(index);
		}
		const Value& operator[](const int& index) const {
			return get(index);
		}

		RingArrayList<Value>& operator=(const RingArrayList<Value>& right) {
			substitute(right);
			return *this;
		}

		std::string toString() {
			using namespace std;
			stringstream ss;

			auto index = headIndex;
			
			for (u_int i = 0; i < count(); i++) {
				ss << "[" << index << "] ";
				ss << get(index).toString() << ", ";
				index = nextIndexOf(index);
			}
			return ss.str();
		}
	};

}
