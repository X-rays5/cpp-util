//
// Created by X-ray on 06/18/22.
//

#ifndef HASHMAP_HASHMAP_HPP
#define HASHMAP_HASHMAP_HPP
#include <functional>
#include <type_traits>
#include <list>
#include <stdexcept>

// https://stackoverflow.com/a/51915825
template <typename T, typename = std::void_t<>>
struct is_std_hashable : std::false_type {};
template <typename T>
struct is_std_hashable<T, std::void_t<decltype(std::declval<std::hash<T>>()(std::declval<T>()))>> : std::true_type { };
template <typename T>
constexpr bool is_std_hashable_v = is_std_hashable<T>::value;

template<typename key_t, typename val_t, std::size_t initial_bucket_count = 9, std::size_t max_bucket_count = 1152921504606846975>
class HashMap {
  static_assert(is_std_hashable_v<key_t>, "key_t must support std::hash");
  static_assert(initial_bucket_count > 0, "bucket_count must be greater then 0");

public:
  struct kv_t {
    key_t key;
    val_t value;
  };

  using kv_ref_t = kv_t&;

public:
  HashMap() {
    buckets_ = InitBuckets(initial_bucket_count);
  }

  ~HashMap() {
    delete[] buckets_;
  }

  /*
   * todo add iterators
  */

  [[nodiscard]] bool Empty() const {
    return element_count_;
  }

  [[nodiscard]] std::size_t Size() const {
    return element_count_;
  }

  inline void Clear() {
    buckets_ = std::vector<bucket_t>();
    element_count_ = 0;
  }

  kv_ref_t Insert(const key_t& key, val_t val) {
    bucket_ptr_t bucket = KeyToBucket(key);

    for (auto&& kv : *bucket) {
      if (kv.key == key) {
        kv.value = val;

        element_count_ += 1;
        return kv;
      }
    }

    element_count_ += 1;
    kv_ref_t new_kv = bucket->emplace_back(kv_t{key, val});

    if (element_count_ / bucket_count_ > 1)
      Resize();

    return new_kv;
  }

  void Erase(const key_t& key) {
    bucket_ptr_t bucket = KeyToBucket(key);

    for (auto it = bucket->begin(); it != bucket->end(); ++it) {
      if (it->key == key) {
        bucket->erase(it);
        element_count_ -= 1;
        return;
      }
    }
  }

  kv_ref_t At(const key_t& key) {
    bucket_ptr_t bucket = KeyToBucket(key);

    for (auto&& kv : bucket) {
      if (kv.key == key)
        return kv;
    }

    throw std::out_of_range(key+" doesn't exist");
  }

  // should return kv
  kv_ref_t operator[](const key_t& key) {
    bucket_ptr_t bucket = KeyToBucket(key);

    for (auto&& kv : bucket) {
      if (kv.first == key)
        return kv;
    }

    throw std::invalid_argument(key+" doesn't exist");
  }

  // since only one item is allowed per key this is a duplicate of Contains(key);
  inline std::size_t Count(const key_t& key) {
    return Contains(key);
  }

  kv_ref_t Find(const key_t& key) {
    return At(key);
  }

  bool Contains(const key_t& key) {
    bucket_ptr_t bucket = KeyToBucket(key);

    for (auto&& kv : bucket) {
      if (kv.first == key)
        return true;
    }

    return false;
  }

private:
  using bucket_t = std::list<kv_t>;
  using bucket_ref_t = bucket_t&;
  using bucket_ptr_t = bucket_t*;

  std::size_t element_count_ = 0;
  std::size_t bucket_count_ = initial_bucket_count;
  bucket_ptr_t buckets_;

private:
  inline std::size_t Hash(const key_t& src) {
    std::hash<key_t> hasher;

    return hasher(src);
  }

  inline std::size_t HashToIndex(const std::size_t& hash) {
    return (hash % bucket_count_);
  }

  inline std::size_t KeyToIdx(const key_t& key) {
    return HashToIndex(Hash(key));
  }

  inline bucket_ptr_t KeyToBucket(const key_t& key) {
    return &buckets_[KeyToIdx(key)];
  }

  inline bool IsPrime(std::size_t n) {
    if(n == 2 || n == 3)
      return true;

    if(n % 2 == 0 || n % 3 == 0)
      return false;

    for(std::size_t i = 5; i * i <= n; i += 6) {
      if(n % i == 0 || n % (i + 2) == 0)
        return false;
    }

    return true;
  }

  inline std::size_t NextPrime(std::size_t n) {
    if (n % 2 == 0)
      n++;

    while (!IsPrime(n))
      n += 2;

    return n;
  }

  inline bucket_ptr_t InitBuckets(std::size_t n) {
    auto new_bucket = new bucket_t[n];
    for (int i = 0; i < n; i++)
      new_bucket[i] = bucket_t{};

    return new_bucket;
  }

  inline void Resize() {
    if (bucket_count_ >= max_bucket_count)
      return;

    std::size_t new_size = NextPrime(bucket_count_ * 2);
    bucket_ptr_t new_buckets = InitBuckets(new_size);

    for (int i = 0; i < bucket_count_; i++) {
      for (kv_ref_t kv : buckets_[i]) {
        new_buckets[Hash(kv.key) % new_size].emplace_back(kv);
      }
    }

    delete[] buckets_;
    buckets_ = new_buckets;
    bucket_count_ = new_size;
  }
};

#endif //HASHMAP_HASHMAP_HPP
