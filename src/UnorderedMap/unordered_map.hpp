#pragma once

#include <cmath>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <utility>

namespace rwstd {

template <typename Key, typename T, typename Hash = std::hash<Key>,
          typename KeyEqual = std::equal_to<Key>,
          typename Allocator = std::allocator<std::pair<const Key, T>>>
class UnorderedMap {
public:
  using key_type = Key;
  using mapped_type = T;
  using value_type = std::pair<const Key, T>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using hasher = Hash;
  using key_equal = KeyEqual;
  using allocator_type = Allocator;
  using reference = value_type &;
  using const_reference = const value_type &;

  using alloc_traits = std::allocator_traits<Allocator>;

private:
  struct Node {
    value_type value;
    Node *next;

    template <typename... Args>
    Node(Args &&...args) : value{std::forward<Args>(args)...}, next{nullptr} {}
  };

public:
  class UnorderedMapForwardIterator {
  public:
    using value_type = UnorderedMap::value_type;
    using difference_type = UnorderedMap::difference_type;
    using pointer = UnorderedMap::value_type *;
    using reference = UnorderedMap::reference;
    using iterator_category = std::forward_iterator_tag;

    Node *node;
    UnorderedMap *map;

    UnorderedMapForwardIterator() : node{nullptr}, map{nullptr} {}
    UnorderedMapForwardIterator(Node *cur_node, UnorderedMap *cur_map)
        : node{cur_node}, map{cur_map} {}

    reference operator*() const { return node->value; }
    pointer operator->() const { return &(node->value); }

    UnorderedMapForwardIterator &operator++() {
      if (node->next != nullptr) {
        node = node->next;
        return *this;
      }
      size_t idx = map->_hash_key(node->value.first);
      node = nullptr;
      for (size_t i = idx + 1; i < map->bucket_count(); ++i) {
        if (map->buckets[i] != nullptr) {
          node = map->buckets[i];
        }
      }
      return *this;
    }

    UnorderedMapForwardIterator operator++(int) {
      auto tmp = *this;
      ++(*this);
      return tmp;
    }

    bool operator==(const UnorderedMapForwardIterator &rhs) {
      return node == rhs.node;
    }

    bool operator!=(const UnorderedMapForwardIterator &rhs) {
      return node != rhs.node;
    }
  };

  using iterator = UnorderedMapForwardIterator;
  using const_iterator = const UnorderedMapForwardIterator;

private:
  using node_alloc_type =
      std::allocator_traits<Allocator>::template rebind_alloc<Node>;
  using node_alloc_traits =
      std::allocator_traits<Allocator>::template rebind_traits<Node>;

  size_t _size = 0;
  float cur_load_factor = 1.0f;

  Node **buckets;
  size_t number_of_buckets;
  key_equal _equal;
  Hash _hash;

  Allocator _value_alloc;
  node_alloc_type _node_alloc;

private:
  // initialise to nullptr
  void _init_buckets() { buckets = new Node *[number_of_buckets](); }

  size_t _hash_key(const Key &key) { return _hash(key) % number_of_buckets; }

  template <typename Forward>
  void _insert_helper(Forward &&value) {
    size_t hashed = _hash_key(value.first);
    Node *proper_bucket = buckets[hashed];

    Node *newNode = node_alloc_traits::allocate(_node_alloc, 1);
    try {
      node_alloc_traits::construct(_node_alloc, newNode,
                                   std::forward<Forward>(value));
    } catch (...) {
      node_alloc_traits::deallocate(_node_alloc, newNode, 1);
      throw;
    }

    // move the new element to be the new head of the bucket
    newNode->next = proper_bucket;
    buckets[hashed] = newNode;
    _size++;
  }

public:
  explicit UnorderedMap(size_type num_buckets, const Hash &hash = Hash(),
                        const key_equal &equal = key_equal(),
                        const Allocator &alloc = Allocator())
      : number_of_buckets{num_buckets}, _equal{equal}, _hash{hash},
        _value_alloc{alloc} {
    _init_buckets();
  }

  UnorderedMap() : UnorderedMap(11) {}

  UnorderedMap(std::initializer_list<value_type> init, size_type num_buckets,
               const Hash &hash = Hash(), const key_equal &equal = key_equal(),
               const Allocator &alloc = Allocator())
      : UnorderedMap(num_buckets, hash, equal, alloc) {
    // TODO: work on this after we implement the re-allocation algorithm
  }

