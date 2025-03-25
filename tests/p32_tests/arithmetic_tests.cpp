#include "common_tests/test_utils.h"
#include <gtest/gtest.h>
#include <random>

// POSIT32 ARITHMETIC TESTS

// Test case for addition
TEST(Posit32Arithmetic, Add) {
  current_operation = "Addition";
  test_exact<posit32>(
      NTESTS32,
      // Posit operation
      [](posit32 a, posit32 b) { return a + b; },
      // Equivalent double operation
      [](double a, double b) { return a + b; },
      // Random generator
      []() { return int_dist32(gen); });
}

// Test case for subtraction
TEST(Posit32Arithmetic, Sub) {
  current_operation = "Subtraction";
  test_exact<posit32>(
      NTESTS32,
      // Posit operation
      [](posit32 a, posit32 b) { return a - b; },
      // Equivalent double operation
      [](double a, double b) { return a - b; },
      // Random generator
      []() { return static_cast<int32_t>(int_dist32(gen)); });
}

// Test case for multiplication
TEST(Posit32Arithmetic, Mul) {
  current_operation = "Multiplication";
  test_exact<posit32>(
      NTESTS32,
      // Posit operation
      [](posit32 a, posit32 b) { return a * b; },
      // Equivalent double operation
      [](double a, double b) { return a * b; },
      // Random generator
      []() { return static_cast<int32_t>(int_dist32(gen)); });
}

// Test case for division
TEST(Posit32Arithmetic, Div) {
  current_operation = "Division";
  test_exact<posit32>(
      NTESTS32,
      // Posit operation
      [](posit32 a, posit32 b) {
        // Skip division by zero
        if (b.value == 0 || b.isNaR()) {
          return posit32(0);
        }
        return a / b;
      },
      // Equivalent double operation
      [](double a, double b) {
        if (b == 0 || std::isnan(b) || std::isinf(b)) {
          return 0.0;
        }
        return a / b;
      },
      // Random generator
      []() { return static_cast<int32_t>(int_dist32(gen)); });
}

// Test square root operation
TEST(Posit32Arithmetic, Sqrt) {
  current_operation = "Square Root";

  for (int i = 0; i < NTESTS32; i++) {
    // Generate random posit value
    posit32 p_a;
    p_a.value = int_dist32(gen);

    double f_a = p_a.toDouble();

    // Skip negative values for sqrt and NaN values
    if (f_a < 0 || std::isnan(f_a) || p_a.isNaR())
      continue;

    posit32 p_result = p_a;
    p_result.sqrt();

    double f_result = std::sqrt(f_a);
    posit32 expected = f_result;

    ASSERT_TRUE(double_eq(p_result.toDouble(), expected.toDouble()))
        << "sqrt(" << f_a << ") = " << f_result << " but got "
        << p_result.toDouble();
  }
}

// Test rounding operation
TEST(Posit32Arithmetic, Round) {
  current_operation = "Rounding";
  for (int i = 0; i < NTESTS32; i++) {
    // Generate random posit value
    posit32 p_a;
    p_a.value = int_dist32(gen);

    double f_a = p_a.toDouble();

    // Skip NaN values
    if (std::isnan(f_a) || p_a.isNaR())
      continue;

    posit32 p_result = p_a;
    p_result.rint();

    double f_result = std::round(f_a);
    posit32 expected = f_result;

    // Skip values at exactly 0.5 boundary like in Rust test
    if (std::abs(std::abs(f_a - std::floor(f_a)) - 0.5) < 1e-10) {
      continue;
    }

    ASSERT_TRUE(double_eq(p_result.toDouble(), expected.toDouble()))
        << "round(" << f_a << ") = " << f_result << " but got "
        << p_result.toDouble();
  }
}

// Test fused multiply-add operation
TEST(Posit32Arithmetic, MulAdd) {
  current_operation = "Fused Multiply-Add";
  for (int i = 0; i < NTESTS32; i++) {
    // Generate random posit values
    posit32 p_a, p_b, p_c;
    p_a.value = int_dist32(gen);
    p_b.value = int_dist32(gen);
    p_c.value = int_dist32(gen);

    double f_a = p_a.toDouble();
    double f_b = p_b.toDouble();
    double f_c = p_c.toDouble();

    posit32 p_result = p_c.fma(p_a, p_b);
    double f_result = std::fma(f_a, f_b, f_c);
    posit32 expected = f_result;

    // Allow up to 1 ULP difference for posit32 - less precise than posit32
    auto ulp_diff = ulp(p_result, expected);
    ASSERT_LE(ulp_diff, 1) << "fma(" << f_a << ", " << f_b << ", " << f_c
                           << ") = " << f_result << " but got "
                           << p_result.toDouble() << " (ULP diff: " << ulp_diff
                           << ")";
  }
}

// Test for quire_mul_add
TEST(Quire32Operations, MulAdd) {
  current_operation = "Quire Multiply-Add";
  for (int i = 0; i < NTESTS32; i++) {
    // Generate random posit values
    posit32 p_a, p_b, p_c;
    p_a.value = int_dist32(gen);
    p_b.value = int_dist32(gen);
    p_c.value = int_dist32(gen);

    double f_a = p_a.toDouble();
    double f_b = p_b.toDouble();
    double f_c = p_c.toDouble();

    // Create and initialize quire
    quire32 q;
    q.clr();

    // Perform operations (q += (p_a, p_b); q += p_c;)
    q.qma(p_a, p_b);
    q.qma(posit32(1.0), p_c);

    // Convert back to posit
    posit32 p = q.toPosit();

    // Calculate expected result using double
    double f = std::fma(f_a, f_b, f_c);
    posit32 expected = f;

    // Allow up to 1 ULP difference for quire operations
    auto ulp_diff = ulp(p, expected);
    ASSERT_LE(ulp_diff, 1) << "Input: (" << f_a << " * " << f_b << " + " << f_c
                           << ")"
                           << "\nResult: " << p.toDouble()
                           << "\nExpected: " << f << " (ULP diff: " << ulp_diff
                           << ")";
  }
}

// Test for quire_mul_sub
TEST(Quire32Operations, MulSub) {
  current_operation = "Quire Multiply-Subtract";
  for (int i = 0; i < NTESTS32; i++) {
    // Generate random posit values
    posit32 p_a, p_b, p_c;
    p_a.value = int_dist32(gen);
    p_b.value = int_dist32(gen);
    p_c.value = int_dist32(gen);

    double f_a = p_a.toDouble();
    double f_b = p_b.toDouble();
    double f_c = p_c.toDouble();

    // Create and initialize quire
    quire32 q;
    q.clr();

    // Perform operations (q -= (p_a, p_b); q += p_c;)
    q.qms(p_a, p_b);
    q.qma(posit32(1.0), p_c);

    // Convert back to posit
    posit32 p = q.toPosit();

    // Calculate expected result using double
    double f = std::fma(-f_a, f_b, f_c);
    posit32 expected = f;

    // Allow up to 1 ULP difference for quire operations
    auto ulp_diff = ulp(p, expected);
    ASSERT_LE(ulp_diff, 1) << "Input: (-" << f_a << " * " << f_b << " + " << f_c
                           << ")"
                           << "\nResult: " << p.toDouble()
                           << "\nExpected: " << f << " (ULP diff: " << ulp_diff
                           << ")";
  }
}