#pragma once

#include "impl/basic_forward_list.hpp"
#include <lib/type_traits.hpp>



namespace bolgenos_testing {


class free_list:
	public bolgenos_testing::_impl::basic_forward_list<
		bolgenos_testing::_impl::basic_fwd_list_node>
{
	using node_type = bolgenos_testing::_impl::basic_fwd_list_node;
	using base_list = bolgenos_testing::_impl::basic_forward_list<node_type>;
public:
	using min_size = lib::integral_constant<size_t, sizeof(node_type)>;
	free_list() = default;



	class iterator
	{
	public:
		iterator() = default;


		iterator& operator++()
		{
			current_ = current_->next;
			return *this;
		}


		iterator operator++(int)
		{
			iterator tmp(*this);
			current_ = current_->next;
			return tmp;
		}


		const void* operator*() const {
			return current_;
		}


		template<typename Iterator>
		bool operator==(const Iterator& other) const
		{
			return current_ == other.current_;
		}


		template<typename Iterator>
		bool operator!=(const Iterator& other) const
		{
			return current_ != other.current_;
		}


	private:

		iterator(node_type* node) :
			current_(node)
		{
		}

		node_type* current_ = nullptr;


		friend
		class const_iterator;

		friend
		class free_list;
	};




	class const_iterator
	{
	public:
		const_iterator() = default;


		const_iterator(iterator other) :
			current_(other.current_)
		{
		}


		const_iterator& operator++()
		{
			current_ = current_->next;
			return *this;
		}


		const_iterator operator++(int)
		{
			const_iterator tmp(*this);
			current_ = current_->next;
			return tmp;
		}

		const void* operator*()
		{
			return current_;
		}

		template<typename Iterator>
		bool operator==(const Iterator& other) const
		{
			return current_ == other.current_;
		}


		template<typename Iterator>
		bool operator!=(const Iterator& other) const
		{
			return current_ != other.current_;
		}

	private:

		const_iterator(const node_type* node) :
			current_(node)
		{
		}


		const node_type* current_ = nullptr;


		friend
		class iterator;

		friend
		class free_list;
	};




	inline
	iterator before_begin()
	{
		return iterator(base_list::before_begin());
	}


	inline
	const_iterator before_begin() const
	{
		return const_iterator(base_list::before_begin());
	}


	inline
	const_iterator cbefore_begin() const
	{
		return const_iterator(base_list::before_begin());
	}


	inline
	iterator begin()
	{
		return iterator(base_list::begin());
	}


	inline
	const_iterator begin() const
	{
		return const_iterator(base_list::begin());
	}


	inline
	const_iterator cbegin() const
	{
		return const_iterator(base_list::begin());
	}


	inline
	iterator end()
	{
		return iterator(base_list::end());
	}


	inline
	const_iterator end() const
	{
		return const_iterator(base_list::end());
	}


	inline
	const_iterator cend() const
	{
		return const_iterator(base_list::end());
	}


	inline
	void push_front(void *value)
	{
		push_after(cbefore_begin(), value);
	}


	inline
	void push_after(const_iterator position, void *value)
	{
		auto node = static_cast<node_type *>(value);
		auto after = const_cast<node_type *>(position.current_);
		base_list::push_after(after, node);
	}


	inline
	void erase_after(const_iterator position)
	{
		auto after = const_cast<node_type *>(position.current_);
		auto removed = base_list::erase_after(after);
		// FIXME: the following assignment will most probably be
		// ignored by compiler. Secure erase is required.
		removed->next = nullptr;
	}

	inline
	void pop_front()
	{
		erase_after(cbefore_begin());
	}


	inline
	void* front()
	{
		return static_cast<void *>(base_list::begin());
	}


	bool empty() const
	{
		return cbegin() == cend();
	}


private:

	friend
	class iterator;
};


} // namespace bolgenos_testing