  UnorderedMap(const UnorderedMap &other)
      : _value_alloc{std::allocator_traits<
            Allocator>::select_on_container_copy_construction()},
        _node_alloc{std::allocator_traits<
            node_alloc_type>::select_on_container_copy_construction()},
        number_of_buckets{other.number_of_buckets}, _equal{other._equal},
        _hash{other._hash} {
    _init_buckets();
    for (size_t i = 0; i < other.number_of_buckets; ++i) {
      Node *current_bucket = other.buckets[i];
      while (current_bucket != nullptr) {
        _insert_helper(current_bucket->value);
        current_bucket = current_bucket->next;
      }
    }
  }

  UnorderedMap(UnorderedMap &&other) noexcept
      : _value_alloc{std::move(other._value_alloc)},
        _node_alloc{std::move(other._node_alloc)},
        number_of_buckets{other.number_of_buckets},
        _equal{std::move(other._equal)}, _hash{std::move(other._hash)},
        buckets{other.buckets} {
    other.buckets = nullptr;
    other.number_of_buckets = 0;
    other._size = 0;
  }

  UnorderedMap &operator=(UnorderedMap copy) {
    swap(copy);
    return *this;
  }

  ~UnorderedMap() {
    clear();
    delete[] buckets;
  }

  void clear() noexcept {
    for (size_t i = 0; i < number_of_buckets; ++i) {
      Node *current_bucket = buckets[i];
      while (current_bucket != nullptr) {
        Node *next = current_bucket->next;
        node_alloc_traits::destroy(_node_alloc, current_bucket);
        node_alloc_traits::deallocate(_node_alloc, current_bucket, 1);

        current_bucket = next;
      }
      buckets[i] = nullptr;
    }

    _size = 0;
  }

  bool empty() const noexcept { return _size == 0; }

  size_t size() const noexcept { return _size; }

  size_t bucket_count() const { return number_of_buckets; }

  std::pair<iterator, bool> insert(const value_type &value) {
    size_t idx = _hash_key(value.first);
    Node *current_bucket = buckets[idx];
    while (current_bucket) {
      if (_equal(current_bucket->value.first, value.first)) {
        return {iterator(current_bucket), false};
      }
      current_bucket = current_bucket->next;
    }

    if (static_cast<float>(_size + 1) >
        static_cast<float>(number_of_buckets) * cur_load_factor) {
      rehash(number_of_buckets * 2);
      idx = _hash_key(value.first);
    }

    _insert_helper(value);
    Node *inserted_node = buckets[idx];
    return {iterator{inserted_node, this}, true};
  }

  std::pair<iterator, bool> insert(value_type &&value) {
    size_t idx = _hash_key(value.first);
    Node *current_bucket = buckets[idx];
    while (current_bucket) {
      if (_equal(current_bucket->value.first, value.first)) {
        return {iterator(current_bucket, this), false};
      }
      current_bucket = current_bucket->next;
    }

    if (static_cast<float>(_size + 1) >
        static_cast<float>(number_of_buckets) * cur_load_factor) {
      rehash(number_of_buckets * 2);
      idx = _hash_key(value.first);
    }

    _insert_helper(std::move(value));
    Node *inserted_node = buckets[idx];
    return {iterator{inserted_node, this}, true};
  }

  template <class... Args>
  std::pair<iterator, bool> emplace(Args &&...args) {
    Node *newNode = node_alloc_traits::allocate(_node_alloc, 1);

    try {
      node_alloc_traits::construct(_node_alloc, newNode,
                                   std::forward<Args>(args)...);
    } catch (...) {
      node_alloc_traits::deallocate(_node_alloc, newNode, 1);
      throw;
    }

    const key_type &key = newNode->value.first;
    size_t idx = _hash_key(key);
    Node *current = buckets[idx];
    while (current) {
      if (_equal(current->value.first, newNode->value.first)) {
        node_alloc_traits::destroy(_node_alloc, newNode);
        node_alloc_traits::deallocate(_node_alloc, newNode, 1);
        return {iterator(current, this), false};
      }
      current = current->next;
    }

    if (static_cast<float>(_size + 1) >
        static_cast<float>(number_of_buckets) * cur_load_factor) {
      rehash(number_of_buckets * 2);
      idx = _hash_key(key);
    }

    newNode->next = buckets[idx];
    buckets[idx] = newNode;
    _size++;
    return {iterator{newNode, this}, true};
  }

