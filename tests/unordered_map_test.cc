#include "unordered_map.hpp"
#include <gtest/gtest.h>

class UnorderedMapTest : public testing::Test {
protected:
  UnorderedMapTest() {
    std::pair<int, int> x = std::make_pair(1, 3);
    v0.insert(x);
    v1.insert({"hello", "hello"});
    v1.insert({"Bye", "Bye"});
  }

  rwstd::UnorderedMap<int, int> v0;
  rwstd::UnorderedMap<std::string, std::string> v1;
};

TEST_F(UnorderedMapTest, InitialState) {
  EXPECT_EQ(v0.size(), 1);
  EXPECT_EQ(v0[1], 3);
  EXPECT_EQ((*v0.find(1)).second, 3);

  EXPECT_EQ(v1.size(), 2);
  EXPECT_EQ(v1["hello"], "hello");
  EXPECT_EQ(v1["Bye"], "Bye");

  auto it = v1.find("Bye");
  v1.erase(it);
  EXPECT_EQ(v1.size(), 1);
}
