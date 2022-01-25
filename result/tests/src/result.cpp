#include "result/result.hpp"
#include <catch2/catch_test_macros.hpp>
#include <compare>
#include <string>
#include <utility>

using result_type1 = result::result<std::string, double>;
using result_type2 = result::result<double, std::string>;
using ok_type1 = result::ok<std::string>;
using ok_type2 = result::ok<double>;
using err_type1 = result::err<std::string>;
using err_type2 = result::err<double>;

TEST_CASE("empty_tag_t comparison", "[empty_tag_t]") {
  result::empty_tag_t a;
  result::empty_tag_t b;
  REQUIRE((a == b) == true);
  REQUIRE((a != b) == false);
}

TEST_CASE("empty tags spaceship operator", "[err_tag_t|empty_tag_t|ok_tag_t]") {
  SECTION("ok_tag_t") {
    bool b;
    auto ord0 = result::ok_tag <=> result::ok_tag;
    auto ord1 = result::ok_tag <=> result::empty_tag;
    auto ord2 = result::ok_tag <=> result::err_tag;
    REQUIRE(ord0 == std::strong_ordering::equal);
    REQUIRE(ord1 == std::strong_ordering::greater);
    REQUIRE(ord2 == std::strong_ordering::greater);
    b = ord0 == 0;
    REQUIRE(b);
    b = ord1 > 0;
    REQUIRE(b);
    b = ord2 > 0;
    REQUIRE(b);
  }
  SECTION("empty_tag_t") {
    bool b;
    auto ord0 = result::empty_tag <=> result::empty_tag;
    auto ord1 = result::empty_tag <=> result::ok_tag;
    auto ord2 = result::empty_tag <=> result::err_tag;
    REQUIRE(ord0 == std::strong_ordering::equal);
    REQUIRE(ord1 == std::strong_ordering::less);
    REQUIRE(ord2 == std::strong_ordering::greater);
    b = ord0 == 0;
    REQUIRE(b);
    b = ord1 < 0;
    REQUIRE(b);
    b = ord2 > 0;
    REQUIRE(b);
  }
  SECTION("err_tag_t") {
    bool b;
    auto ord0 = result::err_tag <=> result::err_tag;
    auto ord1 = result::err_tag <=> result::ok_tag;
    auto ord2 = result::err_tag <=> result::empty_tag;
    REQUIRE(ord0 == std::strong_ordering::equal);
    REQUIRE(ord1 == std::strong_ordering::less);
    REQUIRE(ord2 == std::strong_ordering::less);
    b = ord0 == 0;
    REQUIRE(b);
    b = ord1 < 0;
    REQUIRE(b);
    b = ord2 < 0;
    REQUIRE(b);
  }
}

TEST_CASE("ok constructor / value", "[ok]") {
  std::string s1("abc");
  std::string s2("def");

  ok_type1 r1(s1);
  ok_type1 r2(std::move(s2));

  REQUIRE(r1.value() == "abc");
  REQUIRE(r2.value() == "def");

  REQUIRE(std::move(r1).value() == "abc");
  REQUIRE(std::move(r2).value() == "def");
}

TEST_CASE("err constructor / value", "[ok]") {
  std::string s1("abc");
  std::string s2("def");

  result::err<std::string> e1(s1);
  result::err<std::string> e2(std::move(s2));

  REQUIRE(e1.value() == "abc");
  REQUIRE(e2.value() == "def");

  REQUIRE(std::move(e1).value() == "abc");
  REQUIRE(std::move(e2).value() == "def");
}

TEST_CASE("result<T, E>(result::ok)", "[result<T, E>]") {
  ok_type1 s1("abc");
  result_type1 r1(s1);
  REQUIRE(r1.is_ok());
  REQUIRE(r1.unwrap() == std::string("abc"));
}

TEST_CASE("result<T, E>(result::err)", "[result<T, E>]") {
  result::err<std::string> s1("abc");
  result_type2 r1(s1);
  REQUIRE(r1.is_err());
  REQUIRE(r1.unwrap_err() == std::string("abc"));
}

