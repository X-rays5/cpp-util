// utility to implement a iterator like std::iterator due to deprecation of std::iterator

// example usage
/*
inline Iterator<std::forward_iterator_tag, T> begin() {
  return Iterator<std::forward_iterator_tag, T>(data_, capacity_);
}

inline ConstIterator<std::forward_iterator_tag, T> begin() const {
  return ConstIterator<std::forward_iterator_tag, T>(data_, capacity_);
}

inline ConstIterator<std::forward_iterator_tag, T> cbegin() const {
  return ConstIterator<std::forward_iterator_tag, T>(data_, capacity_);
}

inline Iterator<std::forward_iterator_tag, T> end() {
  return Iterator<std::forward_iterator_tag, T>(data_, capacity_);
}

inline ConstIterator<std::forward_iterator_tag, T> end() const {
  return ConstIterator<std::forward_iterator_tag, T>(data_, capacity_);
}

inline ConstIterator<std::forward_iterator_tag, T> cend() const {
  return ConstIterator<std::forward_iterator_tag, T>(data_, capacity_);
}
*/

#include <cstdlib>
#include <stdexcept>
#include <iterator>
#include <cstddef>

template<typename _iterator_category, typename _value_type, typename _difference_type = std::ptrdiff_t>
struct Iterator {
public:
  using iterator_category = _iterator_category;
  using difference_type   = _difference_type;
  using value_type        = _value_type;
  using pointer           = _value_type*;
  using reference         = _value_type&;

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

template<typename _iterator_category, typename _value_type, typename _difference_type = std::ptrdiff_t>
struct ConstIterator {
public:
  using iterator_category = _iterator_category;
  using difference_type   = std::ptrdiff_t;
  using value_type        = _value_type const;
  using pointer           = _value_type const*;
  using reference         = _value_type const&;

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