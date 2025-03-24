#include "common_tests/test_utils.h"

// Random distributions for p16 tests
std::uniform_int_distribution<int16_t> int_dist16(INT16_MIN, INT16_MAX);

//========================================================================================
// POSIT16 TESTS
//========================================================================================

// Test case for addition
TEST(Posit16Arithmetic, Add)
{
    current_operation = "Addition";
    test_exact<posit16>(
        NTESTS16,
        // Posit operation
        [](posit16 a, posit16 b)
        { return a + b; },
        // Equivalent double operation
        [](double a, double b)
        { return a + b; },
        // Random generator
        []()
        { return int_dist16(gen); });
}

// Test case for subtraction
TEST(Posit16Arithmetic, Sub)
{
    current_operation = "Subtraction";
    test_exact<posit16>(
        NTESTS16,
        // Posit operation
        [](posit16 a, posit16 b)
        { return a - b; },
        // Equivalent double operation
        [](double a, double b)
        { return a - b; },
        // Random generator
        []()
        { return static_cast<int16_t>(int_dist16(gen)); });
}

// Test case for multiplication
TEST(Posit16Arithmetic, Mul)
{
    current_operation = "Multiplication";
    test_exact<posit16>(
        NTESTS16,
        // Posit operation
        [](posit16 a, posit16 b)
        { return a * b; },
        // Equivalent double operation
        [](double a, double b)
        { return a * b; },
        // Random generator
        []()
        { return static_cast<int16_t>(int_dist16(gen)); });
}

// Test case for division
TEST(Posit16Arithmetic, Div)
{
    current_operation = "Division";
    test_exact<posit16>(
        NTESTS16,
        // Posit operation
        [](posit16 a, posit16 b)
        {
            // Skip division by zero
            if (b.value == 0 || b.isNaR())
            {
                return posit16(0);
            }
            return a / b;
        },
        // Equivalent double operation
        [](double a, double b)
        {
            if (b == 0 || std::isnan(b) || std::isinf(b))
            {
                return 0.0;
            }
            return a / b;
        },
        // Random generator
        []()
        { return static_cast<int16_t>(int_dist16(gen)); });
}

// Test square root operation
TEST(Posit16Arithmetic, Sqrt)
{
    current_operation = "Square Root";

    for (int i = 0; i < NTESTS16; i++)
    {
        // Generate random posit value
        posit16 p_a;
        p_a.value = int_dist16(gen);

        double f_a = p_a.toDouble();

        // Skip negative values for sqrt and NaN values
        if (f_a < 0 || std::isnan(f_a) || p_a.isNaR())
            continue;

        posit16 p_result = p_a;
        p_result.sqrt();

        double f_result = std::sqrt(f_a);
        posit16 expected = f_result;

        ASSERT_TRUE(double_eq(p_result.toDouble(), expected.toDouble()))
            << "sqrt(" << f_a << ") = " << f_result
            << " but got " << p_result.toDouble();
    }
}

// Test rounding operation
TEST(Posit16Arithmetic, Round)
{
    current_operation = "Rounding";
    for (int i = 0; i < NTESTS16; i++)
    {
        // Generate random posit value
        posit16 p_a;
        p_a.value = int_dist16(gen);

        double f_a = p_a.toDouble();

        // Skip NaN values
        if (std::isnan(f_a) || p_a.isNaR())
            continue;

        posit16 p_result = p_a;
        p_result.rint();

        double f_result = std::round(f_a);
        posit16 expected = f_result;

        // Skip values at exactly 0.5 boundary like in Rust test
        if (std::abs(std::abs(f_a - std::floor(f_a)) - 0.5) < 1e-10)
        {
            continue;
        }

        ASSERT_TRUE(double_eq(p_result.toDouble(), expected.toDouble()))
            << "round(" << f_a << ") = " << f_result
            << " but got " << p_result.toDouble();
    }
}

