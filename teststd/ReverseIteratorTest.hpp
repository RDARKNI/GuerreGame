
#ifndef REVERSEITERATORTEST
#define REVERSEITERATORTEST

export module ReverseIteratorTest;
#define size_t unsigned int
#define ptrdiff_t long

export template <typename I>
class reverse_iterator_t {
  // using iterator_type = T;
  // using iterator_concept = size_t;
  // using iterator_category = size_t;
  // using value_type = ptrdiff_t;
  // using difference_type = value_type&;
  // using pointer = const value_type&;
  // using reference = value_type*;

 public:
  constexpr explicit reverse_iterator_t(I i) noexcept : it{i} {};

  constexpr decltype(auto) operator*() const { return *(it - 1); }

  constexpr reverse_iterator_t& operator++() {
    --it;
    return *this;
  }

  constexpr reverse_iterator_t operator++(int) {
    auto tmp = *this;
    --it;
    return tmp;
  }
  constexpr reverse_iterator_t& operator--() {
    ++it;
    return *this;
  }
  constexpr reverse_iterator_t operator--(int) {
    auto tmp = *this;
    ++it;
    return tmp;
  }
  constexpr I base() const { return it; }
  constexpr reverse_iterator_t operator<=>(
      const reverse_iterator_t& other) const noexcept = default;

 protected:
  I it;
};

#endif