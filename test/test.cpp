#define CATCH_CONFIG_MAIN

#include <umbra/shadow.hpp>

#include <catch2/catch.hpp>

#include <type_traits>

template<class T>
constexpr bool is_read_only = std::is_const_v<std::remove_reference_t<T>>;

#define IS_READ_ONLY(x) is_read_only<decltype(x)>

TEST_CASE("Shadow allows shadowing without warnings") {
  [[maybe_unused]] constexpr  int x = 0;
  UMBRA_SHADOW(constexpr char x = 'a') { STATIC_REQUIRE(x == 'a'); }
}

TEST_CASE("Frozen variables are const ref") {
  int  x = 0;
  char y = 0;

  STATIC_REQUIRE(!IS_READ_ONLY(x));
  STATIC_REQUIRE(!IS_READ_ONLY(y));

  UMBRA_FREEZE(x, y) {
    STATIC_REQUIRE(IS_READ_ONLY(x));
    STATIC_REQUIRE(IS_READ_ONLY(y));
  }
}
