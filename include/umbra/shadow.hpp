#ifndef INCLUDE_GUARD_umbra_shadow_hpp
#define INCLUDE_GUARD_umbra_shadow_hpp

#include "hedley.h"
// https://www.fluentcpp.com/2019/08/30/how-to-disable-a-warning-in-cpp/

#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <initializer_list>

#define UMBRA_CALL_(_, mac, elem) mac(elem)

#define UMBRA_FOR_VARARGS_(mac, ...)                                      \
  BOOST_PP_SEQ_FOR_EACH(UMBRA_CALL_,                                      \
                        mac,                                              \
                        BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#if defined(__GNUC__)
#  define UMBRA_PRAGMA_IGNORE_SHADOW_                                     \
    _Pragma("GCC diagnostic ignored \"-Wshadow\"")
#elif defined(_MSC_VER)
#  define UMBRA_MSVC_DISABLE_WARNING_(num) __pragma(warning(disable : num))
#  define UMBRA_PRAGMA_IGNORE_SHADOW_                                     \
    /* declaration of 'identifier' hides previous local declaration */    \
    UMBRA_MSVC_DISABLE_WARNING_(4456)                                     \
    /* declaration of 'identifier' hides function parameter*/             \
    UMBRA_MSVC_DISABLE_WARNING_(4457)                                     \
    /* declaration of 'identifier' hides class member */                  \
    UMBRA_MSVC_DISABLE_WARNING_(4458)                                     \
    /* declaration of 'identifier' hides global declaration */            \
    UMBRA_MSVC_DISABLE_WARNING_(4459)
#else
#  define UMBRA_PRAGMA_IGNORE_SHADOW_
#endif

#define UMBRA_IGNORE_SHADOW(...)                                          \
  HEDLEY_DIAGNOSTIC_PUSH UMBRA_PRAGMA_IGNORE_SHADOW_ __VA_ARGS__          \
      HEDLEY_DIAGNOSTIC_POP

#define UMBRA_GENSYM_(sym) HEDLEY_CONCAT3(umbra_gensym, __COUNTER__, sym)

/**
 * Defines a new variable in the introduced scope
 *
 * UMBRA_LET1(int x = 3){
 *   ++x;
 * }
 */
#define UMBRA_LET1(...)                                                   \
  for(__VA_ARGS__; [[maybe_unused]] auto UMBRA_GENSYM(_) : {0})

/**
 * Disable warnings for shadowing for one variable definition introduced in
 * a new scope
 *
 * int const x = 2;
 * UMBRA_SHADOW(int x = 0){
 *   ++x; // ok!
 * }
 */
#define UMBRA_SHADOW(...) UMBRA_IGNORE_SHADOW(UMBRA_LET1(__VA_ARGS__))

#if defined(_MSC_VER)
// I'm not sure why MSVC disagrees where annotations go
#  define UMBRA_POISON_ATTR_ __declspec(deprecated("poisoned"))
#else
#  define UMBRA_POISON_ATTR_ [[deprecated("poisoned"), maybe_unused]]
#endif
#define UMBRA_POISON1 UMBRA_SHADOW(UMBRA_POISON_ATTR_ char name)
/**
 * Poisons a variable within the introduced scope
 *
 * int x = 0;
 * UMBRA_POISON(x){
 *   int y = x; // warning/error!
 * }
 */
#define UMBRA_POISON(...) UMBRA_FOR_VARARGS_(UMBRA_POISON1, __VA_ARGS__)

#define UMBRA_FREEZE1_x_(tmp, name)                                       \
  UMBRA_LET1(auto const& tmp = name) UMBRA_SHADOW(auto const& name = tmp)
#define UMBRA_FREEZE1_(name) UMBRA_FREEZE1_x_(UMBRA_GENSYM(tmp), name)
/**
 * Freezes a variable into a const within the new scope:
 *
 * for(int i =0; i < 10; ++i)
 *   UMBRA_FREEZE(i){
 *     ++i; // error! i is const here
 *   }
 */
#define UMBRA_FREEZE(...) UMBRA_FOR_VARARGS_(UMBRA_FREEZE1_, __VA_ARGS__)

#include <type_traits>
namespace umbra {
/**
 * Our fallback implementation for ReadIn
 */
template<class T>
using ReadIn = std::conditional_t<
    std::is_trivially_copyable_v<T> && sizeof(T) <= 2 * sizeof(void*),
    T const,
    T const&>;
} // namespace umbra
#ifndef UMBRA_READIN_TEMPLATE
/**
 * Customization point! A template that decides if we pass by
 * const & or by value
 */
#  define UMBRA_READIN_TEMPLATE ::umbra::ReadIn
#endif
#define UMBRA_READIN1_x_(tmp, name)                                        \
  UMBRA_LET1(auto const& tmp = name)                                             \
  UMBRA_SHADOW(                                                            \
      UMBRA_READIN_TEMPLATE<std::remove_reference_t<decltype(tmp)>> name = \
          tmp)
#define UMBRA_READIN1_(name) UMBRA_READIN1_x_(UMBRA_GENSYM(tmp), name)
/**
 * Rebind `name` to a ReadIn within the new scope
 *
 * To pass by value or const&? How expensive is a copy? How much do
 * references befuddle the optimizer? Can we automate this decision without
 * interfering with template type deduction?
 *
 * Using UMBRA_READIN, we can take a variable by const& and rebind it to
 * a ReadIn, copying if that's cheap.
 *
 * template<class T>
 * auto f(T const& x) {
 *   UMBRA_READIN(x){
 *     // now x becomes a copy if that's cheap
 *     // without messing with template type deduction
 *   }
 * }
 *
 * The template function is inlinable, so the optimizer can (with luck) see
 * if we end up taking a copy.
 */
#define UMBRA_READIN(...) UMBRA_FOR_VARARGS_(UMBRA_READIN1_, __VA_ARGS__)
#endif