TEST_CASE("result<T, E>(const result<T,E>&)", "[result<T, E>]") {
  result_type1 r0(ok_type1("abc"));
  result_type1 r1(r0);
  REQUIRE(r1.is_ok());
  REQUIRE(r1.unwrap() == std::string("abc"));
  result_type2 r2(err_type1("abc"));
  result_type2 r3(r2);
  REQUIRE(r3.is_err());
  REQUIRE(r3.unwrap_err() == std::string("abc"));
}

TEST_CASE("result<T, E>(result<T,E>&&)", "[result<T, E>]") {
  result_type1 r0(ok_type1("abc"));
  result_type1 r1(std::move(r0));
  REQUIRE(r1.is_ok());
  REQUIRE(r1.unwrap() == std::string("abc"));
  result_type2 r2(err_type1("abc"));
  result_type2 r3(std::move(r2));
  REQUIRE(r3.is_err());
  REQUIRE(r3.unwrap_err() == std::string("abc"));
}

TEST_CASE("result<T, E>::operator(const result<T,E>&)", "[result<T, E>]") {
  result_type1 r0(ok_type1("abc"));
  result_type1 r1(ok_type1("def"));
  r1 = r0;
  REQUIRE(r1.is_ok());
  REQUIRE(r1.unwrap() == std::string("abc"));

  result_type2 r2(err_type1("abc"));
  result_type2 r3(err_type1("def"));
  r3 = r2;
  REQUIRE(r3.is_err());
  REQUIRE(r3.unwrap_err() == std::string("abc"));
}

TEST_CASE("result<T, E>::operator(const result<T,E>&&)", "[result<T, E>]") {
  result_type1 r0(ok_type1("abc"));
  result_type1 r1(ok_type1("def"));
  r1 = std::move(r0);
  REQUIRE(r1.is_ok());
  REQUIRE(r1.unwrap() == std::string("abc"));

  result_type2 r2(err_type1("abc"));
  result_type2 r3(err_type1("def"));
  r3 = std::move(r2);
  REQUIRE(r3.is_err());
  REQUIRE(r3.unwrap_err() == std::string("abc"));
}

TEST_CASE("result<T, E>::operator bool()", "[result<T, E>]") {
  result_type1 r0(ok_type1("abc"));
  result_type2 r1(err_type1("def"));
  REQUIRE(r0.operator bool());
  REQUIRE_FALSE(r1.operator bool());
}

TEST_CASE("result<T, E>(ok_tag_t, Args &&... args)", "[result<T, E>]") {
  std::string s1("abc");
  result_type1 r1(result::ok_tag, "abc");
  REQUIRE(r1.is_ok());
  REQUIRE(r1.unwrap() == std::string("abc"));
}

TEST_CASE("result<T, E>::is_ok()", "[result<T, E>]") {
  ok_type1 s1("abc");
  result_type1 r1(s1);
  REQUIRE(r1.is_ok());
  err_type1 s2("abc");
  result_type2 r2(s2);
  REQUIRE_FALSE(r2.is_ok());
}

TEST_CASE("result<T, E>::is_err()", "[result<T, E>]") {
  result::err<std::string> s1("abc");
  result_type2 r1(s1);
  REQUIRE(r1.is_err());
  ok_type1 s2("abc");
  result_type1 r2(s2);
  REQUIRE_FALSE(r2.is_err());
}

