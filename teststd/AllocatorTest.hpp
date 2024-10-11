
#define size_t unsigned int
#define ptrdiff_t long
template <typename T>
class Allocator {
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using size_type = size_t;
  using difference_type = ptrdiff_t;

  pointer allocate(size_type n) {
    // if (n > std::numeric_limits<size_type>::max() / sizeof(value_type)) {
    //   throw std::bad_alloc();
    // }
    //  Use ::operator new or other means to allocate memory
    return static_cast<pointer>(::operator new(n * sizeof(value_type)));
  }

  void deallocate(pointer p, size_type) noexcept {
    // Use ::operator delete or other means to deallocate memory
    ::operator delete(p);
  }

  void construct(pointer p, const T& val) {
    new (p) T(val);  // Placement new to construct an object
  }

  void destroy(pointer p) {
    p->~T();  // Call the destructor
  }
};