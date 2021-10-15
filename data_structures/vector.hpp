//
// Created by X-ray on 10/15/2021.
//

#pragma once

#ifndef VECTOR_VECTOR_HPP
#define VECTOR_VECTOR_HPP
#include <cstdlib>

struct VectorData {
	void* data_;
	size_t size_;
	size_t current_elements = 0;
};

class VectorException {
public:
	VectorException(const char* what) : what_(what)
	{
	}

	const char* what() {
		return what_;
	}
private:
	const char* what_;
};

template<class T, size_t increase_size_by = 50>
class Vector {
public:
	Vector() {
		VectorData array {
			malloc(increase_size_by * sizeof(T)),
			increase_size_by
		};
		memset(array.data_, 0, increase_size_by * sizeof(T));
		data_ = array;
	}

	~Vector() {
		free(data_.data_);
	}

	inline void Emplace(T& val) {
		CheckSize();
		memmove(GetAtOffset(data_.data_, sizeof(T)), data_.data_, (data_.size_ - 1) * sizeof(T));
		At(0) = val;
		data_.current_elements += 1;
	}

	inline void Push(T val) {
		Emplace(val);
	}

	inline void EmplaceBack(T& val) {
		CheckSize();
		T at = At(data_.current_elements);
		At(data_.current_elements) = val;
		data_.current_elements += 1;
	}

	inline void PushBack(T val) {
		EmplaceBack(val);
	}

	inline void Pop() {
		memmove(data_.data_, GetAtOffset(data_.data_, sizeof(T)), (data_.size_ - 1) * sizeof(T));
		data_.current_elements--;
	}

	inline void PopBack() {
		memset(GetAtOffset(data_.data_, (data_.current_elements - 1) * sizeof(T)), 0, sizeof(T));
		data_.current_elements--;
	}

	// base = offset from ptr to start deleting
	// data = offset from ptr to move to base ptr
	// move_size = amount of bytes to move
	inline void Erase(size_t from, size_t count = 1) {
		size_t base = from * sizeof(T);
		size_t data = base + (count * sizeof(T));
		size_t move_size = (data_.size_ - count) * sizeof(T);

		memmove(GetAtOffset(data_.data_, base), GetAtOffset(data_.data_, data), move_size);
		data_.current_elements -= count;
	}

	inline void Clear() {
		memset(data_.data_, 0, data_.size_ * sizeof(T));
		data_.current_elements = 0;
	}

	inline void ShrinkToFit() {
		Resize(data_.current_elements);
	}

	[[nodiscard]] inline bool Empty() {
		return data_.current_elements == 0;
	}

	[[nodiscard]] size_t Size() {
		return data_.current_elements;
	}

	void Resize(size_t new_size) {
		data_.data_ = realloc(data_.data_, new_size * sizeof(T));
		data_.size_ = new_size;
	}

	void* Data() {
		return data_.data_;
	}

	T& At(size_t index) {
		if (index < data_.size_) {
			T* return_val = reinterpret_cast<T*>(GetAtOffset(data_.data_, index * sizeof(T)));
			return *return_val;
		} else {
			throw VectorException("index out of bounds");
		}
	}

	T& operator[](size_t index) {
		return At(index);
	}

private:
	VectorData data_{};

private:
	inline void* GetAtOffset(void* ptr, size_t offset) {
		return reinterpret_cast<void*>(static_cast<char*>(ptr) + offset);
	}

	inline void CheckSize() {
		if (data_.current_elements == data_.size_ -1) {
			data_.size_ += increase_size_by;
			Resize(data_.size_);
		}
	}
};
#endif //VECTOR_VECTOR_HPP

// example
/*
template<class T>
void PrintVec(Vector<T>& vec) {
	for (int i = 0; i < vec.Size(); i++) {
		std::cout << i << ": " <<  vec[i] << "\n";
	}
	std::cout << std::endl;
}

int main() {
	{
		try {
			Vector<const char*> vec;
			vec.PushBack("oh");
			vec.Push("beep");

			PrintVec(vec);

			vec.Push("boop");
			vec.PushBack("bap");

			vec.Pop();
			vec.PopBack();

			PrintVec(vec);

			vec.PushBack("oh");

			vec.Erase(0);
			PrintVec(vec);
			std::cout << "empty: " << vec.Empty() << "\n";
			vec.Clear();
			std::cout << "empty: " << vec.Empty() << "\n";

			vec.ShrinkToFit();
		} catch(VectorException &e) {
			std::cout << e.what() << "\n";
		}
	}
	return 0;
}
 */