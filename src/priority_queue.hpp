#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {

/**
 * @brief a container like std::priority_queue which is a heap internal.
 * **Exception Safety**: The `Compare` operation might throw exceptions for certain data.
 * In such cases, any ongoing operation should be terminated, and the priority queue should be restored to its original state before the operation began.
 */
template<typename T, class Compare = std::less<T>>
class priority_queue {
private:
	struct node {
		T val;
		node *left;
		node *right;
		int npl;
		node(const T &v, node *l = nullptr, node *r = nullptr)
		    : val(v), left(l), right(r), npl(0) {}
	};

	node *root;
	size_t num;
	Compare cmp;

	static int get_npl(node *t) {
		return t ? t->npl : -1;
	}

	static node *clone(node *t) {
		if (!t) {
			return nullptr;
		}
		return new node(t->val, clone(t->left), clone(t->right));
	}

	static void destroy(node *t) {
		if (t) {
			destroy(t->left);
			destroy(t->right);
			delete t;
		}
	}

	node *merge(node *a, node *b) {
		if (!a) return b;
		if (!b) return a;
		if (cmp(a->val, b->val)) {
			// b has higher priority, so b becomes the new root.
			node *new_right = merge(b->right, a);
			b->right = new_right;
			if (get_npl(b->left) < get_npl(b->right)) {
				node *tmp = b->left;
				b->left = b->right;
				b->right = tmp;
			}
			b->npl = get_npl(b->right) + 1;
			return b;
		} else {
			// a has higher or equal priority, so a becomes the new root.
			node *new_right = merge(a->right, b);
			a->right = new_right;
			if (get_npl(a->left) < get_npl(a->right)) {
				node *tmp = a->left;
				a->left = a->right;
				a->right = tmp;
			}
			a->npl = get_npl(a->right) + 1;
			return a;
		}
	}

public:
	/**
	 * @brief default constructor
	 */
	priority_queue() : root(nullptr), num(0) {}

	/**
	 * @brief copy constructor
	 * @param other the priority_queue to be copied
	 */
	priority_queue(const priority_queue &other)
	    : root(clone(other.root)), num(other.num), cmp(other.cmp) {}

	/**
	 * @brief deconstructor
	 */
	~priority_queue() {
		destroy(root);
	}

	/**
	 * @brief Assignment operator
	 * @param other the priority_queue to be assigned from
	 * @return a reference to this priority_queue after assignment
	 */
	priority_queue &operator=(const priority_queue &other) {
		if (this != &other) {
			node *new_root = clone(other.root);
			destroy(root);
			root = new_root;
			num = other.num;
			cmp = other.cmp;
		}
		return *this;
	}

	/**
	 * @brief get the top element of the priority queue.
	 * @return a reference of the top element.
	 * @throws container_is_empty if empty() returns true
	 */
	const T & top() const {
		if (!root) {
			throw container_is_empty();
		}
		return root->val;
	}

	/**
	 * @brief push new element to the priority queue.
	 * @param e the element to be pushed
	 */
	void push(const T &e) {
		node *new_node = new node(e);
		try {
			root = merge(root, new_node);
			num++;
		} catch (...) {
			delete new_node;
			throw runtime_error();
		}
	}

	/**
	 * @brief delete the top element from the priority queue.
	 * @throws container_is_empty if empty() returns true
	 */
	void pop() {
		if (!root) {
			throw container_is_empty();
		}
		node *old_root = root;
		try {
			root = merge(root->left, root->right);
			num--;
			delete old_root;
		} catch (...) {
			root = old_root;
			throw runtime_error();
		}
	}

	/**
	 * @brief return the number of elements in the priority queue.
	 * @return the number of elements.
	 */
	size_t size() const {
		return num;
	}

	/**
	 * @brief check if the container is empty.
	 * @return true if it is empty, false otherwise.
	 */
	bool empty() const {
		return num == 0;
	}

	/**
	 * @brief merge another priority_queue into this one.
	 * The other priority_queue will be cleared after merging.
	 * The complexity is at most O(logn).
	 * @param other the priority_queue to be merged.
	 */
	void merge(priority_queue &other) {
		if (this == &other) {
			return;
		}
		try {
			root = merge(root, other.root);
			num += other.num;
			other.root = nullptr;
			other.num = 0;
		} catch (...) {
			throw runtime_error();
		}
	}
};

}

#endif