  iterator erase(iterator pos) {
    if (pos == end())
      return end();
    size_t bucket_idx = _hash_key(pos->first);
    Node **addr_of_bucket_pointer = &buckets[bucket_idx];

    while ((*addr_of_bucket_pointer) != nullptr) {
      Node *cur_node = *addr_of_bucket_pointer;
      if (_equal((*addr_of_bucket_pointer)->value.first, pos->first)) {
        *addr_of_bucket_pointer = cur_node->next;

        auto next_ele = ++iterator(cur_node, this);

        node_alloc_traits::destroy(_node_alloc, cur_node);
        node_alloc_traits::deallocate(_node_alloc, cur_node, 1);

        _size--;
        return next_ele;
      }

      addr_of_bucket_pointer = &(cur_node->next);
    }
    return end();
  }

  size_type erase(const Key &key) {
    size_t bucket_idx = _hash_key(key);
    Node **addr_of_bucket_pointer = &buckets[bucket_idx];

    while ((*addr_of_bucket_pointer) != nullptr) {
      Node *cur_node = *addr_of_bucket_pointer;
      if (_equal(cur_node->value.first, key)) {
        *addr_of_bucket_pointer = cur_node->next;

        node_alloc_traits::destroy(_node_alloc, cur_node);
        node_alloc_traits::deallocate(_node_alloc, cur_node, 1);

        _size--;
        return 1;
      }

      addr_of_bucket_pointer = &(cur_node->next);
    }
    return 0;
  }

  void swap(UnorderedMap &other) noexcept {
    using std::swap;
    swap(other.buckets, this->buckets);
    swap(other.number_of_buckets, this->number_of_buckets);
    swap(other._equal, this->_equal);
    swap(other._hash, this->_hash);
    swap(other._size, this->_size);
    swap(other.cur_load_factor, this->cur_load_factor);

    if constexpr (alloc_traits::propogate_on_container_swap::value) {
      swap(other._value_alloc, this->_value_alloc);
    }

    if constexpr (node_alloc_traits::propogate_on_container_swap::value) {
      swap(other._node_alloc, this->_value_alloc);
    }
  }

  mapped_type &operator[](const Key &key) {
    auto result = insert({key, T()});

    return result.first->second;
  }

  iterator find(const Key &key) {
    size_t bucket_idx = _hash_key(key);
    Node *current_bucket = buckets[bucket_idx];

    while (current_bucket) {
      if (!_equal((current_bucket->value).first, key)) {
        current_bucket = current_bucket->next;
        continue;
      }
      return iterator(current_bucket, this);
    }

    return end();
  }

  const_iterator find(const Key &key) const {
    size_t bucket_idx = _hash_key(key);
    Node *current_bucket = buckets[bucket_idx];

    while (current_bucket) {
      if (!_equal((current_bucket->value).first, key)) {
        current_bucket = current_bucket->next;
        continue;
      }
      return const_iterator(current_bucket);
    }

    return end();
  }

  iterator end() noexcept { return iterator(nullptr, nullptr); }

  const_iterator end() const noexcept {
    return const_iterator(nullptr, nullptr);
  }

  const_iterator cend() const noexcept {
    return const_iterator(nullptr, nullptr);
  }

  void rehash(size_t count) {
    // A better implementation would find the smallest prime greater than count
    if (count <= number_of_buckets)
      return;

    size_t min_buckets =
        (size_t)std::ceil(static_cast<float>(_size) / cur_load_factor);
    if (count < min_buckets)
      return;

    // update for hash_key to work
    size_t old_num_buckets = number_of_buckets;
    number_of_buckets = count;

    Node **new_buckets = new Node *[count]();

    for (size_t i = 0; i < old_num_buckets; i++) {
      Node *current_bucket = buckets[i];
      while (current_bucket) {
        size_t new_idx = _hash_key((current_bucket->value).first);

        Node *next_bucket = current_bucket->next;
        current_bucket->next = new_buckets[new_idx];
        new_buckets[new_idx] = current_bucket;

        current_bucket = next_bucket;
      }
    }

    delete[] buckets;

    buckets = new_buckets;
  }

  float load_factor() const {
    auto num_buckets = bucket_count();
    if (num_buckets == 0)
      return 0.0f;
    return static_cast<float>(size()) / num_buckets;
  }

  float max_load_factor() const noexcept { return cur_load_factor; }

  void max_load_factor(float ml) { cur_load_factor = ml; }

  void reserve(size_type count) {
    rehash(std::ceil(static_cast<float>(count) / cur_load_factor));
  }
};
} // namespace rwstd
