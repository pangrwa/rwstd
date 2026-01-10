#pragma once

#include "Allocator/allocator.hpp"
#include "Iterator/normal_iterator.hpp"
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <format>
#include <stdexcept>

namespace rwstd {

template <typename T, typename Allocator = rwstd::Allocator<T>>
class Vector {
public:
  using alloc_traits = std::allocator_traits<Allocator>;
  typedef Allocator allocator_type;
  typedef rwstd::NormalIterator<T *, Vector> iterator;
  typedef rwstd::NormalIterator<const T *, Vector> const_iterator;

  typedef T value_type;
  typedef iterator::pointer pointer;
  typedef const value_type *const_pointer;
  typedef std::size_t size_type;
  typedef iterator::difference_type difference_type;
  typedef iterator::reference reference;
  typedef const value_type &const_reference;

private:
  allocator_type _alloc;
  T *_data;
  size_t _size;
  size_t _capacity;

public:
  Vector() noexcept(noexcept(Allocator())) : Vector(Allocator()) {}

  explicit Vector(const allocator_type &a) : _alloc{a}, _size{0}, _capacity{2} {
    _data = _alloc.allocate(2);
  }

  explicit Vector(size_t size, const allocator_type &alloc = Allocator())
      : _alloc{alloc}, _size{size}, _capacity(size * 2) {
    _data = _alloc.allocate(_capacity);
    for (size_t i = 0; i < _size; ++i) {
      alloc_traits::construct(_alloc, _data + i, T{});
    }
  }

  explicit Vector(size_t size, const T &value,
                  const allocator_type &alloc = Allocator())
      : _alloc{alloc}, _size(size), _capacity(size * 2) {
    _data = _alloc.allocate(_capacity);
    for (size_t i = 0; i < _size; ++i) {
      alloc_traits::construct(_alloc, _data + i, T{value});
    }
  }

  template <std::input_iterator InputIt>
  Vector(InputIt first, InputIt last) : Vector() {
    this->insert(this->cbegin(), first, last);
  }

  Vector(std::initializer_list<T> init) : Vector() {
    this->insert(this->cbegin(), init);
  }

  Vector(const Vector &other) : Vector(other._size, other._alloc) {
    std::copy(other._data, other._data + other._size, _data);
  }

  Vector(Vector &&other) : Vector(other._size, other._alloc) {
    _data = other._data;
    other._data = nullptr;
    other._size = 0;
  }

  Vector &operator=(const Vector &other) {
    _size = other._size;
    _capacity = other._capacity;
    _alloc = other._alloc;
    _data = _alloc.allocate(_capacity);
    std::copy(other._data, other._data + other._size, _data);
    return *this;
  }

  Vector &operator=(Vector &&other) noexcept {
    _size = other._size;
    _capacity = other._capacity;
    _alloc = other._alloc;
    _data = other._data;
    other._data = nullptr;
    other._size = 0;
    return *this;
  }

  Vector &operator=(std::initializer_list<T> init) {
    this->insert(this->cbegin(), init);
    return this;
  }
  /*
   * Element access
   */

  T &at(size_t pos) {
    if (pos >= _size) {
      throw std::out_of_range(
          std::format("vector::range_check pos: {} >= size(): {}", pos, _size));
    }
    return _data[pos];
  }

  const T &at(size_t pos) const {
    if (pos >= _size) {
      throw std::out_of_range(
          std::format("vector::range_check pos: {} >= size(): {}", pos, _size));
    }
    return _data[pos];
  }

  T &operator[](size_t pos) { return _data[pos]; }
  const T &operator[](size_t pos) const { return _data[pos]; }

  T &front() { return (*this)[0]; }
  const T &front() const { return (*this)[0]; }

  T &back() { return (*this)[_size - 1]; }
  const T &back() const { return (*this)[_size - 1]; }

  T *data() { return _data; }
  const T *data() const { return _data; }

  /*
   * Iterators
   */

  iterator begin() { return iterator(_data); }

  iterator end() { return iterator(_data + _size); }

  const_iterator cbegin() const noexcept { return const_iterator(_data); }

  const_iterator cend() const noexcept { return const_iterator(_data + _size); }

  /*
   * capacity
   */
  bool empty() const { return _size == 0; }

  size_t size() const { return _size; }

  size_t max_size() const { return alloc_traits::max_size(_alloc); }

  size_t capacity() const { return _capacity; }

  void reserve(size_type new_cap) {
    if (new_cap <= _capacity)
      return;
    if (new_cap >= max_size())
      throw std::length_error(
          std::format("{}: Size is too big for Vector", new_cap));

    T *new_data = _alloc.allocate(new_cap);

    size_t i = 0;
    try {
      for (; i < _size; ++i) {
        alloc_traits::construct(_alloc, new_data + i,
                                std::move_if_noexcept(*(_data + i)));
      }
    } catch (...) {
      for (size_t j = 0; j < i; ++j) {
        alloc_traits::destroy(_alloc, new_data + j);
      }
      alloc_traits::deallocate(_alloc, new_data, new_cap);
      throw;
    }

    for (i = 0; i < _size; ++i) {
      alloc_traits::destroy(_alloc, _data + i);
    }

    alloc_traits::deallocate(_alloc, _data, _capacity);

    _data = new_data;
    _capacity = new_cap;
  }

