#ifndef LOSEVA_SHARED_DS_HPP
#define LOSEVA_SHARED_DS_HPP

#include <cstddef>
#include <string>

namespace loseva {

template< typename T >
struct Array {
  T* data = nullptr;
  std::size_t size = 0;
  std::size_t capacity = 0;
};

template< typename T >
void init(Array< T >& arr)
{
  arr.data = nullptr;
  arr.size = 0;
  arr.capacity = 0;
}

template< typename T >
void destroy(Array< T >& arr)
{
  delete[] arr.data;
  loseva::init(arr);
}

template< typename T >
void push_back(Array< T >& arr, const T& val)
{
  if (arr.size == arr.capacity) {
    const std::size_t new_cap = (arr.capacity == 0) ? 1 : arr.capacity * 2;
    T* new_data = new T[new_cap];
    for (std::size_t i = 0; i < arr.size; ++i) {
      new_data[i] = arr.data[i];
    }
    delete[] arr.data;
    arr.data = new_data;
    arr.capacity = new_cap;
  }
  arr.data[arr.size++] = val;
}

struct Person {
  std::size_t id = 0;
  std::string info = "";
};

bool has_person(const Array< Person >& arr, const std::size_t id)
{
  for (std::size_t i = 0; i < arr.size; ++i) {
    if (arr.data[i].id == id) {
      return true;
    }
  }
  return false;
}

}

#endif
