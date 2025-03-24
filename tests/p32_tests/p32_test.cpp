#include "common_tests/test_utils.h"

// Random distributions for p32 tests
std::uniform_int_distribution<int32_t> int_dist32(INT32_MIN/2, INT32_MAX/2);
std::uniform_real_distribution<double> real_dist(-100.0, 100.0);

//========================================================================================
// POSIT32 TESTS
//========================================================================================

// Test case for addition
TEST(Posit32Arithmetic, Add)
{
    current_operation = "Addition";
    test_exact<posit32>(
        NTESTS32,
        // Posit operation
        [](posit32 a, posit32 b)
        { return a + b; },
        // Equivalent double operation
        [](double a, double b)
        { return a + b; },
        // Random generator
        []()
        { return int_dist32(gen); });
}

// Test case for subtraction
TEST(Posit32Arithmetic, Sub)
{
    current_operation = "Subtraction";
    test_exact<posit32>(
        NTESTS32,
        // Posit operation
        [](posit32 a, posit32 b)
        { return a - b; },
        // Equivalent double operation
        [](double a, double b)
        { return a - b; },
        // Random generator
        []()
        { return int_dist32(gen); });
}

// Test case for multiplication
TEST(Posit32Arithmetic, Mul)
{
    current_operation = "Multiplication";
    test_exact<posit32>(
        NTESTS32,
        // Posit operation
        [](posit32 a, posit32 b)
        { return a * b; },
        // Equivalent double operation
        [](double a, double b)
        { return a * b; },
        // Random generator
        []()
        { return int_dist32(gen); });
}

