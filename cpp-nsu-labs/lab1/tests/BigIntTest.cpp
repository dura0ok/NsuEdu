#include <bitset>
#include "gtest/gtest.h"
#include "../BigInt.h"

namespace {
using NumbersLibrary::BigInt;

TEST(BigIntTest, StringCastTest) {
  auto a = BigInt("10");
  EXPECT_EQ(static_cast<std::string>(a), "10");
}

TEST(BigIntTest, StringConstructorTest) {
  auto num = BigInt("123321");
  EXPECT_EQ(num.getSize(), 6);
}

TEST(BigIntTest, IntConstructorTest) {
  auto a = BigInt(2);
  a += BigInt(8);
  auto ans = BigInt("10");
  EXPECT_EQ(ans, a);
}

TEST(BigIntTest, BadStringConstructorTest) {
  EXPECT_THROW(BigInt(""), std::invalid_argument);
  EXPECT_THROW(BigInt("123c"), std::invalid_argument);
}

TEST(BigIntTest, CopyTest) {
  auto num = BigInt("123");
  auto copy = num;
  EXPECT_EQ(copy.getSize(), 3);
}

TEST(BigIntTest, EqualTest) {
  auto num = BigInt("1234");
  auto num2 = BigInt("321");
  num2 = num;
  EXPECT_EQ(num2.getSize(), 4);
  EXPECT_EQ(num.getSize(), 4);
  EXPECT_TRUE(BigInt() == BigInt(0));
}

TEST(BigIntTest, SelfAssignTest) {
  auto num = BigInt("1234");
  auto copy = num;
  num = num;
  EXPECT_TRUE(copy == num);
}

TEST(BigIntTest, SumTwoIntsTest) {
  auto firstNum = BigInt("99");
  auto secondNum = BigInt("1");
  auto ans = BigInt("100");
  auto res = firstNum + secondNum;
  EXPECT_TRUE(res == ans);
  EXPECT_TRUE(BigInt("99") + BigInt(1) == BigInt("100"));
}

TEST(BigIntTest, SumTestsRange) {
  for (int a = 0; a < 1000; a++) {
    for (int b = 0; b < 1000; b++) {
      auto firstNum = BigInt(std::to_string(a));
      auto secondNum = BigInt(std::to_string(b));
      auto res = firstNum + secondNum;
      EXPECT_EQ(res, BigInt(std::to_string(a + b)));
    }
  }
}

TEST(BigIntTest, SubstractTestsRange) {
  for (int a = 0; a < 1000; a++) {
    for (int b = 0; b < 1000; b++) {
      auto firstNum = BigInt(std::to_string(a));
      auto secondNum = BigInt(std::to_string(b));
      auto res = firstNum - secondNum;
      EXPECT_EQ(res, BigInt(std::to_string(a - b)));
    }
  }
}

TEST(BigIntTest, MultiplyTestsRange) {
  for (int a = 0; a < 1000; a++) {
    for (int b = 0; b < 1000; b++) {
      auto firstNum = BigInt(std::to_string(a));
      auto secondNum = BigInt(std::to_string(b));
      auto res = firstNum * secondNum;
      EXPECT_EQ(res, BigInt(std::to_string(a * b)));
    }
  }
}

TEST(BigIntTest, DivisionTestsRange) {
  for (int a = 1; a < 100; a++) {
    for (int b = 1; b < 100; b++) {
      auto firstNum = BigInt(std::to_string(a));
      auto secondNum = BigInt(std::to_string(b));
      auto res = firstNum / secondNum;
      auto ans = BigInt(std::to_string(static_cast<int>(a / b)));
      EXPECT_EQ(res, ans);
    }
  }
}

TEST(BigIntTest, SumPositiveWithBiggerNegative) {
  auto firstNum = BigInt("5");
  auto secondNum = BigInt("-100");
  auto res = firstNum + secondNum;
  auto ans = BigInt("-95");
  EXPECT_EQ(res, ans);
}

TEST(BigIntTest, OneIntNegative) {
  auto firstNum = BigInt("100");
  auto secondNum = BigInt("-15");
  auto ans = BigInt("85");
  auto res = firstNum + secondNum;
  EXPECT_EQ(res, ans);
}

TEST(BigIntTest, TwoIntNegative) {
  auto firstNum = BigInt("-100");
  auto secondNum = BigInt("-100");
  auto ans = BigInt("-200");
  auto res = firstNum + secondNum;

  EXPECT_EQ(res, ans);
}

TEST(BigIntTest, DiffSign) {
  auto firstNum = BigInt("100");
  auto secondNum = BigInt("-100");
  auto ans = BigInt("0");
  auto res = firstNum + secondNum;
  EXPECT_TRUE(res == ans);
}

TEST(BigIntTest, SubstractFirstPositiveNegativeSecond) {
  auto firstNum = BigInt("0");
  auto secondNum = BigInt("15");
  auto ans = BigInt("-15");
  auto res = firstNum - secondNum;
  EXPECT_EQ(res, ans);
}

TEST(BigIntTest, SubstractTwoPositive) {
  auto firstNum = BigInt("100");
  auto secondNum = BigInt("15");
  auto ans = BigInt("85");
  auto res = firstNum - secondNum;
  EXPECT_EQ(res, ans);
}

TEST(BigIntTest, SubstractFirstNegativeSecondPostive) {
  auto firstNum = BigInt("-15");
  auto secondNum = BigInt("15");
  auto ans = BigInt("-30");
  auto res = firstNum - secondNum;
  EXPECT_EQ(res, ans);
}

TEST(BigIntTest, SubstractNegatives) {
  auto firstNum = BigInt("-15");
  auto secondNum = BigInt("-15");
  auto ans = BigInt("0");
  auto res = firstNum - secondNum;
  EXPECT_EQ(res, ans);
}

TEST(BigIntTest, SimpleMiltiple) {
  auto firstNum = BigInt("2");
  auto secondNum = BigInt("5");
  auto ans = BigInt("10");
  auto res = firstNum * secondNum;
  EXPECT_EQ(res, ans);
}

TEST(BigIntTest, MultipleTwoNegatives) {
  auto firstNum = BigInt("-2");
  auto secondNum = BigInt("2");
  auto ans = BigInt("-4");
  auto res = firstNum * secondNum;
  EXPECT_EQ(res, ans);
}

TEST(BigIntTest, MultiplyFirstPositiveSecondNegative) {
  auto firstNum = BigInt("2");
  auto secondNum = BigInt("-2");
  auto ans = BigInt("-4");
  auto res = firstNum * secondNum;
  EXPECT_EQ(res, ans);
}

TEST(BigIntTest, MultiplyNegatives) {
  auto firstNum = BigInt("-2");
  auto secondNum = BigInt("-2");
  auto ans = BigInt("4");
  auto res = firstNum * secondNum;
  EXPECT_EQ(res, ans);
}

TEST(BigIntTest, MiltiplyWithZero) {
  auto firstNum = BigInt("0");
  auto secondNum = BigInt("-2");
  auto ans = BigInt("0");
  auto res = firstNum * secondNum;
  EXPECT_EQ(res, ans);
}

TEST(BigIntTest, MiltiplyWithZeroToZero) {
  auto firstNum = BigInt("0");
  auto secondNum = BigInt("0");
  auto ans = BigInt("0");
  auto res = firstNum * secondNum;
  EXPECT_EQ(res, ans);
}

TEST(BigIntTest, MultiplyFirstPositiveSecondNegative2) {
  auto firstNum = BigInt("111");
  auto secondNum = BigInt("-113");
  auto ans = BigInt("-12543");
  auto res = firstNum * secondNum;
  EXPECT_EQ(res, ans);
}

TEST(BigIntTest, SimplyDivision) {
  auto firstNum = BigInt("30");
  auto secondNum = BigInt("2");
  auto ans = BigInt("15");
  auto res = firstNum / secondNum;
  EXPECT_EQ(res, ans);
}

TEST(BigIntTest, SimplyDivision2) {
  auto firstNum = BigInt("100");
  auto secondNum = BigInt("3");
  auto ans1 = BigInt("33");
  auto ans2 = BigInt("1");
  auto res1 = firstNum / secondNum;
  auto res2 = firstNum % secondNum;
  EXPECT_EQ(res1, ans1);
  EXPECT_EQ(res2, ans2);
}

TEST(BigIntTest, IncrementOperator) {
  auto a = BigInt("0");
  auto x = a++;
  auto preRes = BigInt("0");
  auto res = BigInt("1");
  EXPECT_EQ(res, a);
  EXPECT_EQ(preRes, x);
}

TEST(BigIntTest, IncrementOperatorNegativeNum) {
  auto a = BigInt("-1");
  a++;
  auto res = BigInt("0");
  EXPECT_EQ(res, a);
}

TEST(BigIntTest, DecrementPostfixTest) {
  auto a = BigInt("10");
  auto x = --a;
  auto res = BigInt("9");
  EXPECT_EQ(res, a);
  EXPECT_EQ(res, x);
}

TEST(BigIntTest, DecrementOperator) {
  auto a = BigInt("0");
  auto x = a--;
  auto preRes = BigInt("0");
  auto res = BigInt("-1");
  EXPECT_EQ(res, a);
  EXPECT_EQ(preRes, x);
}

TEST(BigIntTest, DecrementOperatorNegativeNum) {
  auto a = BigInt("0");
  a--;
  auto res = BigInt("-1");
  EXPECT_EQ(res, a);
}

TEST(BigIntTest, IncrementPostfixTest) {
  auto a = BigInt("10");
  auto x = ++a;
  auto res = BigInt("11");
  EXPECT_EQ(res, a);
  EXPECT_EQ(res, x);
}

TEST(BigIntTest, DivisionNegativeToPositive) {
  auto firstNum = BigInt("-100");
  auto secondNum = BigInt("2");
  auto ans = BigInt("-50");
  auto res = firstNum / secondNum;
  EXPECT_EQ(res, ans);
}

TEST(BigIntTest, DivisionTwoPositive) {
  auto firstNum = BigInt("17");
  auto secondNum = BigInt("5");
  auto ans1 = BigInt("3");
  auto ans2 = BigInt("2");
  auto res1 = firstNum / secondNum;
  auto res2 = firstNum % secondNum;
  EXPECT_EQ(res1, ans1);
  EXPECT_EQ(res2, ans2);
}

TEST(BigIntTest, DivisionPositiveToNegative) {
  auto firstNum = BigInt("17");
  auto secondNum = BigInt("-5");
  auto ans1 = BigInt("-3");
  auto ans2 = BigInt("2");
  auto res1 = firstNum / secondNum;
  auto res2 = firstNum % secondNum;
  EXPECT_EQ(res1, ans1);
  EXPECT_EQ(res2, ans2);
}

TEST(BigIntTest, DivisionZeroTest) {
  auto firstNum = BigInt("17");
  auto zero = BigInt("0");
  EXPECT_THROW(auto res = firstNum / zero, std::invalid_argument);
}

TEST(BigIntTest, CompareNeitralAndNegativeTest) {
  auto a = BigInt("-1");
  auto b = BigInt("0");
  EXPECT_TRUE(b > a);
  EXPECT_TRUE(a < b);
  EXPECT_FALSE(a == b);
}

TEST(BigIntTest, BinTest) {
  auto a = BigInt("16");
  auto res = BigInt::bin(a);
  auto ans = BigInt("10000");
  EXPECT_EQ(ans, res);
}

TEST(BigIntTest, BinTestRange) {
  for (size_t i = 0; i < 100; ++i) {
    auto ans = BigInt(std::bitset<16>(i).to_string());
    EXPECT_EQ(BigInt::bin(BigInt(std::to_string(i))), ans);
  }
}

TEST(BigIntTest, DecTest) {
  auto a = BigInt("10000");
  auto res = BigInt("16");
  EXPECT_EQ(BigInt::dec(a), res);
}

TEST(BigIntTest, DecTestRange) {
  for (size_t i = 0; i < 100; ++i) {
    auto bin = BigInt::bin(BigInt(std::to_string(i)));
    EXPECT_EQ(BigInt::dec(bin), BigInt(std::to_string(i)));
  }
}

TEST(BigIntTest, BitwiseTest) {
  auto a = BigInt("11");
  auto b = BigInt("2");
  a &= b;
  EXPECT_EQ(a, BigInt("2"));
}

TEST(BigIntTest, BinaryComplement) {
  auto a = BigInt(10);
  auto b = ~a;
  EXPECT_EQ(b, BigInt(5));
}

}