TEST_CASE("result<T, E>::operator=", "[result<T, E>]") {
  SECTION("operator=(const Result<T, E>&)") {
    result_type1 r1(ok_type1("abc"));
    result_type1 r2(ok_type1("def"));
    REQUIRE(r1.unwrap() == std::string("abc"));
    r1 = r2;
    REQUIRE(r1.unwrap() == std::string("def"));
  }
  SECTION("operator=(const Result<T, E>&)") {
    result_type1 r1(ok_type1("abc"));
    result_type1 r2(ok_type1("def"));
    result_type1 r3(err_type2(5.0));
    REQUIRE(r1.unwrap() == std::string("abc"));
    r1 = r2;
    REQUIRE(r1.unwrap() == std::string("def"));
    r1 = r3;
    REQUIRE(r1.is_err());
    REQUIRE(r1.unwrap_err() == 5.0);
  }
  SECTION("operator=(Result<T, E>&&)") {
    result_type1 r1(ok_type1("abc"));
    result_type1 r2(ok_type1("def"));
    result_type1 r3(err_type2(5.0));
    REQUIRE(r1.unwrap() == std::string("abc"));
    r1 = std::move(r2);
    REQUIRE(r1.unwrap() == std::string("def"));
    r1 = std::move(r3);
    REQUIRE(r1.is_err());
    REQUIRE(r1.unwrap_err() == 5.0);
  }
  SECTION("operator=(const ok<T>&)") {
    result_type1 r1(ok_type1("abc"));
    ok_type1 ok1("def");
    err_type2 err1(5.0);
    REQUIRE(r1.unwrap() == std::string("abc"));
    r1 = ok1;
    REQUIRE(r1.unwrap() == std::string("def"));
    r1 = err1;
    REQUIRE(r1.is_err());
    REQUIRE(r1.unwrap_err() == 5.0);
  }
  SECTION("operator=(ok<T>&&)") {
    result_type1 r1(ok_type1("abc"));
    ok_type1 ok1("def");
    err_type2 err1(5.0);
    REQUIRE(r1.unwrap() == std::string("abc"));
    r1 = std::move(ok1);
    REQUIRE(r1.unwrap() == std::string("def"));
    r1 = std::move(err1);
    REQUIRE(r1.is_err());
    REQUIRE(r1.unwrap_err() == 5.0);
  }
  SECTION("operator=(const err<T>&)") {
    result_type2 r1(ok_type2(5.0));
    ok_type2 ok1(2.0);
    err_type1 err1("abc");
    REQUIRE(r1.unwrap() == 5.0);
    r1 = ok1;
    REQUIRE(r1.unwrap() == 2.0);
    r1 = err1;
    REQUIRE(r1.is_err());
    REQUIRE(r1.unwrap_err() == std::string("abc"));
  }
  SECTION("operator=(err<T>&&)") {
    result_type2 r1(ok_type2(5.0));
    ok_type2 ok1(2.0);
    err_type1 err1("abc");
    REQUIRE(r1.unwrap() == 5.0);
    r1 = std::move(ok1);
    REQUIRE(r1.unwrap() == 2.0);
    r1 = std::move(err1);
    REQUIRE(r1.is_err());
    REQUIRE(r1.unwrap_err() == std::string("abc"));
  }
}

TEST_CASE("result<T, E>::operator==", "[result<T, E>]") {
  SECTION("result<T, E>::operator==(const result<T, E>&)") {
    result_type1 r1(ok_type1("abc"));
    result_type1 r2(ok_type1("abc"));
    REQUIRE(r1 == r2);
  }

  SECTION("result<T, E>::operator!=(const result<T, E>&)") {
    result_type1 r1(ok_type1("abc"));
    result_type1 r2(ok_type1("abcd"));
    REQUIRE(r1 != r2);
  }

  SECTION("result<T, E>::operator==(const ok<T>&)") {
    result_type1 r1(ok_type1("abc"));
    ok_type1 rhs("abc");
    REQUIRE(r1 == rhs);
  }

  SECTION("result<T, E>::operator!=(const ok<T>&)") {
    result_type1 r1(ok_type1("abc"));
    ok_type1 rhs("abcd");
    REQUIRE(r1 != rhs);
  }

  SECTION("result<T, E>::operator==(const err<T>&)") {
    result_type2 r1(err_type1("abc"));
    err_type1 rhs("abc");
    REQUIRE(r1 == rhs);
  }

  SECTION("result<T, E>::operator!=(const err<T>&)") {
    result_type2 r1(err_type1("abc"));
    err_type1 rhs("abcd");
    REQUIRE(r1 != rhs);
  }
}

TEST_CASE("result<T, E>::contains()", "[result<T, E>]") {
  result_type1 r1(ok_type1("abc"));
  result_type1 r2(err_type2(0.5));
  REQUIRE(r1.contains("abc"));
  REQUIRE_FALSE(r1.contains("abcd"));
  REQUIRE_FALSE(r2.contains("abc"));
}

TEST_CASE("result<T, E>::contains_err()", "[result<T, E>]") {
  result_type1 r1(ok_type1("abc"));
  result_type1 r2(err_type2(0.5));
  REQUIRE_FALSE(r1.contains_err(0.5));
  REQUIRE(r2.contains_err(0.5));
  REQUIRE_FALSE(r2.contains_err(0.6));
}

