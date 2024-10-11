
#ifndef ARRAYTEST
#define ARRAYTEST
#include "ReverseIteratorTest.hpp"
#define __ERRHANDLING
#define size_t unsigned int
#define ptrdiff_t long

template <typename T, size_t N>
class Array {
 public:
  using value_type = T;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = T*;
  using const_iterator = const T*;
  using reverse_iterator = reverse_iterator_t<iterator>;
  using const_reverse_iterator = reverse_iterator_t<const_iterator>;

  constexpr size_type size() const { return N; }
  constexpr size_type max_size() const { return N; }
  constexpr bool empty() const { return N == 0; };

  constexpr void fill(value_type el) {
    for (size_type i = 0; i < size(); ++i) {
      dat[i] = el;
    }
  }
  constexpr auto data(this auto&& self) { return self.dat; }
  constexpr auto& front(this auto&& self) { return self.dat[0]; }
  constexpr auto& back(this auto&& self) { return self.dat[N - 1]; }
  constexpr auto& operator[](this auto&& self, size_type i) {
    return self.dat[i];
  }

  constexpr auto& at(this auto&& self, int i) {
    if (i < 0 || i >= N) {
      __ERRHANDLING
    }
    return self.dat[i];
  }

  constexpr void swap(Array<T, N>& other) {
    for (size_type i{}; i < N; ++i) {
      T tmp = dat[i];
      dat[i] = other.dat[i];
      other.dat[i] = tmp;
    }
  }
  constexpr iterator begin() noexcept { return dat; }
  constexpr const_iterator begin() const noexcept { return dat; }
  constexpr iterator end() noexcept { return dat + N; };
  constexpr const_iterator end() const noexcept { return dat + N; };

  constexpr reverse_iterator rbegin() noexcept {
    return reverse_iterator{end()};
  }
  constexpr const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator{cend()};
  }
  constexpr reverse_iterator rend() noexcept {
    return reverse_iterator{begin()};
  }
  constexpr const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator{begin()};
  }
  constexpr const_iterator cbegin() const noexcept { return begin(); }
  constexpr const_iterator cend() const noexcept { return end(); }
  constexpr const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(cend());
  }
  constexpr const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(cbegin());
  }

  T dat[N];
};

#endif