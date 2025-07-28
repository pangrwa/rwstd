#pragma once

#include <iterator>

namespace rwstd {
/*
 * The goal of this class is to convert a pointer into an iterator - basically
 * just a wrapper
 */
template <typename Iterator, typename Container>
class NormalIterator {
protected:
  // let copmiler auto deduce the tag - cp20 have contiguous range - but i think
  // this method is for < cpp20
  typedef std::iterator_traits<Iterator> _traits_type;
  Iterator _iterator;

public:
  typedef typename _traits_type::difference_type difference_type;
  typedef typename _traits_type::value_type value_type;
  typedef typename _traits_type::pointer pointer;
  typedef typename _traits_type::reference reference;
  typedef typename _traits_type::iterator_category iterator_category;

public:
  NormalIterator() = default;
  explicit NormalIterator(const Iterator iterator) : _iterator{iterator} {}

  NormalIterator(const NormalIterator<Iterator, Container> &_i) = default;
  NormalIterator &
  operator=(const NormalIterator<Iterator, Container> &_i) = default;

  bool operator==(const NormalIterator<Iterator, Container> &rhs) const {
    return this->_iterator == rhs._iterator;
  }

  reference operator*() const { return *_iterator; }

  pointer operator->() const { return _iterator; }

  // ++it - pre
  NormalIterator &operator++() {
    ++_iterator;
    return *this;
  }

  // it++ - post
  NormalIterator operator++(int) {
    auto tmp = *this; // copy
    ++(*this);
    return tmp;
  }

  // --it - pre
  NormalIterator &operator--() {
    --_iterator;
    return *this;
  }

  // it-- - post
  NormalIterator operator--(int) {
    auto tmp = *this;
    --(*this);
    return tmp;
  }

  // arithmetic operators
  NormalIterator operator+(difference_type n) const {
    return NormalIterator(_iterator + n);
  }

  NormalIterator &operator+=(difference_type n) const {
    this->_iterator += n;
    return *this;
  }

  NormalIterator operator-(difference_type n) const {
    return NormalIterator(_iterator - n);
  }

  NormalIterator &operator-=(difference_type n) const {
    this->_iterator -= n;
    return *this;
  }

  difference_type operator-(const NormalIterator &other) const {
    return this->_iterator - other._iterator;
  }

  reference operator[](difference_type n) const { return _iterator[n]; }

  bool operator==(const NormalIterator &rhs) {
    return _iterator == rhs._iterator;
  }

  bool operator!=(const NormalIterator &rhs) {
    return _iterator != rhs._iterator;
  }

  bool operator<(const NormalIterator &rhs) {
    return _iterator < rhs._iterator;
  }

  bool operator<=(const NormalIterator &rhs) {
    return _iterator <= rhs._iterator;
  }

  bool operator>(const NormalIterator &rhs) {
    return _iterator > rhs._iterator;
  }

  bool operator>=(const NormalIterator &rhs) {
    return _iterator >= rhs._iterator;
  }
};
} // namespace rwstd
