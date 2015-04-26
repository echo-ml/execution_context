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
