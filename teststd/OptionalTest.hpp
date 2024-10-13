
#ifndef ARRAYTEST
#define ARRAYTEST
#include <cwchar>
#include <optional>

#define __ERRHANDLING
#define size_t unsigned int
#define ptrdiff_t long

struct nullopt_t {
  struct __secret_tag {
    explicit __secret_tag() = default;
  };
  constexpr explicit nullopt_t(__secret_tag, __secret_tag) noexcept {}
};

/*
If the value has a defined sentinel type OR if we pass a sentinel value, create a myoptional



*/

template <typename T, T sentinel>
class MyOptional {
  using value_type = T;
  // using iterator = T*;
  // using const_iterator = const T*;

  constexpr MyOptional() noexcept : val{T::sentinelvalue} {}
  constexpr MyOptional(nullopt_t) noexcept : val{T::sentinelvalue} {}
  constexpr MyOptional(const MyOptional&) = default;
  constexpr MyOptional(MyOptional&&) = default;

  template <class... Args>
  constexpr explicit MyOptional(std::in_place_t, Args&&... args);
  template <class U, class... Args>
  constexpr explicit MyOptional(std::in_place_t, std::initializer_list<U> ilist, Args&&... args);
  template <class U>
  constexpr MyOptional(const MyOptional<U, U::sentinel_value>& other);
  template <class U>
  constexpr MyOptional(MyOptional<U, U::sentinel_value>&& other);
  template <class U = T>
  constexpr MyOptional(U&& value);

  constexpr MyOptional& operator=(std::nullopt_t) noexcept { val = T::sentinel_value; }

  constexpr MyOptional& operator=(const MyOptional& other) = default;

  constexpr MyOptional& operator=(MyOptional&& other) = default;

  template <class U = T>
  constexpr MyOptional& operator=(U&& value);

  template <class U>
  constexpr MyOptional& operator=(const MyOptional<U>& other);

  template <class U>
  constexpr MyOptional& operator=(MyOptional<U>&& other);

  constexpr bool has_value() const { return val != T::sentinel_value; }

  constexpr explicit operator bool() const { return has_value(); }

  constexpr const T* operator->() const noexcept { return &val; }

  constexpr T* operator->() noexcept { return &val; }

  constexpr auto&& operator*(this auto&& self) noexcept { return std::forward<decltype(self)>(self).val; }

  constexpr auto&& value(this auto&& self) {
    if (self.has_value()) { return *self; }
    std::__throw_bad_optional_access();
  }

  constexpr void reset() noexcept { val.~T(), new (&val) T::sentinel_value; }

  void swap(MyOptional& other) noexcept {
    if (has_value() && other.has_value()) { return std::swap(**this, *other); }
    if (has_value()) { new (&other.val) T(std::move(val)), reset(); }
    if (other.has_value()) { new (&val) T(std::move(other.val)), other.reset(); }
  }
  template <class... Args>
  constexpr T& emplace(Args&&... args) {
    val.~T(), new (&val) T(std::forward<Args>(args)...);
  }
  template <class U, class... Args>
  constexpr T& emplace(std::initializer_list<U> ilist, Args&&... args) {
    val.~T(), new (&val) T(ilist, std::forward<Args>(args)...);
  }

  template <class U>
  constexpr auto&& value_or(this auto&& self, U&& default_value) {
    return self.has_value() ? *self : static_cast<T>(std::forward<U>(default_value));
  }
  T val;
};
#endif