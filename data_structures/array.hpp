//
// Created by X-ray on 06/21/22.
//

#ifndef ARRAY_ARRAY_HPP
#define ARRAY_ARRAY_HPP
#include <cstdlib>
#include <stdexcept>
#include <iterator>

template<typename T>
class Array {
public:
  explicit Array(std::size_t capacity) {
    capacity_ = capacity;
    data_ = new T[capacity];
  }

  ~Array() {
    delete[] data_;
  }

  inline T& At(std::size_t idx) const {
    if (idx < capacity_) {
      return data_[idx];
    } else {
      throw std::out_of_range("call to At() was out of bounds");
    }
  }

  inline T& operator[](std::size_t idx) const {
    return At(idx);
  }

  inline T& Front() const {
    return data_[0];
  }

  inline T& Back() const {
    return data_[capacity_ - 1];
  }

  inline T* Data() const {
    return data_;
  }

  [[nodiscard]] inline std::size_t Capacity() const {
    return capacity_;
  }

  inline void Fill(const T& val) {
    for (auto&& elem : *this) {
      elem = val;
    }
  }

  struct Iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = T;
    using pointer           = value_type*;
    using reference         = value_type&;

    explicit Iterator(pointer data, std::size_t max, std::size_t cur = 0) : data_(data), max_(max), cur_(cur) {}

    reference operator*() const {
      OutOfRange();

      return data_[cur_];
    }

    pointer operator->() {
      OutOfRange();

      return data_[cur_];
    }

    Iterator& operator++() {
      cur_++;
      return *this;
    }

    Iterator operator++(int) {
      Iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    friend bool operator== (const Iterator& a, const Iterator& b) {
      return a.cur_ == b.max_;
    };

    friend bool operator!= (const Iterator& a, const Iterator& b) {
      return a.cur_ != b.max_;
    };

  private:
    pointer data_;
    std::size_t max_;
    std::size_t cur_;

  private:
    inline void OutOfRange() const {
      if (cur_ >= max_)
        throw std::out_of_range("iterator is out of range");
    }
  };

  struct ConstIterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = T const;
    using pointer           = value_type const*;
    using reference         = value_type const&;

    explicit ConstIterator(pointer data, std::size_t max, std::size_t cur = 0) : data_(data), max_(max), cur_(cur) {}

    reference operator*() const {
      OutOfRange();

      return data_[cur_];
    }

    pointer operator->() {
      OutOfRange();

      return data_[cur_];
    }

    ConstIterator& operator++() {
      cur_++;
      return *this;
    }

    ConstIterator operator++(int) {
      ConstIterator tmp = *this;
      ++(*this);
      return tmp;
    }

    friend bool operator== (const ConstIterator& a, const ConstIterator& b) {
      return a.cur_ == b.max_;
    };

    friend bool operator!= (const ConstIterator& a, const ConstIterator& b) {
      return a.cur_ != b.max_;
    };

  private:
    pointer data_;
    std::size_t max_;
    std::size_t cur_;

  private:
    inline void OutOfRange() const {
      if (cur_ >= max_)
        throw std::out_of_range("iterator is out of range");
    }
  };

  inline Iterator begin() {
    return Iterator(data_, capacity_);
  }

  inline ConstIterator begin() const {
    return ConstIterator(data_, capacity_);
  }

  inline ConstIterator cbegin() const {
    return ConstIterator(data_, capacity_);
  }

  inline Iterator end() {
    return Iterator(data_, capacity_);
  }

  inline ConstIterator end() const {
    return ConstIterator(data_, capacity_);
  }

  inline ConstIterator cend() const {
    return ConstIterator(data_, capacity_);
  }

private:
  std::size_t capacity_;
  T* data_;
};

#endif //ARRAY_ARRAY_HPP