// Test case for division
TEST(Posit32Arithmetic, Div)
{
    current_operation = "Division";
    test_exact<posit32>(
        NTESTS32,
        // Posit operation
        [](posit32 a, posit32 b)
        {
            // Skip division by zero
            if (b.value == 0 || b.isNaR())
            {
                return posit32(0);
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
        { return int_dist32(gen); });
}

// Test square root operation
TEST(Posit32Arithmetic, Sqrt)
{
    current_operation = "Square Root";

    for (int i = 0; i < NTESTS32; i++)
    {
        // Generate random posit value
        posit32 p_a;
        p_a.value = int_dist32(gen);

        double f_a = p_a.toDouble();

        // Skip negative values for sqrt
        if (f_a < 0)
            continue;

        posit32 p_result = p_a;
        p_result.sqrt();

        double f_result = std::sqrt(f_a);
        posit32 expected = f_result;

        ASSERT_TRUE(double_eq(p_result.toDouble(), expected.toDouble()))
            << "sqrt(" << f_a << ") = " << f_result
            << " but got " << p_result.toDouble();
    }
}

// Test rounding operation
TEST(Posit32Arithmetic, Round)
{
    current_operation = "Rounding";
    for (int i = 0; i < NTESTS32; i++)
    {
        // Generate random posit value
        posit32 p_a;
        p_a.value = int_dist32(gen);

        double f_a = p_a.toDouble();

        posit32 p_result = p_a;
        p_result.rint();

        double f_result = std::round(f_a);
        posit32 expected = f_result;

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
TEST(Posit32Arithmetic, MulAdd)
{
    current_operation = "Fused Multiply-Add";
    for (int i = 0; i < NTESTS32; i++)
    {
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

        ASSERT_TRUE(double_eq(p_result.toDouble(), expected.toDouble()))
            << "fma(" << f_a << ", " << f_b << ", " << f_c << ") = " << f_result
            << " but got " << p_result.toDouble();
    }
}

//========================================================================================
// QUIRE32 TESTS
//========================================================================================

// Test for quire_mul_add
TEST(Quire32Operations, MulAdd)
{
    current_operation = "Quire Multiply-Add";
    for (int i = 0; i < NTESTS32; i++)
    {
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

        ASSERT_TRUE(double_eq(p.toDouble(), expected.toDouble()))
            << "Input: (" << f_a << " * " << f_b << " + " << f_c << ")"
            << "\nResult: " << p.toDouble()
            << "\nExpected: " << f;
    }
}

// Test for quire_mul_sub
TEST(Quire32Operations, MulSub)
{
    current_operation = "Quire Multiply-Subtract";
    for (int i = 0; i < NTESTS32; i++)
    {
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

        ASSERT_TRUE(double_eq(p.toDouble(), expected.toDouble()))
            << "Input: (-" << f_a << " * " << f_b << " + " << f_c << ")"
            << "\nResult: " << p.toDouble()
            << "\nExpected: " << f;
    }
}

//========================================================================================
// CONVERSION TESTS
//========================================================================================

// Test conversion from posit32 to double for all possible values
TEST(Posit32Conversion, ConvertP32ToF64)
{
    for (int n = -1000; n < 1000; n++)
    {
        posit32 p;
        p = n;
        double f = p.toDouble();
        posit32 p_back = f;

        ASSERT_EQ(p.value, p_back.value)
            << "Failed at n=" << n
            << ": original=" << p.value
            << ", converted back=" << p_back.value;
    }

    // Test random larger values
    for (int i = 0; i < 1000; i++)
    {
        int32_t n = static_cast<int32_t>(int_dist32(gen));
        posit32 p;
        p = n;
        double f = p.toDouble();
        posit32 p_back = f;

        ASSERT_EQ(p.value, p_back.value)
            << "Failed at n=" << n
            << ": original=" << p.value
            << ", converted back=" << p_back.value;
    }
}

// Test conversion from posit32 to float
TEST(Posit32Conversion, ConvertP32ToF32)
{
    for (int n = -1000; n < 1000; n++)
    {
        posit32 p;
        p = n;
        double d = p.toDouble();
        float f = static_cast<float>(d);
        double d_back = static_cast<double>(f);
        posit32 p_back = d_back;

        ASSERT_EQ(p.value, p_back.value)
            << "Failed at n=" << n
            << ": original=" << p.value
            << ", converted back=" << p_back.value;
    }
}

// Test random conversion from double to posit32
TEST(Posit32Conversion, ConvertF64ToP32Rand)
{
    for (int i = 0; i < NTESTS32; i++)
    {
        double f = real_dist(gen);
        posit32 p = f;

        // Just ensure no crashes
        ASSERT_NO_THROW({
            double d = p.toDouble();
            (void)d; // Avoid unused variable warning
        });
    }
}

// Test random conversion from float to posit32
TEST(Posit32Conversion, ConvertF32ToP32Rand)
{
    for (int i = 0; i < NTESTS32; i++)
    {
        float f = static_cast<float>(real_dist(gen));
        double d = static_cast<double>(f);
        posit32 p = d;

        // Just ensure no crashes
        ASSERT_NO_THROW({
            double d_back = p.toDouble();
            (void)d_back; // Avoid unused variable warning
        });
    }
}

// Test conversion from posit32 to int32
TEST(Posit32Conversion, ConvertP32ToI32)
{
    for (int n = -1000; n < 1000; n++)
    {
        posit32 p;
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

// Test conversion from posit32 to int64
TEST(Posit32Conversion, ConvertP32ToI64)
{
    for (int n = -1000; n < 1000; n++)
    {
        posit32 p;
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

TEST(Posit32Special, NaR)
{
    // Test NaR detection
    posit32 p;
    p.toNaR();
    ASSERT_TRUE(p.isNaR());
    ASSERT_EQ(p.value, 0x80000000);

    // Test converting NaR to double (should not crash)
    ASSERT_NO_THROW({
        double d = p.toDouble();
        (void)d; // Avoid unused variable warning
    });
}

//========================================================================================
// COMPARISON OPERATOR TESTS
//========================================================================================

TEST(Posit32Comparison, LessThan)
{
    double a = 5.0;
    double b = 10.0;
    posit32 pa = a;
    posit32 pb = b;

    ASSERT_TRUE(pa < pb);
    ASSERT_FALSE(pb < pa);

    // Test with negative numbers
    pa = -a;
    pb = -b;
    ASSERT_FALSE(pa < pb);
    ASSERT_TRUE(pb < pa);
}

TEST(Posit32Comparison, LessThanEqual)
{
    double a = 5.0;
    double b = 5.0;
    double c = 10.0;
    posit32 pa = a;
    posit32 pb = b;
    posit32 pc = c;

    ASSERT_TRUE(pa <= pb);
    ASSERT_TRUE(pa <= pc);
    ASSERT_FALSE(pc <= pa);
}

TEST(Posit32Comparison, Equal)
{
    double a = 5.0;
    double b = 5.0;
    double c = 10.0;
    posit32 pa = a;
    posit32 pb = b;
    posit32 pc = c;

    ASSERT_TRUE(pa == pb);
    ASSERT_FALSE(pa == pc);
}

TEST(Posit32Comparison, NotEqual)
{
    double a = 5.0;
    double b = 5.0;
    double c = 10.0;
    posit32 pa = a;
    posit32 pb = b;
    posit32 pc = c;

    ASSERT_FALSE(pa != pb);
    ASSERT_TRUE(pa != pc);
}

TEST(Posit32Comparison, GreaterThan)
{
    double a = 10.0;
    double b = 5.0;
    posit32 pa = a;
    posit32 pb = b;

    ASSERT_TRUE(pa > pb);
    ASSERT_FALSE(pb > pa);

    // Test with negative numbers
    pa = -a;
    pb = -b;
    ASSERT_FALSE(pa > pb);
    ASSERT_TRUE(pb > pa);
}

TEST(Posit32Comparison, GreaterThanEqual)
{
    double a = 5.0;
    double b = 5.0;
    double c = 10.0;
    posit32 pa = a;
    posit32 pb = b;
    posit32 pc = c;

    ASSERT_TRUE(pa >= pb);
    ASSERT_TRUE(pc >= pa);
    ASSERT_FALSE(pa >= pc);
}

//========================================================================================
// INCREMENT/DECREMENT TESTS
//========================================================================================

TEST(Posit32Operators, Increment)
{
    posit32 p = 5.0;
    posit32 original = p;
    ++p;

    ASSERT_TRUE(p > original);
}

TEST(Posit32Operators, Decrement)
{
    posit32 p = 5.0;
    posit32 original = p;
    --p;

    ASSERT_TRUE(p < original);
}

//========================================================================================
// BIT OPERATIONS TESTS
//========================================================================================

TEST(Posit32BitOperations, BitwiseNot)
{
    posit32 p;
    p.value = 0x55555555;
    posit32 not_p = ~p;
    ASSERT_EQ(not_p.value, 0xAAAAAAAA);
}

TEST(Posit32BitOperations, BitwiseAnd)
{
    posit32 a, b;
    a.value = 0x55555555;
    b.value = 0x33333333;
    posit32 result = a & b;
    ASSERT_EQ(result.value, 0x11111111);
}

TEST(Posit32BitOperations, BitwiseOr)
{
    posit32 a, b;
    a.value = 0x55555555;
    b.value = 0x33333333;
    posit32 result = a | b;
    ASSERT_EQ(result.value, 0x77777777);
}

TEST(Posit32BitOperations, BitwiseXor)
{
    posit32 a, b;
    a.value = 0x55555555;
    b.value = 0x33333333;
    posit32 result = a ^ b;
    ASSERT_EQ(result.value, 0x66666666);
}

TEST(Posit32BitOperations, LeftShift)
{
    posit32 p;
    p.value = 0x12345678;
    posit32 result = p << 4;
    ASSERT_EQ(result.value, 0x23456780);
}

TEST(Posit32BitOperations, RightShift)
{
    posit32 p;
    p.value = 0x12345678;
    posit32 result = p >> 4;
    ASSERT_EQ(result.value, 0x01234567);
}

//========================================================================================
// SPECIFIC VALUES TESTS
//========================================================================================

TEST(Posit32SpecificValues, Zero)
{
    posit32 p;
    p.value = 0;
    ASSERT_EQ(p.toDouble(), 0.0);

    posit32 zero = 0.0;
    ASSERT_EQ(zero.value, 0);
}

TEST(Posit32SpecificValues, One)
{
    posit32 one = 1.0;
    ASSERT_EQ(one.toDouble(), 1.0);

    // Check specific bit pattern for 1.0
    ASSERT_EQ(one.value, 0x40000000);
}

TEST(Posit32SpecificValues, NegativeOne)
{
    posit32 neg_one = -1.0;
    ASSERT_EQ(neg_one.toDouble(), -1.0);

    // Check specific bit pattern for -1.0
    ASSERT_EQ(neg_one.value, 0xC0000000);
}

//========================================================================================
// ADVANCED TESTS
//========================================================================================

// Test that adding and then subtracting the same value returns the original
TEST(Posit32Advanced, AddSubCancel)
{
    current_operation = "Add-Sub Cancellation";
    for (int i = 0; i < NTESTS32 / 10; i++)
    { // Reduced iteration count
        posit32 p_a, p_b;
        p_a.value = int_dist32(gen);
        p_b.value = int_dist32(gen);

        posit32 sum = p_a + p_b;
        posit32 original = sum - p_b;

        ASSERT_TRUE(double_eq(original.toDouble(), p_a.toDouble(), 1e-12, 1e-1))
            << "Failed: (" << p_a.toDouble() << " + " << p_b.toDouble() << ") - " << p_b.toDouble()
            << " = " << original.toDouble() << " but expected " << p_a.toDouble();
    }
}

// Test that multiplying and then dividing by the same value returns the original
TEST(Posit32Advanced, MulDivCancel)
{
    current_operation = "Mul-Div Cancellation";
    for (int i = 0; i < NTESTS32 / 10; i++)
    { // Reduced iteration count
        posit32 p_a, p_b;
        p_a.value = int_dist32(gen);
        p_b.value = int_dist32(gen);

        // Skip division by zero or very small numbers
        if (p_b.value == 0 || p_b.isNaR() || std::abs(p_b.toDouble()) < 1e-6)
        {
            continue;
        }

        posit32 product = p_a * p_b;
        posit32 original = product / p_b;

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