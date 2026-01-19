
#include "unordered_map.hpp"
#include <iostream>
#include <iterator>
#include <unordered_map>

template <typename T>
void print_typename() {
  std::cout << typeid(T).name() << std::endl;
}

int main() {
  std::unordered_map<int, int> test;
  rwstd::UnorderedMap<int, int> temp;

  print_typename<rwstd::UnorderedMap<int, int>::iterator::value_type>();

  print_typename<std::iterator_traits<
      rwstd::UnorderedMap<int, int>::iterator>::iterator_category>();

  std::cout << "size: " << temp.size() << std::endl;

  std::cout << "buckets: " << temp.bucket_count() << std::endl;
}
