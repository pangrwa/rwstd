#include "Vector/vector.hpp"
#include <gtest/gtest.h>

class VectorTest : public testing::Test {
protected:
  VectorTest() {
    v1 = rwstd::Vector<int>(2);
    v2 = rwstd::Vector<int>(3, 3);
  }

  rwstd::Vector<int> v0;
  rwstd::Vector<int> v1;
  rwstd::Vector<int> v2;
};

TEST_F(VectorTest, InitialState) {
  EXPECT_EQ(v0.size(), 0);
  EXPECT_EQ(v0.capacity(), 2);

  EXPECT_EQ(v1.size(), 2);
  EXPECT_EQ(v2.size(), 3);
  for (int i = 0; i < v2.size(); ++i) {
    EXPECT_EQ(v2.at(i), 3);
  }
}

TEST_F(VectorTest, ElementAccess) {
  EXPECT_EQ(v2[0], 3);

  EXPECT_EQ(v2.front(), 3);
  EXPECT_EQ(v2.back(), 3);
}

TEST_F(VectorTest, Modifiers) {
  v0.push_back(1);
  EXPECT_EQ(v0.back(), 1);

  v0.push_back(2);
  EXPECT_EQ(v0.back(), 2);

  v0.push_back(3);
  EXPECT_EQ(v0.back(), 3);

  EXPECT_EQ(v0.size(), 3);

  v0.pop_back();
  EXPECT_EQ(v0.back(), 2);
  EXPECT_EQ(v0.size(), 2);
}
