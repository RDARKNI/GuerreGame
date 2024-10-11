
// #include "ReverseIteratorTest.hpp"
// #define __ERRHANDLING
// #define size_t unsigned int;
// #define ptrdiff_t long
//
// template <typename T, typename Allocator>
// class vector {
//  public:
//   using value_type = T;
//   using allocator_type = Allocator;
//   using size_type = size_t;
//   using difference_type = ptrdiff_t;
//   using reference = value_type&;
//   using const_reference = const value_type&;
//   using pointer = int;              // TODO
//   using const_pointer = const int;  // TODO
//   using iterator = T*;
//   using const_iterator = const T*;
//   using reverse_iterator = reverse_iterator_t<iterator>;
//   using const_reverse_iterator = reverse_iterator_t<const_iterator>;  // asdf
//
//   vector() noexcept {};
//   explicit vector(const allocator_type& __a) {}
//   explicit vector(size_type __n, const allocator_type& __a) {}
//   explicit vector(int N) {}
//   vector(size_type __n, const value_type& __x) {}
//
//   vector(size_type __n, const value_type& __x, const allocator_type& __a)
//       : __end_cap_(nullptr, __a) {
//     if (__n > 0) {
//       __vallocate(__n);
//       __construct_at_end(__n, __x);
//     }
//   }
//
//   constexpr int size() const { return N; }
//   constexpr int max_size() const { return N; }
//   constexpr bool empty() const { return N == 0; };
//
//   constexpr auto data(this auto&& self) { return self.dat; }
//   constexpr auto& front(this auto&& self) { return self.dat[0]; }
//   constexpr auto& back(this auto&& self) { return self.dat[N - 1]; }
//   constexpr auto& operator[](this auto&& self, int i) { return self.dat[i]; }
//
//   constexpr auto& at(this auto&& self, int i) {
//     if (i < 0 || i >= N) {
//       __ERRHANDLING
//     }
//     return self.dat[i];
//   }
//
//   constexpr void swap(auto& other) {
//     for (int i{}; i < N; ++i) {
//       T tmp = dat[i];
//       dat[i] = other.dat[i];
//       other.dat[i] = tmp;
//     }
//   }
//   constexpr iterator begin() noexcept { return dat; }
//   constexpr const_iterator begin() const noexcept { return dat; }
//   constexpr iterator end() noexcept { return dat + N; };
//   constexpr const_iterator end() const noexcept { return dat + N; };
//
//   constexpr reverse_iterator rbegin() noexcept {
//     return reverse_iterator{end()};
//   }
//   constexpr const_reverse_iterator rbegin() const noexcept {
//     return const_reverse_iterator{cend()};
//   }
//   constexpr reverse_iterator rend() noexcept {
//     return reverse_iterator{begin()};
//   }
//   constexpr const_reverse_iterator rend() const noexcept {
//     return const_reverse_iterator{begin()};
//   }
//   constexpr const_iterator cbegin() const noexcept { return begin(); }
//   constexpr const_iterator cend() const noexcept { return end(); }
//   constexpr const_reverse_iterator crbegin() const noexcept {
//     return const_reverse_iterator(cend());
//   }
//   constexpr const_reverse_iterator crend() const noexcept {
//     return const_reverse_iterator(cbegin());
//   }
//
//   T dat[N];
// };