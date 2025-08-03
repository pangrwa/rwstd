
#include "Vector/vector.hpp"
#include <algorithm>
#include <iostream>
#include <vector>

#include <cxxabi.h>
#include <typeinfo>

template <typename T>
std::string print_type() {
  int status;
  char *demangled =
      abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status);

  std::string res = status == 0 ? demangled : typeid(T).name();
  free(demangled);
  return res;
}

int main() {
  rwstd::Vector<int> test{5};
  for (int i = 0; i < 5; ++i) {
    test.push_back(5 - i);
  }
  // To check what category did the compiler actually assign
  using V = rwstd::Vector<int>;
  using Iter = rwstd::Vector<int>::iterator;
  using Cat = std::iterator_traits<Iter>::iterator_category;
  // random_access_iterator_tag - contiguous would be more explicit but I don't
  // think that was implemented by the copmiler
  std::cout << "demangled typename: " << print_type<Cat>() << std::endl;
  std::cout << "demangled reference: " << print_type<V::reference>()
            << std::endl;
  std::cout << "demangled const reference: " << print_type<V::const_reference>()
            << std::endl;

  std::cout << "Before sort" << std::endl;
  for (const auto &x : test) {
    std::cout << x << " ";
  }
  std::cout << std::endl;

  std::sort(test.begin(), test.end());

  std::cout << "after sort: " << std::endl;

  for (const auto &x : test) {
    std::cout << x << " ";
  }
  std::cout << "size: " << test.size() << std::endl;

  test.insert(test.cbegin() + 2, 20);
  for (const auto &x : test) {
    std::cout << x << " ";
  }
  std::cout << std::endl;

  [[maybe_unused]] rwstd::Vector<int> copy = test;
  std::cout << "copy: " << std::endl;

  for (const auto &y : test) {
    std::cout << y << std::endl;
  }

  [[maybe_unused]] rwstd::Vector<int> move = std::move(copy);

  std::cout << "*******************" << std::endl;

  rwstd::Vector<std::string> move_two = {"a", "b", "c"};
  move_two.emplace(move_two.cbegin(), "3");
  move_two.emplace_back("50");
  for (size_t i = 0; i < move_two.size(); ++i) {
    std::cout << move_two[i] << " ";
  }

  std::cout << std::endl;
}
