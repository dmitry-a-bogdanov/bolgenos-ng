#pragma once

namespace testing {





template<class T>
class forward_list
{
public:
	using value_type = T;
	using reference = value_type&;
	using const_reference = const value_type&;
	using error_type = bool;

	forward_list() = default;
	forward_list(const forward_list&) = delete;
	forward_list& operator=(const forward_list&) = delete;
	~forward_list();

	inline reference front()
	{
		return first_->value_;
	}

	inline const_reference front() const
	{
		return first_->value_;
	}

	void pop_front();
	error_type push_front(const value_type&);

	inline bool empty() const
	{
		return first_ == nullptr;
	}

private:
	struct list_item_type;
	list_item_type* first_ = nullptr;
}; // class forward_list


template<class T>
struct forward_list<T>::list_item_type
{
	T value_;
	list_item_type* next_;
};


template<class T>
forward_list<T>::~forward_list()
{
	for (auto it = first_; it != nullptr;) {
		auto next = it->next_;
		delete it;
		it = next;
	}
}


template<class T>
void forward_list<T>::pop_front()
{
	auto removed_elem = first_;
	first_ = first_->next_;
	delete first_;
}


template<class T>
bool forward_list<T>::push_front(const value_type& value)
{
	list_item_type *elem = new list_item_type();
	if (!elem) {
		return false;
	}
	elem->next_ = first_;
	elem->value_ = value;
	first_ = elem;
	return true;
}


} // namespace testing
