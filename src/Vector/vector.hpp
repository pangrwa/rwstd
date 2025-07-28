#pragma once

#include "Iterator/normal_iterator.hpp"
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <format>
#include <stdexcept>

namespace rwstd {

template <typename T>
class Vector {
public:
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
  T *_data;
  size_t _size;
  size_t _capacity;

public:
  Vector() : _size{0}, _capacity{2} { _data = new T(2); }

  explicit Vector(size_t size) : _size(0), _capacity(size * 2) {
    _data = new T(_capacity);
  }

  explicit Vector(size_t size, const T &value)
      : _size(size), _capacity(size * 2) {
    _data = new T(_capacity);
    std::fill(_data, _data + _size, value);
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

  size_t capacity() const { return _capacity; }

  void reserve(size_type new_cap) {
    T *new_data = new T(new_cap);
    std::memcpy(new_data, _data, _size);
    delete[] _data;
    _data = new_data;
    _capacity = new_cap;
  }

  void shrink_to_fit() {
    T *new_data = new T(_size);
    std::memcpy(new_data, _data, _size);
    delete[] _data;
    _data = new_data;
    _capacity = _size;
  }

  /*
   * Modifiers
   */

  void clear() {
    T *new_data = new T(2);
    delete[] _data;
    _data = new_data;
    _size = 0;
    _capacity = 2;
  }

  void push_back(const T &value) {
    if (_size == _capacity) {
      _reallocate();
    }
    _data[_size] = value;
    _size++;
  }

  // https://stackoverflow.com/questions/72056842/how-can-i-implement-iterator-insert-const-iterator-position-inputiterator-firs
  iterator insert(const_iterator pos, const T &value) {
    difference_type idx = pos - this->cbegin();
    if (_size == _capacity) {
      _reallocate();
    }

    iterator modifed_pos = this->begin() + idx;
    std::copy_backward(modifed_pos, this->end(), ++this->end());
    std::fill(modifed_pos, modifed_pos + 1, value);
    _size++;
  }

  void pop_back() { _size--; }

private:
  void _reallocate() {
    T *new_data = new T(_capacity * 2);
    std::memcpy(new_data, _data, _size * sizeof(T));
    delete[] _data;
    _data = new_data;
    _capacity = _capacity * 2;
  }
};
} // namespace rwstd