// Test fused multiply-add operation
TEST(Posit16Arithmetic, MulAdd)
{
    current_operation = "Fused Multiply-Add";
    for (int i = 0; i < NTESTS16; i++)
    {
        // Generate random posit values
        posit16 p_a, p_b, p_c;
        p_a.value = int_dist16(gen);
        p_b.value = int_dist16(gen);
        p_c.value = int_dist16(gen);

        double f_a = p_a.toDouble();
        double f_b = p_b.toDouble();
        double f_c = p_c.toDouble();

        posit16 p_result = p_c.fma(p_a, p_b);
        double f_result = std::fma(f_a, f_b, f_c);
        posit16 expected = f_result;

        // Allow up to 1 ULP difference for posit16 - less precise than posit32
        auto ulp_diff = ulp(p_result, expected);
        ASSERT_LE(ulp_diff, 1)
            << "fma(" << f_a << ", " << f_b << ", " << f_c << ") = " << f_result
            << " but got " << p_result.toDouble() << " (ULP diff: " << ulp_diff << ")";
    }
}

//========================================================================================
// QUIRE16 TESTS
//========================================================================================

// Test for quire_mul_add
TEST(Quire16Operations, MulAdd)
{
    current_operation = "Quire Multiply-Add";
    for (int i = 0; i < NTESTS16; i++)
    {
        // Generate random posit values
        posit16 p_a, p_b, p_c;
        p_a.value = int_dist16(gen);
        p_b.value = int_dist16(gen);
        p_c.value = int_dist16(gen);

        double f_a = p_a.toDouble();
        double f_b = p_b.toDouble();
        double f_c = p_c.toDouble();

        // Create and initialize quire
        quire16 q;
        q.clr();

        // Perform operations (q += (p_a, p_b); q += p_c;)
        q.qma(p_a, p_b);
        q.qma(posit16(1.0), p_c);

        // Convert back to posit
        posit16 p = q.toPosit();

        // Calculate expected result using double
        double f = std::fma(f_a, f_b, f_c);
        posit16 expected = f;

        // Allow up to 1 ULP difference for quire operations
        auto ulp_diff = ulp(p, expected);
        ASSERT_LE(ulp_diff, 1)
            << "Input: (" << f_a << " * " << f_b << " + " << f_c << ")"
            << "\nResult: " << p.toDouble()
            << "\nExpected: " << f << " (ULP diff: " << ulp_diff << ")";
    }
}

// Test for quire_mul_sub
TEST(Quire16Operations, MulSub)
{
    current_operation = "Quire Multiply-Subtract";
    for (int i = 0; i < NTESTS16; i++)
    {
        // Generate random posit values
        posit16 p_a, p_b, p_c;
        p_a.value = int_dist16(gen);
        p_b.value = int_dist16(gen);
        p_c.value = int_dist16(gen);

        double f_a = p_a.toDouble();
        double f_b = p_b.toDouble();
        double f_c = p_c.toDouble();

        // Create and initialize quire
        quire16 q;
        q.clr();

        // Perform operations (q -= (p_a, p_b); q += p_c;)
        q.qms(p_a, p_b);
        q.qma(posit16(1.0), p_c);

        // Convert back to posit
        posit16 p = q.toPosit();

        // Calculate expected result using double
        double f = std::fma(-f_a, f_b, f_c);
        posit16 expected = f;

        // Allow up to 1 ULP difference for quire operations
        auto ulp_diff = ulp(p, expected);
        ASSERT_LE(ulp_diff, 1)
            << "Input: (-" << f_a << " * " << f_b << " + " << f_c << ")"
            << "\nResult: " << p.toDouble()
            << "\nExpected: " << f << " (ULP diff: " << ulp_diff << ")";
    }
}

//========================================================================================
// CONVERSION TESTS
//========================================================================================

// Test conversion from posit16 to double for all possible values
TEST(Posit16Conversion, ConvertP16ToF64)
{
    for (int n = -1000; n < 1000; n++)
    {
        posit16 p;
        p = n;
        double f = p.toDouble();
        posit16 p_back = f;

        ASSERT_EQ(p.value, p_back.value)
            << "Failed at n=" << n
            << ": original=" << p.value
            << ", converted back=" << p_back.value;
    }

    // Test random larger values
    for (int i = 0; i < 1000; i++)
    {
        int16_t n = static_cast<int16_t>(int_dist16(gen));
        posit16 p;
        p = n;
        double f = p.toDouble();
        posit16 p_back = f;

        ASSERT_EQ(p.value, p_back.value)
            << "Failed at n=" << n
            << ": original=" << p.value
            << ", converted back=" << p_back.value;
    }
}