TEST_CASE("result<T, E>::ok()", "[result<T, E>]") {
  ok_type1 s1("abc");
  result_type1 r1(s1);
  REQUIRE(r1.is_ok());
  auto x = r1.ok();
  REQUIRE(x.has_value());
  REQUIRE(x.value().get() == std::string("abc"));

  err_type2 d1(5.0);
  result_type1 r2(d1);
  REQUIRE_FALSE(r2.is_ok());
  auto y = r2.ok();
  REQUIRE(y == std::nullopt);
  REQUIRE_FALSE(y.has_value());
}

TEST_CASE("result<T, E>::err()", "[result<T, E>]") {
  result::err<std::string> s1("abc");
  result_type2 r1(s1);
  REQUIRE(r1.is_err());
  auto x = r1.err();
  REQUIRE(x.has_value());
  REQUIRE(x.value().get() == std::string("abc"));

  ok_type2 d1(5.0);
  result_type2 r2(d1);
  REQUIRE_FALSE(r2.is_err());
  auto y = r2.err();
  REQUIRE(y == std::nullopt);
  REQUIRE_FALSE(y.has_value());
}

TEST_CASE("result<T, E>::ok_unchecked()", "[result<T, E>]") {
  ok_type1 s1("abc");
  result_type1 r1(s1);
  REQUIRE(r1.is_ok());
  REQUIRE(r1.ok_unchecked() == std::string("abc"));
  REQUIRE(std::move(r1).ok_unchecked() == std::string("abc"));
}

TEST_CASE("result<T, E>::err_unchecked()", "[result<T, E>]") {
  result::err<std::string> s1("abc");
  result_type2 r1(s1);
  REQUIRE(r1.is_err());
  REQUIRE(r1.err_unchecked() == std::string("abc"));
  REQUIRE(std::move(r1).err_unchecked() == std::string("abc"));
}

TEST_CASE("result<T, E>::try_ok()", "[result<T, E>]") {
  ok_type1 s1("abc");
  result_type1 r1(s1);
  REQUIRE(r1.is_ok());
  REQUIRE(r1.try_ok() == std::string("abc"));
}

TEST_CASE("result<T, E>::try_err()", "[result<T, E>]") {
  result::err<std::string> s1("abc");
  result_type2 r1(s1);
  REQUIRE(r1.is_err());
  REQUIRE(r1.try_err() == std::string("abc"));
}

TEST_CASE("result<T, E>::unwrap()", "[result<T, E>]") {
  ok_type1 s1("abc");
  result_type1 r1(s1);
  REQUIRE(r1.is_ok());
  REQUIRE(r1.unwrap() == std::string("abc"));
}

TEST_CASE("result<T, E>::unwrap_or()", "[result<T, E>]") {
  err_type2 d1(5.0);
  result_type1 r1(d1);
  REQUIRE(r1.is_err());
  REQUIRE(r1.unwrap_or("") == std::string(""));
}

TEST_CASE("result<T, E>::unwrap_or_default()", "[result<T, E>]") {
  err_type1 d1("abc");
  result_type2 r1(d1);
  REQUIRE(r1.is_err());
  REQUIRE(r1.unwrap_or_default() == double());
}

TEST_CASE("result<T, E>::unwrap_err()", "[result<T, E>]") {
  result::err<std::string> s1("abc");
  result_type2 r1(s1);
  REQUIRE(r1.is_err());
  REQUIRE(r1.unwrap_err() == std::string("abc"));
}

TEST_CASE("result<T, E>::unwrap_err_or_default()", "[result<T, E>]") {
  result::ok<std::string> s1("abc");
  result_type1 r1(s1);
  REQUIRE(r1.is_ok());
  REQUIRE(r1.unwrap_err_or_default() == double());
}

