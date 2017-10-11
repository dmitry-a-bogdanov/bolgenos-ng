#pragma once

#include <memory>
#include <utility>


namespace std
{


namespace _details
{


template<class R, class... Args>
struct callable
{
    virtual R call(Args...) const = 0;
    virtual std::unique_ptr<callable> clone() const = 0;
    virtual ~callable() {}
};

template<class Functor, class R, class... Args>
struct function_impl: callable<R, Args...>
{
    function_impl(const function_impl& other)
        : f{ other.f }
    {
    }

    function_impl(Functor function)
        : f{ std::move(function) }
    {
    }

    virtual R call(Args... args) const override
    {
        return f(args...);
    }


    virtual std::unique_ptr<callable<R, Args...>> clone() const override
    {
        return std::make_unique<function_impl<Functor, R, Args...>>(f);
    }

    Functor f;

    virtual ~function_impl() {}
};


}
// namespace _details


template<class>
class function;

template<class R, class... Args>
class function<R(Args...)>
{
public:
    using result_type = R;


    function() noexcept = default;


    function(std::nullptr_t) noexcept
    {
    }


    function(const function& other)
    {
        _holder = other._holder->clone();
    }


    function(function&& other)
    {
        std::swap(_holder, other._holder);
    }


    template<class U>
    function(U f)
    {
        using impl_type = _details::function_impl<U, R, Args...>;
        _holder = std::make_unique<impl_type>(std::forward<U>(f));
    }


    function& operator =(const function& other)
    {
        _holder = other._holder->clone();
        return *this;
    }


    template<class F>
    function& operator=(F f)
    {
        using impl_type = _details::function_impl<F, R, Args...>;
        _holder = std::make_unique<impl_type>(std::forward(f));
        return *this;
    }


    result_type operator()(Args ...args)
    {
        return _holder->call(std::forward<Args>(args)...);
    }


private:
    using base_holder_type = _details::callable<R, Args...>;
    std::unique_ptr<base_holder_type> _holder{};
};


}
// namespace std
