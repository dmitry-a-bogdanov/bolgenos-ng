#pragma once

#include <concepts.hpp>
#include <utility.hpp>

namespace lib {

template<class T>
class CircularIntrusiveList;

template<typename T>
struct IntrusiveListNode {
	explicit constexpr IntrusiveListNode(T* obj) : owner{obj}
	{}

	T* const owner;

	IntrusiveListNode* next{nullptr};
	IntrusiveListNode* prev{nullptr};

	friend class CircularIntrusiveList<T>;
};

template<class T>
class CircularIntrusiveList {
public:
	using node_getter_type = IntrusiveListNode<T>* (T::*)();
	explicit CircularIntrusiveList(node_getter_type node_getter):
		_get_node{node_getter}
	{}

	class iterator;
	class end_iterator;

	iterator begin() {
		return iterator{_some};
	}

	end_iterator end() {
		return end_iterator{_some};
	}

	[[nodiscard]] constexpr bool empty() const { return _some == nullptr; }

	T* some() { return _some->owner; }

	void insert(T* value);

	T* remove(T* value);
private:
	IntrusiveListNode<T>* get_node(T* value) {
		return (value->*_get_node)();
	}

	IntrusiveListNode<T>* _some{nullptr};
	node_getter_type _get_node;
};

template<class T>
class CircularIntrusiveList<T>::iterator {
public:

	bool operator==(const end_iterator& other) const {
		return (_node == other._node) && has_shifted() == other.has_shifted();
	}

	bool operator!=(const end_iterator& other) const {
		return !(*this == other);
	}

	T* operator*() {
		return _node->owner;
	}

	iterator& operator++() {
		_node = _node->next;
		++_shift;
		return *this;
	}

private:
	explicit iterator(IntrusiveListNode<T>* node):
		_node{node}
	{}

	bool has_shifted() const { return _shift != 0; }

	IntrusiveListNode<T>* _node;
	uint32_t _shift{0};

	friend class CircularIntrusiveList<T>;
};

template<class T>
class CircularIntrusiveList<T>::end_iterator {
public:

	bool operator==(const iterator& other) const {
		return (_node == other._node) && has_shifted() == other.has_shifted();
	}

private:
	explicit end_iterator(IntrusiveListNode<T>* node):
		_node{node}
	{}

	bool has_shifted() const { return true; }

	IntrusiveListNode<T>* _node;

	friend class CircularIntrusiveList<T>;
};


template<class T>
void CircularIntrusiveList<T>::insert(T* value)
{
	auto value_node = get_node(value);
	if (_some == nullptr) {
		value_node->prev = value_node;
		value_node->next = value_node;
		_some = value_node;
	} else {
		// build: [_some] [value] [_some->next]
		value_node->next = _some->next;
		value_node->prev = _some;
		_some->next->prev = value_node;
		_some->next = value_node;
	}
}

template<class T>
T* CircularIntrusiveList<T>::remove(T* value)
{
	auto* value_node = get_node(value);
	if (value_node == value_node->next) {
		value_node->next = nullptr;
		value_node->prev = nullptr;
		_some = nullptr;
		return nullptr;
	} else {
		auto ret_node = value_node->next;
		value_node->prev->next = value_node->next;
		value_node->next->prev = value_node->prev;
		value_node->next = nullptr;
		value_node->prev = nullptr;
		if (_some == value_node) {
			_some = ret_node;
		}
		return ret_node->owner;
	}
}

}
