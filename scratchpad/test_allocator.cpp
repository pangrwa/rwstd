

#include "allocator.hpp"
#include <iostream>
#include <memory>

int main() {
  std::allocator<int> int_allocator;

  rwstd::Allocator<int> a;
  int *p1 = a.allocate(2);
  new (p1) int(10);
  new (p1 + 1) int(11);
  std::cout << "first: " << *p1 << " second: " << *(p1 + 1) << std::endl;

  a.deallocate(p1, 2);

  using traits = std::allocator_traits<decltype(a)>;
  p1 = traits::allocate(a, 2);
  traits::construct(a, p1, 15);
  traits::construct(a, p1 + 1, 16);

  std::cout << "first: " << *p1 << " second: " << *(p1 + 1) << std::endl;

  traits::deallocate(a, p1, 2);
}
