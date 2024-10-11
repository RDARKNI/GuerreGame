#define size_t unsigned int
#define ptrdiff_t long

template <class Alloc>
struct allocator_traits {
  using allocator_type = Alloc;
  using value_type = Alloc::value_type;
  using pointer = Alloc::pointer;             // todo (otherwise value_type*)
  using const_pointer = Alloc::void_pointer;  // todo
  using const_void_pointer = Alloc::void_pointer;  // todo

  using difference_type = Alloc::void_pointer;  // todo
  using size_type = Alloc::size_type;           // todo
  using propagate_on_container_copy_assignment =
      Alloc::propagate_on_container_copy_assignment;  // todo
  using propagate_on_container_move_assignment =
      Alloc::propagate_on_container_move_assignment;  // todo
  using propagate_on_container_swap =
      Alloc::propagate_on_container_swap;  // todo
  using is_always_equal =
      Alloc::is_always_equal;  // other std::is_empty<Alloc>::type // todo
  constexpr static pointer allocate(Alloc& a, size_type n) {}
  constexpr static pointer allocate(Alloc& a, size_type n,
                                    const_void_pointer hint) {}

  // static constexpr std::allocation_result<pointer, size_type>
  // allocate_at_least( Alloc& a, size_type n )

  constexpr static void deallocate(Alloc& a, pointer p, size_type n);
  template <class T, class... Args>
  constexpr static void construct(Alloc& a, T* p, Args&&... args);
  template <class T>

  constexpr static void destroy(Alloc& a, T* p);
  constexpr static size_type max_size(const Alloc& a) noexcept;
  constexpr static Alloc select_on_container_copy_construction(const Alloc& a);
};