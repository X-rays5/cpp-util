//
// Created by X-ray on 10/15/2021.
//

#pragma once

#ifndef ARRAY_ARRAY_HPP
#define ARRAY_ARRAY_HPP
#include <cstdlib>

struct ArrayData {
	void* data_;
	size_t size_;
};

class ArrayException {
public:
	ArrayException(const char* what) : what_(what)
	{
	}

	const char* what() {
		return what_;
	}
private:
	const char* what_;
};

template<class T>
class Array {
public:
	Array(size_t size) {
		ArrayData array {
			malloc(size * sizeof(T)),
			size
		};
		memset(array.data_, 0, size);
		data_ = array;
	}

	~Array() {
		free(data_.data_);
	}

	size_t Size() {
		return data_.size_;
	}

	void Resize(size_t new_size) {
		data_.data_ = realloc(data_.data_, new_size * sizeof(T));
		data_.size_ = new_size;
	}

	T& operator[](const size_t index) const {
		if (index < data_.size_) {
			return *reinterpret_cast<T*>(reinterpret_cast<void*>((static_cast<char*>(data_.data_) + (index * sizeof(T)))));
		} else {
			throw ArrayException("index out of bounds");
		}
	}

private:
ArrayData data_{};
};
#endif //ARRAY_ARRAY_HPP
