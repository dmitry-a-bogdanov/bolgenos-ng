#pragma once

#include <bolgenos-ng/error.h>

#include <lib/ostream.hpp>

// Special rules for OST:
//
// * Avoid dynamic allocation in all possible cases


namespace ost
{


class Conf
{
public:

    static void bad_stream(lib::ostream& out)
    {
        instance()._bad_stream = &out;
    }


    static lib::ostream& bad_stream()
    {
        return *instance()._bad_stream;
    }


    static void good_stream(lib::ostream& out)
    {
        instance()._good_stream = &out;
    }


    static lib::ostream& good_stream()
    {
        return *instance()._good_stream;
    }


protected:
    static Conf& instance()
    {
        static Conf instance;
        return instance;
    }


private:
    Conf() = default;

    lib::ostream* _good_stream { &lib::cinfo };
    lib::ostream* _bad_stream { &lib::cout };
};

class Assert
{
public:
    Assert() = delete;
    Assert(const Assert&) = delete;
    Assert& operator=(Assert&) = delete;

    explicit Assert(lib::ostream& good_ostream, lib::ostream& bad_ostream, bool cond,
            const char* expr, const char* file, int line)
            : _good_ostream { good_ostream }, _bad_ostream { bad_ostream }, _cond { cond }, _expr {
                    expr }, _file { file }, _line { line }
    {
    }


    template<class T>
    lib::ostream& operator<<(const T& arg)
    {
        _already_printed = true;
        print_preamble();
        return cond_stream() << ": " << arg;
    }


    virtual ~Assert()
    {
        if (!_already_printed)
            print_preamble();

        cond_stream() << lib::endl;

        if (!_cond)
            panic("Test failed");
    }
protected:

    void print_preamble()
    {
        cond_stream() << _file << ":" << _line << ": " << "assertion(" << _expr << ") "
                << (_cond ? "passed" : "failed");
    }


    lib::ostream& cond_stream()
    {
        if (_cond)
            return _good_ostream;
        else
            return _bad_ostream;
    }


    bool get_cond() const
    {
        return _cond;
    }

private:
    lib::ostream& _good_ostream;
    lib::ostream& _bad_ostream;
    bool _cond;
    const char* _expr;
    const char* _file;
    int _line;

    bool _already_printed { false };
};


#define OST_ASSERT(expr) \
	::ost::Assert(::ost::Conf::good_stream(), \
			::ost::Conf::bad_stream(), \
			(expr), \
			stringify(expr), \
			__PRETTY_FUNCTION__, \
			__LINE__)

#ifdef OSTv3

class Module;

struct inplace_list_elem_t
{
    void* next = nullptr;
};


class GlobalTestScope
{
public:
    GlobalTestScope(const GlobalTestScope&) = delete;
    GlobalTestScope& operator=(GlobalTestScope&) = delete;

    GlobalTestScope& instance()
    {
        static GlobalTestScope singleton;
        return singleton;
    }


    void register_module(Module* module)
    {
        module->next = _first;
        _first = module;
    }
private:
    GlobalTestScope() = default;

    Module *_first{ nullptr };
};


class TestFunction
{
public:
    virtual void run() = 0;
    virtual ~TestFunction();
};


class TestFunctionContainer: public TestFunction
{
public:
};



class Module: private inplace_list_elem_t
{
public:
    Module(const char *name);


    void run()
    {
        Conf::good_stream() << "Executing test module: " << _name;
        do_test();
        Conf::good_stream() << "Done.";
    }


protected:
    void do_test();


private:
    const char* _name;
    friend class GlobalTestScope;
};


#endif // OSTv3


}
// namespace ost
