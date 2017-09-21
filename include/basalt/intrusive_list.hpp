#pragma once


namespace basalt
{


template<class T>
class intrusive_list;


template<class T>
class intrusive_list_node
{
public:
	intrusive_list_node(const T& val)
		: data{ val }
	{
	}
	intrusive_list_node(const intrusive_list_node<T>&) = delete;
	intrusive_list_node& operator =(const intrusive_list_node<T>&) = delete;

	T data;
	intrusive_list_node<T>* _next{ nullptr };

	friend intrusive_list<T>;
};


template<typename T>
intrusive_list_node<T> make_intrusive_list_node(T&&);


template<typename T>
intrusive_list_node<T> make_intrusive_list_node(const T&);


template<typename T>
class intrusive_list
{
public:
	using node_type = intrusive_list_node<T>;

	class iterator
	{
	public:
		using list_type = intrusive_list<T>;
		using node_type = intrusive_list_node<T>;

		iterator() = default;

		bool operator ==(const iterator& rhs)
		{
			return _node == rhs._node;
		}

		bool operator !=(const iterator& rhs)
		{
			return _node != rhs._node;
		}

		iterator& operator ++()
		{
			_node = _node->_next;
			return *this;
		}

		T& operator *()
		{
			return _node->data;
		}
	private:
		iterator(node_type *node)
		{
			_node = node;
		}

		node_type* _node{ nullptr };
		friend list_type;
	};


	class const_iterator
	{
	public:
		using list_type = const intrusive_list<T>;
		using node_type = const intrusive_list_node<T>;

		const_iterator() = default;

		bool operator ==(const const_iterator& rhs)
		{
			return _node == rhs._node;
		}

		bool operator !=(const const_iterator& rhs)
		{
			return _node != rhs._node;
		}

		const_iterator& operator ++()
		{
			_node = _node->_next;
			return *this;
		}

		const T& operator *()
		{
			return _node->data;
		}
	private:
		const_iterator(node_type *node)
		{
			_node = node;
		}

		node_type* _node{ nullptr };
		friend list_type;
	};


	iterator begin()
	{
		return iterator{ _first };
	}


	const_iterator begin() const
	{
		return const_iterator{ _first };
	}


	iterator end()
	{
		return iterator{ nullptr };
	}


	const_iterator end() const
	{
		return const_iterator{ nullptr };
	}

	bool empty() const
	{
		return _first == nullptr;
	}


	void push_front(node_type& node)
	{
		node._next = _first;
		_first = &node;
	}

private:
	node_type *_first{ nullptr };
};


}
// namespace basalt
