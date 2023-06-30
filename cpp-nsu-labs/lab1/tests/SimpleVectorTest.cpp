#include "gtest/gtest.h"
#include "../SimpleVector.h"

namespace {
using DataStructure::SimpleVector;
TEST(SimpleVectorTest, PushTest) {
  auto testingObject = SimpleVector<int>();
  constexpr size_t pushesCount = 1000;
  for (size_t i = 0; i < pushesCount; ++i) {
    testingObject.push(static_cast<int>(i));
  }
  EXPECT_EQ(testingObject.size(), pushesCount);
}

TEST(SimpleVectorTest, GetTest) {
  auto testingObject = SimpleVector<int>();
  constexpr size_t pushesCount = 500;
  try {
    for (size_t i = 0; i < pushesCount; ++i) {
      testingObject.push(static_cast<int>(i));
      EXPECT_EQ(testingObject[i], i);
    }
  } catch (const std::exception &e) {
    FAIL();
  }
}

TEST(SimpleVectorTest, GetBadIndexTest) {
  auto testingObject = SimpleVector<int>();
  EXPECT_THROW(testingObject[-1], std::invalid_argument);
}

TEST(SimpleVectorTest, CopyOperatorTest) {
  auto testingObject = SimpleVector<int>();
  testingObject.push(1);
  auto copy = testingObject;
  copy.push(2);
  EXPECT_EQ(copy.size(), 2);
  EXPECT_EQ(copy[1], 2);
}

TEST(SimpleVectorTest, EqualsOperatorTest) {
  auto testingObject = SimpleVector<int>();
  testingObject.push(1);
  auto testingObject2 = SimpleVector<int>();
  testingObject2 = testingObject;
  testingObject2.push(3);
  EXPECT_EQ(testingObject2.size(), 2);
  EXPECT_EQ(testingObject.size(), 1);
}

TEST(SimpleVectorTest, SelfEqualsTest) {
  auto testingObject = SimpleVector<int>();
  testingObject.push(1);
  auto copy = testingObject;
  testingObject = testingObject;
  EXPECT_TRUE(testingObject == copy);
}

TEST(SimpleVectorTest, AddLeftTest) {
  auto testingObject = SimpleVector<int>();
  testingObject.push(1);
  testingObject.PushLeft(2);
  EXPECT_EQ(testingObject[0], 2);
  EXPECT_EQ(testingObject[1], 1);

  auto EmptyVector = SimpleVector<int>();
  EmptyVector.PushLeft(10);
  EXPECT_EQ(EmptyVector[0], 10);

  auto TestingSecondObject = SimpleVector<int>();
  TestingSecondObject.push(1);
  TestingSecondObject.push(2);
  TestingSecondObject.PushLeft(3);
  EXPECT_EQ(TestingSecondObject[0], 3);
  EXPECT_EQ(TestingSecondObject[1], 1);
  EXPECT_EQ(TestingSecondObject[2], 2);

}

}