// Test conversion from posit16 to float
TEST(Posit16Conversion, ConvertP16ToF32)
{
    for (int n = -1000; n < 1000; n++)
    {
        posit16 p;
        p = n;
        double d = p.toDouble();
        float f = static_cast<float>(d);
        double d_back = static_cast<double>(f);
        posit16 p_back = d_back;

        ASSERT_EQ(p.value, p_back.value)
            << "Failed at n=" << n
            << ": original=" << p.value
            << ", converted back=" << p_back.value;
    }
}

// Test random conversion from double to posit16
TEST(Posit16Conversion, ConvertF64ToP16Rand)
{
    std::uniform_real_distribution<double> real_dist(-100.0, 100.0);
    for (int i = 0; i < NTESTS16; i++)
    {
        double f = real_dist(gen);
        posit16 p = f;

        // Just ensure no crashes
        ASSERT_NO_THROW({
            double d = p.toDouble();
            (void)d; // Avoid unused variable warning
        });
    }
}

// Test random conversion from float to posit16
TEST(Posit16Conversion, ConvertF32ToP16Rand)
{
    std::uniform_real_distribution<double> real_dist(-100.0, 100.0);
    for (int i = 0; i < NTESTS16; i++)
    {
        float f = static_cast<float>(real_dist(gen));
        double d = static_cast<double>(f);
        posit16 p = d;

        // Just ensure no crashes
        ASSERT_NO_THROW({
            double d_back = p.toDouble();
            (void)d_back; // Avoid unused variable warning
        });
    }
}

// Test conversion from posit16 to int32
TEST(Posit16Conversion, ConvertP16ToI32)
{
    for (int n = -1000; n < 1000; n++)
    {
        posit16 p;
        p = n;
        double f = p.toDouble();
        long long int i_from_p = p.toInt();
        int32_t i_from_f = static_cast<int32_t>(std::round(f));

        // Skip very small values where precision might be an issue
        if (std::abs(f) < 1e-6)
        {
            continue;
        }

        ASSERT_EQ(i_from_p, i_from_f)
            << "Failed at n=" << n
            << ": i_from_p=" << i_from_p
            << ", i_from_f=" << i_from_f;
    }
}

// Test conversion from posit16 to int64
TEST(Posit16Conversion, ConvertP16ToI64)
{
    for (int n = -1000; n < 1000; n++)
    {
        posit16 p;
        p = n;
        double f = p.toDouble();
        long long int i_from_p = p.toRInt();
        int64_t i_from_f = static_cast<int64_t>(std::round(f));

        // Skip very small values where precision might be an issue
        if (std::abs(f) < 1e-6)
        {
            continue;
        }

        ASSERT_EQ(i_from_p, i_from_f)
            << "Failed at n=" << n
            << ": i_from_p=" << i_from_p
            << ", i_from_f=" << i_from_f;
    }
}

//========================================================================================
// SPECIAL VALUES TESTS
//========================================================================================

TEST(Posit16Special, NaR)
{
    // Test NaR detection
    posit16 p;
    p.toNaR();
    ASSERT_TRUE(p.isNaR());
    ASSERT_EQ(p.value, 0x8000);

    // Test converting NaR to double (should not crash)
    ASSERT_NO_THROW({
        double d = p.toDouble();
        (void)d; // Avoid unused variable warning
    });
}

//========================================================================================
// COMPARISON OPERATOR TESTS
//========================================================================================

TEST(Posit16Comparison, LessThan)
{
    double a = 5.0;
    double b = 10.0;
    posit16 pa = a;
    posit16 pb = b;

    ASSERT_TRUE(pa < pb);
    ASSERT_FALSE(pb < pa);

    // Test with negative numbers
    pa = -a;
    pb = -b;
    ASSERT_FALSE(pa < pb);
    ASSERT_TRUE(pb < pa);
}

TEST(Posit16Comparison, LessThanEqual)
{
    double a = 5.0;
    double b = 5.0;
    double c = 10.0;
    posit16 pa = a;
    posit16 pb = b;
    posit16 pc = c;

    ASSERT_TRUE(pa <= pb);
    ASSERT_TRUE(pa <= pc);
    ASSERT_FALSE(pc <= pa);
}

TEST(Posit16Comparison, Equal)
{
    double a = 5.0;
    double b = 5.0;
    double c = 10.0;
    posit16 pa = a;
    posit16 pb = b;
    posit16 pc = c;

    ASSERT_TRUE(pa == pb);
    ASSERT_FALSE(pa == pc);
}

