#pragma once

#include <utility> // for std::move
#include <algorithm> // for copy_n
#include <iostream>
#include <cstring>

constexpr int DEFAULT_VECTOR_SIZE = 2;

namespace DataStructure {

template<typename T>
class SimpleVector {
 private:
  T *elements;
  size_t capacity{};
  size_t current{};

  void push(T data, size_t index);

 public:
  bool operator==(const SimpleVector &vec) const;
  template<class U>
  friend std::ostream &operator<<(std::ostream &os, const SimpleVector<T> &printObject);

  void push(T data) noexcept;

  void PushLeft(T data);

  void PopBack();

  T &operator[](int index);

  T operator[](int index) const;

  [[nodiscard]] size_t size() const { return current; }

  [[maybe_unused]] int getCapacity() { return capacity; }

  SimpleVector() {
    elements = new T[DEFAULT_VECTOR_SIZE];
    capacity = DEFAULT_VECTOR_SIZE;
    current = 0;
  }

  SimpleVector(const SimpleVector &other) : capacity(other.size()), current(other.current) {
    elements = new T[capacity];
    std::copy_n(other.elements, other.current, elements);
  }

  SimpleVector(SimpleVector &&other) noexcept;

  SimpleVector &operator=(const SimpleVector &other) {
    if (this == &other) return *this;

    delete[]elements;
    elements = nullptr;
    elements = new T[other.size()];
    capacity = other.current;
    current = other.current;

    std::copy_n(other.elements, current, elements);

    return *this;
  }

  SimpleVector &operator=(SimpleVector &&other) noexcept {
    if (this == &other) return *this;

    delete[]elements;
    elements = other.elements;
    capacity = other.capacity;
    current = other.current;

    // Without this we will get double free
    other.elements = nullptr;
    other.capacity = 0;
    other.current = 0;

    return *this;
  }

  ~SimpleVector() {
    delete[] elements;
    elements = nullptr;
  }

  void ExtendCapacity(T data);
};

template<typename T>
void SimpleVector<T>::ExtendCapacity(T) {
  T *extended_array = new T[2 * capacity];

  for (size_t i = 0; i < capacity; i++) {
    extended_array[i] = elements[i];
  }

  capacity *= 2;
  T *old_data = elements;
  elements = extended_array;
  delete[] old_data;
}

template<typename T>
void SimpleVector<T>::push(T data) noexcept {
  if (current == capacity) ExtendCapacity(data);

  elements[current] = data;
  current++;
}

template<typename T>
void SimpleVector<T>::push(T data, size_t index) {
  if (index == capacity)
    push(data);
  else
    elements[index] = data;
}

template<typename T>
bool SimpleVector<T>::operator==(const SimpleVector &vec) const {
  if (current != vec.current) return false;
  if (current > 0 && memcmp(elements, vec.elements, current * sizeof(T))) return false;
  return true;
}

template<typename T>
void SimpleVector<T>::PushLeft(T data) {
  if (size() == 0) {
    push(data);
    return;
  }
  if (capacity == size()) ExtendCapacity(data);
  current++;
  for (unsigned int i = size() - 1; i > 0; i--) {
    elements[i] = elements[i - 1];
  }

  elements[0] = data;
}

template<typename T>
T &SimpleVector<T>::operator[](int index) {
  if (index < 0 || index > static_cast<int>(current))
    throw std::invalid_argument("bad index : " + std::to_string(index));
  return elements[index];
}

template<typename T>
T SimpleVector<T>::operator[](int index) const {
  return elements[index];
}
template<typename T>
SimpleVector<T>::SimpleVector(SimpleVector &&other) noexcept
    :elements(other.elements), capacity(other.capacity), current(other.current) {
  other.elements = nullptr;
  other.current = 0;
}
template<typename T>
void SimpleVector<T>::PopBack() {
  elements[--current].~T();
}

template<typename T>
std::ostream &operator<<(std::ostream &os, const SimpleVector<T> &printObject) {
  for (int i = 0; i < static_cast<int>(printObject.size()); ++i) {
    os << printObject[i];
  }
  return os;
}

template<typename T = char>
std::ostream &operator<<(std::ostream &os, const SimpleVector<char> &printObject) {
  for (int i = 0; i < static_cast<int>(printObject.size()); ++i) {
    os << static_cast<int>(printObject[i]);
  }

  return os;
}

}