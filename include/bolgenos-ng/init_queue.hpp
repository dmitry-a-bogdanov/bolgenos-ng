#pragma once

#include <basalt/type_traits.hpp>
#include <basalt/intrusive_list.hpp>

#include <bolgenos-ng/error.h>

namespace bolgenos { namespace init {


class InitCallback
{
public:
	virtual bool invoke() = 0;
	virtual const char *name() = 0;
	virtual ~InitCallback() = default;
};


enum class prio_t
{
	zero = 0,
	cpu_configuration,
	start_serving_irq,
	highest,
	high,
	normal,
	low,
	lowest,
	runtime_tests,
	max
};




class Queue
{
public:
	using subqueue_type = basalt::intrusive_list<InitCallback *>;
	using node_type = basalt::intrusive_list_node<InitCallback *>;

	static Queue& instance();

	void execute() const;

	void register_callback(node_type& node, prio_t prio = prio_t::normal);

private:
	Queue() = default;

	subqueue_type _routines[basalt::to_integral(prio_t::max)];
};


template<typename Functor>
struct FunctorRegisterer: public InitCallback
{
	FunctorRegisterer(prio_t prio, const char* reg_name, Functor f)
		: _f{ f }, _name{ reg_name }
	{
		bolgenos::init::Queue::instance().register_callback(_queue_node, prio);
	}

	FunctorRegisterer(const FunctorRegisterer<Functor>&) = delete;
	FunctorRegisterer<Functor>& operator =(const FunctorRegisterer<Functor>&) = delete;

	const char* name() override { return _name; }
	bool invoke() override { return _f(); }

	Functor _f;
	const char* _name;
	basalt::intrusive_list_node<InitCallback *> _queue_node{static_cast<InitCallback *>(this)};
};


} }