TEST_CASE("result<T, E> transformations") {
  SECTION("map") {
    result_type1 r1(ok_type1("abc"));
    auto fun = [](const std::string &x) {
      std::string s(x);
      s += std::string("def");
      return s;
    };
    auto r2 = r1.map(fun);
    REQUIRE(r2.is_ok());
    REQUIRE(r2.unwrap() == std::string("abcdef"));

    result_type1 r3(err_type2(5.0));
    auto r4 = r3.map(fun);
    REQUIRE(r4.is_err());
    REQUIRE(r4.unwrap_err() == 5.0);
  }
  SECTION("map_err") {
    result_type1 r1(ok_type1("abc"));
    auto fun = [](const double &x) {
      double d = x;
      d += 5.0;
      return d;
    };
    auto r2 = r1.map_err(fun);
    REQUIRE(r2.is_ok());
    REQUIRE(r2.unwrap() == std::string("abc"));

    result_type1 r3(err_type2(5.0));
    auto r4 = r3.map_err(fun);
    REQUIRE(r4.is_err());
    REQUIRE(r4.unwrap_err() == 10.0);
  }
  SECTION("map_or_else") {
    auto fun = [](const std::string &x) {
      std::string s(x);
      s += std::string("def");
      return s;
    };
    auto default_fun = [](const double &x) {
      if (x == 5.0)
        return std::string("five");
      return std::string("undefined");
    };
    result_type1 r1(ok_type1("abc"));
    result_type1 r2(err_type2(5.0));
    result_type1 r3(err_type2(3.0));
    REQUIRE(r1.map_or_else(default_fun, fun) == std::string("abcdef"));
    REQUIRE(r2.map_or_else(default_fun, fun) == std::string("five"));
    REQUIRE(r3.map_or_else(default_fun, fun) == std::string("undefined"));
  }
  SECTION("and_") {
    result_type1 r1(ok_type1("abc"));
    result_type1 r2(err_type2(5.0));
    using result_type3 = result::result<bool, double>;
    result_type3 r3(result::ok(true));
    REQUIRE(r1.and_(r3) == r3);
    REQUIRE(r2.and_(r3) == result_type3(result::err(5.0)));
  }
  SECTION("and_then") {
    result_type1 r1(ok_type1("abc"));
    result_type1 r2(ok_type1("abcdef"));
    result_type1 r3(err_type2(5.0));
    using result_type3 = result::result<bool, double>;
    auto fun = [](const std::string &x) {
      if (x == "abc")
        return result_type3(result::ok(true));
      return result_type3(result::ok(false));
    };
    REQUIRE(r1.and_then(fun) == result_type3(result::ok(true)));
    REQUIRE(r2.and_then(fun) == result_type3(result::ok(false)));
    REQUIRE(r3.and_then(fun) == result_type3(result::err(5.0)));
  }
  SECTION("or_") {
    result_type1 r1(ok_type1("abc"));
    result_type1 r2(err_type2(5.0));
    using result_type3 = result::result<std::string, bool>;
    result_type3 r3(result::err(true));
    REQUIRE(r1.or_(r3) == result_type3(ok_type1("abc")));
    REQUIRE(r2.or_(r3) == result_type3(result::err(true)));
  }
  SECTION("or_else") {
    result_type1 r1(ok_type1("abc"));
    result_type1 r2(err_type2(5.0));
    result_type1 r3(err_type2(3.0));
    using result_type3 = result::result<std::string, bool>;
    auto fun = [](const double &x) {
      if (x == 5.0)
        return result_type3(result::err(true));
      return result_type3(result::err(false));
    };
    REQUIRE(r1.or_else(fun) == result_type3(ok_type1("abc")));
    REQUIRE(r2.or_else(fun) == result_type3(result::err(true)));
    REQUIRE(r3.or_else(fun) == result_type3(result::err(false)));
  }
}

TEST_CASE("std::hash<result<T, E>>", "[std::hash]") {
  result_type1 r0(ok_type1("abc"));
  result_type1 r1(ok_type1("abc"));
  result_type2 r2(err_type1("abc"));
  REQUIRE(std::hash<result_type1>{}(r0) == std::hash<result_type1>{}(r1));
  // comparing the same value but with a different 'state' ok vs err
  REQUIRE(std::hash<result_type1>{}(r1) != std::hash<result_type2>{}(r2));

  result_type2 r3(ok_type2(5.0));
  result_type2 r4(ok_type2(5.0));
  result_type1 r5(err_type2(5.0));
  REQUIRE(std::hash<result_type2>{}(r3) == std::hash<result_type2>{}(r4));
  // comparing the same value but with a different 'state' ok vs err
  REQUIRE(std::hash<result_type2>{}(r3) != std::hash<result_type1>{}(r5));
}