TEST(Posit16Comparison, NotEqual)
{
    double a = 5.0;
    double b = 5.0;
    double c = 10.0;
    posit16 pa = a;
    posit16 pb = b;
    posit16 pc = c;

    ASSERT_FALSE(pa != pb);
    ASSERT_TRUE(pa != pc);
}

TEST(Posit16Comparison, GreaterThan)
{
    double a = 10.0;
    double b = 5.0;
    posit16 pa = a;
    posit16 pb = b;

    ASSERT_TRUE(pa > pb);
    ASSERT_FALSE(pb > pa);

    // Test with negative numbers
    pa = -a;
    pb = -b;
    ASSERT_FALSE(pa > pb);
    ASSERT_TRUE(pb > pa);
}

TEST(Posit16Comparison, GreaterThanEqual)
{
    double a = 5.0;
    double b = 5.0;
    double c = 10.0;
    posit16 pa = a;
    posit16 pb = b;
    posit16 pc = c;

    ASSERT_TRUE(pa >= pb);
    ASSERT_TRUE(pc >= pa);
    ASSERT_FALSE(pa >= pc);
}

//========================================================================================
// INCREMENT/DECREMENT TESTS
//========================================================================================

TEST(Posit16Operators, Increment)
{
    posit16 p = 5.0;
    posit16 original = p;
    ++p;

    ASSERT_TRUE(p > original);
}

TEST(Posit16Operators, Decrement)
{
    posit16 p = 5.0;
    posit16 original = p;
    --p;

    ASSERT_TRUE(p < original);
}

//========================================================================================
// ADVANCED TESTS
//========================================================================================

// Test that adding and then subtracting the same value returns the original
TEST(Posit16Advanced, AddSubCancel)
{
    current_operation = "Add-Sub Cancellation";

    // Create a distribution that generates values in the range [-MAX/2, MAX/2]
    std::uniform_int_distribution<int16_t> safe_dist(INT16_MIN / 2, INT16_MAX / 2);

    for (int i = 0; i < NTESTS16 / 10; i++)
    { // Reduced iteration count
        posit16 p_a, p_b;
        p_a.value = safe_dist(gen);
        p_b.value = safe_dist(gen);

        // Skip NaN/NaR values
        if (p_a.isNaR() || p_b.isNaR() || std::isnan(p_a.toDouble()) || std::isnan(p_b.toDouble()))
        {
            continue;
        }

        posit16 sum = p_a + p_b;
        posit16 original = sum - p_b;

        // Allow a slightly higher tolerance for posit16
        ASSERT_TRUE(double_eq(original.toDouble(), p_a.toDouble(), 1e-12, 1e-1))
            << "Failed: (" << p_a.toDouble() << " + " << p_b.toDouble() << ") - " << p_b.toDouble()
            << " = " << original.toDouble() << " but expected " << p_a.toDouble();
    }
}

// Test that multiplying and then dividing by the same value returns the original
TEST(Posit16Advanced, MulDivCancel)
{
    current_operation = "Mul-Div Cancellation";

    // Create a distribution that generates values in the range [-MAX/2, MAX/2]
    std::uniform_int_distribution<int16_t> safe_dist(INT16_MIN / 2, INT16_MAX / 2);

    for (int i = 0; i < NTESTS16 / 10; i++)
    { // Reduced iteration count
        posit16 p_a, p_b;
        p_a.value = safe_dist(gen);
        p_b.value = safe_dist(gen);

        // Skip division by zero, NaR, NaN, or very small numbers
        if (p_b.value == 0 || p_b.isNaR() || p_a.isNaR() ||
            std::isnan(p_b.toDouble()) || std::isnan(p_a.toDouble()) ||
            std::abs(p_b.toDouble()) < 1e-6)
        {
            continue;
        }

        posit16 product = p_a * p_b;
        posit16 original = product / p_b;

        // Allow a slightly higher tolerance for posit16
        ASSERT_TRUE(double_eq(original.toDouble(), p_a.toDouble(), 1e-12, 1e-1))
            << "Failed: (" << p_a.toDouble() << " * " << p_b.toDouble() << ") / " << p_b.toDouble()
            << " = " << original.toDouble() << " but expected " << p_a.toDouble();
    }
}

// Main function
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    // Add the custom listener
    ::testing::TestEventListeners &listeners = ::testing::UnitTest::GetInstance()->listeners();
    listeners.Append(new EpsilonStatisticsPrinter);

    return RUN_ALL_TESTS();
}