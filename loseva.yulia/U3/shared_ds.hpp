#ifndef LOSEVA_SHARED_DS_HPP
#define LOSEVA_SHARED_DS_HPP

#include <cstddef>
#include <string>

namespace loseva {

template<typename T>
struct Array {
  T* data;
  std::size_t size;
  std::size_t capacity;
};

template<typename T>
void init(Array<T>& arr) {
  arr.data = nullptr;
  arr.size = 0;
  arr.capacity = 0;
}

template<typename T>
void destroy(Array<T>& arr) {
  delete[] arr.data;
  init(arr);
}

template<typename T>
void push_back(Array<T>& arr, const T& val) {
  if (arr.size == arr.capacity) {
    std::size_t new_cap = (arr.capacity == 0) ? 1 : arr.capacity * 2;
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
  std::size_t id;
  std::string info;
};

struct Meet {
  std::size_t id1;
  std::size_t id2;
  int time;
};

struct Date {
  int d;
  int m;
  int y;
};

struct MeetWithDate {
  Meet meet;
  Date date;
};

struct Period {
  Date start;
  Date end;
  bool is_empty;
};

struct RangeStack {
  Period* data;
  std::size_t size;
  std::size_t capacity;
};

void init(RangeStack& st) {
  st.data = nullptr;
  st.size = 0;
  st.capacity = 0;
}

void destroy(RangeStack& st) {
  delete[] st.data;
  st.data = nullptr;
  st.size = 0;
  st.capacity = 0;
}

void push(RangeStack& st, const Period& val) {
  if (st.size == st.capacity) {
    std::size_t new_cap = (st.capacity == 0) ? 1 : st.capacity * 2;
    Period* new_data = new Period[new_cap];
    for (std::size_t i = 0; i < st.size; ++i) {
      new_data[i] = st.data[i];
    }
    delete[] st.data;
    st.data = new_data;
    st.capacity = new_cap;
  }
  st.data[st.size++] = val;
}

void pop(RangeStack& st) {
  if (st.size > 0) {
    st.size--;
  }
}

bool has_person(const Array<Person>& arr, std::size_t id) {
  for (std::size_t i = 0; i < arr.size; ++i) {
    if (arr.data[i].id == id) {
      return true;
    }
  }
  return false;
}

int compare_dates(const Date& a, const Date& b) {
  if (a.y != b.y) return (a.y < b.y) ? -1 : 1;
  if (a.m != b.m) return (a.m < b.m) ? -1 : 1;
  if (a.d != b.d) return (a.d < b.d) ? -1 : 1;
  return 0;
}

}

#endif
