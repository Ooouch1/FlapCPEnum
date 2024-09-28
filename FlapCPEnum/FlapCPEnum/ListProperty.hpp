#pragma once

#include "abbreviation.h"
#include <memory>

namespace mylib {
	// An object to keep information of RingList
	// for sharing between RingList and RingListNode.
	// The instance of this class should never be public to other module.
	template<typename TNode>
	class ListProperty {
	//public:
	//	typedef std::shared_ptr<TNode> SharedNodePointer;

	private:

		TNode *head;
		TNode *tail;
		u_int count_ = 0;

	public:
		void countUp() {
			count_++;
		}

		void countDown() {
			count_--;
		}

		const u_int& count() const {
			return count_;
		}

		TNode *getHead() const {
			return head;
		}

		void setHead(TNode *node) {
			head = node;
		}

		TNode *getTail() const {
			return tail;
		}

		void setTail(TNode *node) {
			tail = node;
		}

		bool isHead(const TNode *node) const {
			return node == head;
		}

		bool isTail(const TNode *node) const {
			return node == tail;
		}

		void clear() {
			head = NULL;
			tail = NULL;
			count_ = 0;
		}
	};
}