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
  for (size_t i = 0; i < v2.size(); ++i) {
    EXPECT_EQ(v2.at(i), 3);
  }
}

TEST_F(VectorTest, ElementAccess) {
  EXPECT_EQ(v2[0], 3);

  EXPECT_EQ(v2.front(), 3);
  EXPECT_EQ(v2.back(), 3);

  EXPECT_EQ(v2[1], 3);
  EXPECT_EQ(v2.at(1), 3);
}

// check empty vectors iterators
TEST_F(VectorTest, Iterators) {
  v2.back() = 5;
  EXPECT_EQ(*(v2.begin()), 3);

  EXPECT_EQ(*(v2.end() - 1), 5);
}

TEST_F(VectorTest, Capacity) {
  rwstd::Vector<int> x = {1};
  x.shrink_to_fit();
  EXPECT_EQ(x.size(), 1);
  EXPECT_EQ(x.capacity(), 1);

  v2.shrink_to_fit();
  EXPECT_EQ(v2.size(), 3);
  EXPECT_EQ(v2.capacity(), 3);

  v1.reserve(100);
  EXPECT_EQ(v1.size(), 2);
  EXPECT_EQ(v1.capacity(), 100);
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

TEST_F(VectorTest, InsertModifier) {
  rwstd::Vector<int> temp = {1, 2, 3, 4, 5};
  auto x = temp.insert(temp.cbegin() + 2, 10);
  EXPECT_EQ(*x, 10);
  EXPECT_EQ(temp.size(), 6);

  // come out with some clever way to test whether or not the value is actually
  // moved

  auto first_ele_inserted_itr = temp.insert(temp.cend(), 3, 20);
  EXPECT_EQ(temp.size(), 9);
  for (size_t i = temp.size() - 1; i >= temp.size() - 3; i--) {
    EXPECT_EQ(temp[i], 20);
  }
  EXPECT_EQ(first_ele_inserted_itr, temp.end() - 3);

  rwstd::Vector<int> inserted_array = {30, 30, 30};
  first_ele_inserted_itr =
      temp.insert(temp.cend(), inserted_array.cbegin(), inserted_array.cend());

  EXPECT_EQ(temp.size(), 12);
  for (size_t i = temp.size() - 1; i >= temp.size() - 3; i--) {
    EXPECT_EQ(temp[i], 30);
  }
  EXPECT_EQ(first_ele_inserted_itr, temp.end() - 3);

  first_ele_inserted_itr = temp.insert(temp.cbegin(), {40, 40, 40, 40});
  EXPECT_EQ(temp.size(), 16);
  for (size_t i = 0; i < 4; ++i) {
    EXPECT_EQ(temp.at(i), 40);
  }
  EXPECT_EQ(first_ele_inserted_itr, temp.begin());

  rwstd::Vector<int> nothing;
  first_ele_inserted_itr =
      temp.insert(temp.cend(), nothing.cbegin(), nothing.cend());
  EXPECT_EQ(temp.end(), first_ele_inserted_itr);
}
