#include "common_tests/test_utils.h"

// POSIT8 INCREMENT/DECREMENT TESTS

TEST(Posit8Operators, Increment) {
  posit8 p = 5.0;
  posit8 original = p;
  ++p;

  ASSERT_TRUE(p > original);
}

TEST(Posit8Operators, Decrement) {
  posit8 p = 5.0;
  posit8 original = p;
  --p;

  ASSERT_TRUE(p < original);
} 