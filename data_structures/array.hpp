//
// Created by X-ray on 06/21/22.
//

#ifndef ARRAY_ARRAY_HPP
#define ARRAY_ARRAY_HPP
#include <cstdlib>
#include <stdexcept>

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
    for (int i = 0; i < capacity_; i++) {
      data_[i] = val;
    }
  }

private:
  std::size_t capacity_;
  T* data_;
};

#endif //ARRAY_ARRAY_HPP
