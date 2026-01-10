#include <cstddef>
#include <cstdlib>
#include <limits>
#include <new>

namespace rwstd {

template <typename T>
class Allocator {
public:
  typedef T value_type;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

  constexpr Allocator() noexcept = default;
  constexpr Allocator(const Allocator &other) noexcept = default;
  template <class U>
  constexpr Allocator(const Allocator<U> &_) noexcept {};

  constexpr T *allocate(size_type n) {
    if (std::numeric_limits<size_t>::max() / sizeof(T) < n) {
      throw std::bad_array_new_length();
    }
    T *ptr = static_cast<T *>(::operator new(n * sizeof(T)));

    return ptr;
  }

  void deallocate(T *p, size_type /*_*/) { ::operator delete(p); }
};

template <class T1, class T2>
constexpr bool operator==(const Allocator<T1> &_,
                          const Allocator<T2> & /*_*/) noexcept {
  return true;
}

template <class T1, class T2>
constexpr bool operator!=(const Allocator<T1> &_,
                          const Allocator<T2> & /*_*/) noexcept {
  return false;
}
} // namespace rwstd
