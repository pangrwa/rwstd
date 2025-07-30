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
  Vector() : _size{0}, _capacity{2} { _data = new T[2]; }

  explicit Vector(size_t size) : _size{size}, _capacity(size * 2) {
    _data = new T[_capacity];
    std::fill(_data, _data + _size, T{});
  }

  explicit Vector(size_t size, const T &value)
      : _size(size), _capacity(size * 2) {
    _data = new T[_capacity];
    std::fill(_data, _data + _size, value);
  }

  template <std::input_iterator InputIt>
  Vector(InputIt first, InputIt last) : Vector() {
    this->insert(this->cbegin(), first, last);
  }

  Vector(std::initializer_list<T> init) : Vector() {
    this->insert(this->cbegin(), init);
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
    T *new_data = new T[new_cap];
    std::memcpy(new_data, _data, _size);
    delete[] _data;
    _data = new_data;
    _capacity = new_cap;
  }

  // at all times capcity must be at least size 2 - write test case for this
  void shrink_to_fit() {
    size_t target_size = std::max(_size, (size_t)2);
    T *new_data = new T[target_size];
    std::memcpy(new_data, _data, _size);
    delete[] _data;
    _data = new_data;
    _capacity = target_size;
  }

  /*
   * Modifiers
   */

  void clear() {
    T *new_data = new T[2];
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

  iterator insert(const_iterator pos, const T &value) {
    difference_type idx = pos - this->cbegin();
    if (_size == _capacity) {
      _reallocate();
    }

    iterator modified_pos = this->begin() + idx;
    std::copy_backward(modified_pos, this->end(), ++this->end());
    std::fill(modified_pos, modified_pos + 1, value);
    _size++;

    return modified_pos;
  }

  iterator insert(const_iterator pos, T &&value) {
    difference_type idx = pos - this->cbegin();
    if (_size == _capacity) {
      _reallocate();
    }

    iterator modified_pos = this->begin() + idx;
    std::copy_backward(modified_pos, this->end(), ++this->end());
    *modified_pos = std::move(value);
    _size++;
    return modified_pos;
  }

  iterator insert(const_iterator pos, size_type count, const T &value) {
    difference_type idx = pos - this->cbegin();
    while (_size + (size_t)count > _capacity) {
      _reallocate();
    }

    iterator modified_pos = this->begin() + idx;
    std::copy_backward(modified_pos, this->end(),
                       this->end() + (difference_type)count);
    std::fill(modified_pos, modified_pos + (difference_type)count, value);

    _size += count;
    return modified_pos;
  }

  template <std::input_iterator InputIt>
  iterator insert(const_iterator pos, InputIt first, InputIt last) {
    difference_type idx = pos - this->cbegin();
    difference_type count = last - first;
    while (_size + (size_t)count > _capacity) {
      _reallocate();
    }

    iterator modified_pos = this->begin() + idx;
    iterator end_point = this->end() + count;
    std::copy_backward(modified_pos, this->end(), this->end() + count);
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

  void pop_back() { _size--; }

private:
  void _reallocate() {
    T *new_data = new T[_capacity * 2];
    std::memcpy(new_data, _data, _size * sizeof(T));
    delete[] _data;
    _data = new_data;
    _capacity = _capacity * 2;
  }
};
} // namespace rwstd
