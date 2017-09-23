#pragma once

#include <cstddef>

#include "pointer_control_block.hpp"
#include "weak_ptr.hpp"

#if 0
namespace std {




template<class T>
class shared_ptr final
{
public:
	using element_type = T;
	using weak_type = std::weak_ptr<T>;

	constexpr shared_ptr() noexcept = default;
	constexpr shared_ptr(std::nullptr_t) noexcept;
	shared_ptr(const shared_ptr& other) noexcept;
	shared_ptr(shared_ptr&& other);

	~shared_ptr();

	shared_ptr& operator =(const shared_ptr&);
	shared_ptr& operator =(shared_ptr&&);

	element_type& operator *() const noexcept;
	element_type* operator ->() const noexcept;


private:
	_details::pointer_control_block* _pcb{ nullptr };
};


// STD violation: allocated_shared takes reference to Alloc instead of const reference
template<class T, class Alloc, class... Args>
shared_ptr<T> allocate_shared(Alloc& a, Args&&... args);


} // namespace std
#endif