  // at all times capcity must be at least size 2 - write test case for this
  void shrink_to_fit() {
    if (_size == _capacity)
      return;
    if (_size == 0) {
      if (_data) {
        alloc_traits::deallocate(_alloc, _data, _capacity);
        _data = alloc_traits::allocate(_alloc, 2);
        _capacity = 2;
      }
      return;
    }

    T *new_data = _alloc.allocate(_size);
    size_t i = 0;
    try {
      for (; i < _size; ++i) {
        alloc_traits::construct(_alloc, new_data,
                                std::move_if_noexcept(*(_data + i)));
      }
    } catch (...) {
      for (size_t j = 0; j < i; ++j) {
        alloc_traits::destroy(_alloc, new_data + j);
      }
      alloc_traits::deallocate(_alloc, new_data, _size);
      throw;
    }

    for (i = 0; i < _size; ++i) {
      alloc_traits::destroy(_alloc, _data + i);
    }

    alloc_traits::deallocate(_alloc, _data, _size);

    _data = new_data;
    _capacity = _size;
  }

  /*
   * Modifiers
   */

  void clear() {
    T *new_data = alloc_traits::allocate(_alloc, 2);
    for (size_t i = 0; i < _size; ++i) {
      alloc_traits::destroy(_alloc, _data + i);
    }
    alloc_traits::deallocate(_alloc, _data, _size);

    _data = new_data;
    _size = 0;
    _capacity = 2;
  }

  void push_back(const T &value) {
    if (_size == _capacity) {
      reserve(_capacity * 2);
    }
    alloc_traits::construct(_alloc, _data + _size, value);
    _size++;
  }

  iterator insert(const_iterator pos, const T &value) {
    difference_type idx = pos - this->cbegin();
    if (_size == _capacity) {
      reserve(_capacity * 2);
    }

    alloc_traits::construct(_alloc, _data + _size,
                            std::move_if_noexcept(*(_data + _size - 1)));

    iterator modified_pos = this->begin() + idx;
    std::move_backward(modified_pos, this->end() - 1, this->end());
    *modified_pos = value;

    _size++;
    return modified_pos;
  }

  iterator insert(const_iterator pos, T &&value) {
    difference_type idx = pos - this->cbegin();
    if (_size == _capacity) {
      reserve(_capacity * 2);
    }

    alloc_traits::construct(_alloc, _data + _size,
                            std::move_if_noexcept(*(_data + _size - 1)));

    iterator modified_pos = this->begin() + idx;
    std::move_backward(modified_pos, this->end() - 1, this->end());
    *modified_pos = std::move_if_noexcept(value);

    _size++;
    return modified_pos;
  }

  iterator insert(const_iterator pos, size_type count, const T &value) {
    difference_type idx = pos - this->cbegin();
    while (_size + (size_t)count > _capacity) {
      reserve(_capacity * 2);
    }

    for (size_t i = 0; i < count; ++i) {
      alloc_traits::construct(
          _alloc, _data + _size + i,
          std::move_if_noexcept(*(_data + _size - count + i)));
    }

    iterator modified_pos = this->begin() + idx;
    std::move_backward(modified_pos, this->end() - (difference_type)count,
                       this->end() - (difference_type)count + 1);

    std::fill(modified_pos, modified_pos + (difference_type)count, value);

    _size += count;
    return modified_pos;
  }

  template <std::input_iterator InputIt>
  iterator insert(const_iterator pos, InputIt first, InputIt last) {
    difference_type idx = pos - this->cbegin();
    difference_type count = last - first;
    while (_size + (size_t)count > _capacity) {
      reserve(_capacity * 2);
    }

    for (size_t i = 0; i < (size_t)count; ++i) {
      alloc_traits::construct(
          _alloc, _data + _size + i,
          std::move_if_noexcept(*(_data + _size - count + i)));
    }

    iterator modified_pos = this->begin() + idx;
    std::move_backward(modified_pos, this->end() - (difference_type)count,
                       this->end() - (difference_type)count + 1);

    iterator end_point = this->end() + count;
    for (iterator start_point = modified_pos;
         start_point != end_point && first != last; ++first) {
      *start_point = *first;
      start_point++;
    }

    _size += (size_t)count;
    return modified_pos;
  }

  iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
    return insert(pos, ilist.begin(), ilist.end());
  }

  template <class... Args>
  iterator emplace(const_iterator pos, Args &&...args) {
    difference_type idx = pos - this->cbegin();
    if (_size == _capacity) {
      reserve(_capacity * 2);
    }

    alloc_traits::construct(_alloc, _data + _size,
                            std::move_if_noexcept(*(_data + _size + 1)));

    iterator modified_pos = this->begin() + idx;
    std::move_backward(modified_pos, this->end() - 1, this->end());

    // pos should be pointing to a valid value in the vector, i dont think
    // emplaceconstructible is actually required
    T new_value = T{std::forward<Args>(args)...};
    *modified_pos = std::move(new_value);
    _size++;
    return modified_pos;
  }

  template <class... Args>
  void emplace_back(Args &&...args) {
    if (_size == _capacity) {
      reserve(_capacity * 2);
    }

    T new_obj = T{std::forward<Args>(args)...};
    alloc_traits::construct(_alloc, _data + _size,
                            std::move_if_noexcept(new_obj));
    _size++;
  }

  void pop_back() {
    if (_size == 0)
      return;
    alloc_traits::destroy(_alloc, _data + _size - 1);
    --_size;
  }
};
} // namespace rwstd
