#ifndef LOSEVA_SHARED_DS_HPP
#define LOSEVA_SHARED_DS_HPP

#include <cstddef>
#include <string>

namespace loseva {

template< typename T >
struct Array_t {
  T* data = nullptr;
  std::size_t size = 0;
  std::size_t capacity = 0;
};

template< typename T >
void init(Array_t< T >& arr)
{
  arr.data = nullptr;
  arr.size = 0;
  arr.capacity = 0;
}

template< typename T >
void destroy(Array_t< T >& arr)
{
  delete[] arr.data;
  loseva::init(arr);
}

template< typename T >
void push_back(Array_t< T >& arr, const T& val)
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

struct Person_t {
  std::size_t id = 0;
  std::string info = "";
};

struct Meet_t {
  std::size_t id1 = 0;
  std::size_t id2 = 0;
  int time = 0;
};

struct Date_t {
  int d = 0;
  int m = 0;
  int y = 0;
};

struct MeetWithDate_t {
  Meet_t meet;
  Date_t date;
};

struct Period_t {
  Date_t start;
  Date_t end;
  bool is_empty = true;
};

struct RangeStack_t {
  Period_t* data = nullptr;
  std::size_t size = 0;
  std::size_t capacity = 0;
};

void init(RangeStack_t& st)
{
  st.data = nullptr;
  st.size = 0;
  st.capacity = 0;
}

void destroy(RangeStack_t& st)
{
  delete[] st.data;
  st.data = nullptr;
  st.size = 0;
  st.capacity = 0;
}

void push(RangeStack_t& st, const Period_t& val)
{
  if (st.size == st.capacity) {
    const std::size_t new_cap = (st.capacity == 0) ? 1 : st.capacity * 2;
    Period_t* new_data = new Period_t[new_cap];
    for (std::size_t i = 0; i < st.size; ++i) {
      new_data[i] = st.data[i];
    }
    delete[] st.data;
    st.data = new_data;
    st.capacity = new_cap;
  }
  st.data[st.size++] = val;
}

void pop(RangeStack_t& st)
{
  if (st.size > 0) {
    st.size--;
  }
}

bool has_person(const Array_t< Person_t >& arr, const std::size_t id)
{
  for (std::size_t i = 0; i < arr.size; ++i) {
    if (arr.data[i].id == id) {
      return true;
    }
  }
  return false;
}

int compare_dates(const Date_t& a, const Date_t& b)
{
  if (a.y != b.y) {
    return (a.y < b.y) ? -1 : 1;
  }
  if (a.m != b.m) {
    return (a.m < b.m) ? -1 : 1;
  }
  if (a.d != b.d) {
    return (a.d < b.d) ? -1 : 1;
  }
  return 0;
}

}

#endif
