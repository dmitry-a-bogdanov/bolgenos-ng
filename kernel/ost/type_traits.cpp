#include <type_traits>

namespace {

class ClassType {};

} // namespace


using namespace std;


namespace t_add_const
{

static_assert(is_same<typename add_const<int>::type, add_const_t<int>>(), "");
static_assert(is_same<typename add_const<const long>::type, add_const_t<const long>>(), "");

static_assert(is_same<add_const<int>::type, const int>::value, "");
static_assert(is_same<add_const<volatile int>::type, const volatile int>::value, "");
static_assert(is_same<add_const<int *>::type, int* const>::value, "");
static_assert(is_same<add_const<int &>::type, int&>::value, "");
static_assert(is_same<add_const<int (int)>::type, int (int)>::value, "");
static_assert(is_same<add_const<const int>::type, const int>::value, "");
static_assert(is_same<add_const<ClassType>::type, const ClassType>::value, "");
static_assert(is_same<add_const<volatile ClassType>::type, const volatile ClassType>::value, "");
static_assert(is_same<add_const<ClassType*>::type,ClassType* const>::value, "");
static_assert(is_same<add_const<ClassType&>::type, ClassType&>::value, "");
static_assert(is_same<add_const<ClassType (ClassType)>::type, ClassType (ClassType)>::value, "");
static_assert(is_same<add_const<const ClassType>::type, const ClassType>::value, "");

} // namespace t_add_const



namespace t_is_same
{

static_assert(is_same<int, int>::value == true, "");
static_assert(is_same<const int, const int>::value == true, "");
static_assert(is_same<int&, int&>::value == true, "");
static_assert(is_same<ClassType, ClassType>::value == true, "");

static_assert(is_same<void, int>::value == false, "");
static_assert(is_same<int, const int>::value == false, "");
static_assert(is_same<int, int&>::value == false, "");
static_assert(is_same<int, ClassType>::value == false, "");

} // t_is_same



namespace t_integral_constant
{

template<class T, T val>
using ic = std::integral_constant<T, val>;

static_assert(ic<int, 1>{}() == 1, "");
static_assert(ic<int, 0>{}() == 0, "");
static_assert(ic<int, -1>{}() == -1, "");

static_assert(ic<bool, true>{}() == true, "");
static_assert(ic<bool, false>{}() == false, "");

static_assert(ic<int, 1>() == 1, "");
static_assert(ic<int, 0>() == 0, "");
static_assert(ic<int, -1>() == -1, "");

static_assert(ic<bool, true>() == true, "");
static_assert(ic<bool, false>() == false, "");

void test01()
{
	const bool *p = &std::integral_constant<bool, true>::value;
	(void)p;
}

typedef std::true_type                      true_type;
typedef std::false_type                     false_type;

typedef true_type::value_type               true_value_type;
typedef true_type::type                     true_type;
typedef true_type::type::value_type         true_type_value_type;
typedef true_type::type::type               true_type_type;

typedef false_type::value_type              false_value_type;
typedef false_type::type                    false_type;
typedef false_type::type::value_type        false_type_value_type;
typedef false_type::type::type              false_type_type;


static_assert(true_type::value == true, "" );
static_assert(false_type::value == false, "" );
static_assert(true_type::type::value == true, "" );
static_assert(false_type::type::value == false, "" );

} // namespace t_integral_constant



namespace t_remove_reference {

static_assert(is_same<remove_reference<int&>::type, int>::value, "");
static_assert(is_same<remove_reference<int>::type, int>::value, "");
static_assert(is_same<remove_reference<const int&>::type, const int>::value, "");
static_assert(is_same<remove_reference<int*&>::type, int*>::value, "");
static_assert(is_same<remove_reference<ClassType&>::type, ClassType>::value, "");
static_assert(is_same<remove_reference<ClassType>::type, ClassType>::value, "");
static_assert(is_same<remove_reference<int(&)(int)>::type, int(int)>::value, "");
static_assert(is_same<remove_reference<int&&>::type, int>::value, "");
static_assert(is_same<remove_reference<int>::type, int>::value, "");
static_assert(is_same<remove_reference<const int&&>::type, const int>::value, "");
static_assert(is_same<remove_reference<int*&&>::type, int*>::value, "");
static_assert(is_same<remove_reference<ClassType&&>::type, ClassType>::value, "");
static_assert(is_same<remove_reference<ClassType>::type, ClassType>::value, "");
static_assert(is_same<remove_reference<int(&&)(int)>::type, int(int)>::value, "");

} // namespace t_remove_reference


