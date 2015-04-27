#include <echo/execution_context/tbb/blocked_range.h>
#include <echo/test.h>

using namespace echo;
using namespace echo::execution_context::intel_tbb;

TEST_CASE("tbb_blocked_range") {
  using Range = BlockedRange<int>;
  Range b1(5, 11, 2);

  SECTION("basic split") {
    Range b2(b1, tbb::split());
    CHECK(b1.begin() == 5);
    CHECK(b1.end() == 8);
    CHECK(b1.size() == 3);
    CHECK(!b1.empty());
    CHECK(b1.is_divisible());

    CHECK(b2.begin() == 8);
    CHECK(b2.end() == 11);

    Range b3(b2, tbb::split());
    CHECK((b3.size() + b2.size()) == 3);
    CHECK((!b3.is_divisible() || !b2.is_divisible()));
    CHECK(b2.end() == b3.begin());
  }

  SECTION("proportional split") {
    Range b2(b1, tbb::proportional_split(2, 1));
    CHECK(b1.begin() == 5);
    CHECK(b1.end() == 9);
    CHECK(b2.begin() == 9);
    CHECK(b2.end() == 11);
  }
}

TEST_CASE("tbb_k_blocked_range") {
  auto b1 = make_blocked_range(make_blocked_range(10, 60, 10),
                               make_blocked_range(20, 30));

  CHECK(!b1.empty());
  CHECK(b1.is_divisible());

  auto s1 = project<0>(b1);
  CHECK(s1.begin() == 10);
  CHECK(s1.end() == 60);
  CHECK(s1.grainsize() == 10);

  auto s2 = project<1>(b1);
  CHECK(s2.begin() == 20);
  CHECK(s2.end() == 30);
  CHECK(s2.grainsize() == 1);

  auto b2 = make_blocked_range(make_blocked_range(20, 30),
                               make_blocked_range(10, 60, 10));

  auto b3 = make_blocked_range(make_blocked_range(20, 30, 20),
                               make_blocked_range(10, 100));

  CHECK(b3.is_divisible());
  CHECK(!b3.empty());

  auto b4 = make_blocked_range(make_blocked_range(10, 100),
                               make_blocked_range(20, 30, 20));

  CHECK(b4.is_divisible());
  CHECK(!b4.empty());

  auto b5 = make_blocked_range(make_blocked_range(100, 100),
                               make_blocked_range(20, 30, 20));
  CHECK(b5.empty());

  auto b6 = make_blocked_range(make_blocked_range(20, 30, 20),
                               make_blocked_range(100, 100));
  CHECK(b6.empty());

  auto b7 = make_blocked_range(make_blocked_range(20, 30, 20),
                               make_blocked_range(20, 30, 20),
                               make_blocked_range(100, 100));
  CHECK(b7.empty());
}

TEST_CASE("tbb_k_blocked_range_split") {
  using Range = KBlockedRange<2, int, int>;
  auto b1 = make_blocked_range(make_blocked_range(10, 60, 10),
                               make_blocked_range(20, 30));
  auto b2 = make_blocked_range(make_blocked_range(20, 30),
                               make_blocked_range(10, 60, 10));

  SECTION("split1") {
    Range b3(b1, tbb::split());
    auto s11 = project<0>(b1);
    auto s12 = project<1>(b1);

    auto s21 = project<0>(b3);
    auto s22 = project<1>(b3);

    CHECK(s11.begin() == 10);
    CHECK(s11.end() == 60);
    CHECK(s21.begin() == 10);
    CHECK(s21.end() == 60);

    CHECK(s12.begin() == 20);
    CHECK(s12.end() == 25);
    CHECK(s22.begin() == 25);
    CHECK(s22.end() == 30);
  }

  SECTION("split2") {
    Range b3(b2, tbb::split());
    auto s11 = project<0>(b2);
    auto s12 = project<1>(b2);

    auto s21 = project<0>(b3);
    auto s22 = project<1>(b3);

    CHECK(s12.begin() == 10);
    CHECK(s12.end() == 60);
    CHECK(s22.begin() == 10);
    CHECK(s22.end() == 60);

    CHECK(s11.begin() == 20);
    CHECK(s11.end() == 25);
    CHECK(s21.begin() == 25);
    CHECK(s21.end() == 30);
  }
}

TEST_CASE("tbb_k_blocked_range_split3") {
  using Range = KBlockedRange<3, int, int, int>;
  auto b1 = make_blocked_range(make_blocked_range(10, 60, 10),
                               make_blocked_range(0, 100, 50),
                               make_blocked_range(20, 30));

  SECTION("split1") {
    Range b2(b1, tbb::split());
    auto s11 = project<0>(b1);
    auto s12 = project<1>(b1);
    auto s13 = project<2>(b1);

    auto s21 = project<0>(b2);
    auto s22 = project<1>(b2);
    auto s23 = project<2>(b2);

    CHECK(s11.begin() == 10);
    CHECK(s11.end() == 60);
    CHECK(s21.begin() == 10);
    CHECK(s21.end() == 60);

    CHECK(s12.begin() == 0);
    CHECK(s12.end() == 100);
    CHECK(s22.begin() == 0);
    CHECK(s22.end() == 100);

    CHECK(s13.begin() == 20);
    CHECK(s13.end() == 25);
    CHECK(s23.begin() == 25);
    CHECK(s23.end() == 30);
  }
}

TEST_CASE("tbb_blocked_range_from_shape") {
  SECTION("no grainularity") {
    auto b1 = make_blocked_range(make_k_shape(10, 5));
    auto s1 = project<0>(b1);
    auto s2 = project<1>(b1);

    CHECK(s1.begin() == 0);
    CHECK(s1.end() == 10);
    CHECK(s1.grainsize() == 1);

    CHECK(s2.begin() == 0);
    CHECK(s2.end() == 5);
    CHECK(s2.grainsize() == 1);
  }
  SECTION("with grainularity") {
    auto b1 = make_blocked_range(make_k_shape(10, 5), 5);
    auto s1 = project<0>(b1);
    auto s2 = project<1>(b1);

    CHECK(s1.begin() == 0);
    CHECK(s1.end() == 10);
    CHECK(s1.grainsize() == 5);

    CHECK(s2.begin() == 0);
    CHECK(s2.end() == 5);
    CHECK(s2.grainsize() == 1);
  }
}
