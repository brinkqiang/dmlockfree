
#ifndef __DMATOMIC_QUEUE_H_INCLUDE__
#define __DMATOMIC_QUEUE_H_INCLUDE__

#include <atomic>
#include <cassert>
#include <cstddef>
#include <stdexcept>
#include <type_traits>

template <typename T> class CDMAtomicQueue {
public:
  explicit CDMAtomicQueue(const size_t capacity)
      : capacity_(capacity),
        slots_(capacity_ < 2 ? nullptr
                             : static_cast<T *>(operator new[](
                                   sizeof(T) * (capacity_ + 2 * kPadding)))),
        head_(0), tail_(0) {
    if (capacity_ < 2) {
      throw std::invalid_argument("size < 2");
    }
    assert(alignof(CDMAtomicQueue<T>) >= kCacheLineSize);
    assert(reinterpret_cast<char *>(&tail_) -
               reinterpret_cast<char *>(&head_) >=
           static_cast<std::ptrdiff_t>(kCacheLineSize));
  }

  ~CDMAtomicQueue() {
    while (front()) {
      pop();
    }
    operator delete[](slots_);
  }

  // non-copyable and non-movable
  CDMAtomicQueue(const CDMAtomicQueue &) = delete;
  CDMAtomicQueue &operator=(const CDMAtomicQueue &) = delete;

  template <typename... Args>
  void emplace(Args &&... args) noexcept(
      std::is_nothrow_constructible<T, Args &&...>::value) {
    static_assert(std::is_constructible<T, Args &&...>::value,
                  "T must be constructible with Args&&...");
    auto const head = head_.load(std::memory_order_relaxed);
    auto nextHead = head + 1;
    if (nextHead == capacity_) {
      nextHead = 0;
    }
    while (nextHead == tail_.load(std::memory_order_acquire))
      ;
    new (&slots_[head + kPadding]) T(std::forward<Args>(args)...);
    head_.store(nextHead, std::memory_order_release);
  }

  template <typename... Args>
  bool try_emplace(Args &&... args) noexcept(
      std::is_nothrow_constructible<T, Args &&...>::value) {
    static_assert(std::is_constructible<T, Args &&...>::value,
                  "T must be constructible with Args&&...");
    auto const head = head_.load(std::memory_order_relaxed);
    auto nextHead = head + 1;
    if (nextHead == capacity_) {
      nextHead = 0;
    }
    if (nextHead == tail_.load(std::memory_order_acquire)) {
      return false;
    }
    new (&slots_[head + kPadding]) T(std::forward<Args>(args)...);
    head_.store(nextHead, std::memory_order_release);
    return true;
  }

  void push(const T &v) noexcept(std::is_nothrow_copy_constructible<T>::value) {
    static_assert(std::is_copy_constructible<T>::value,
                  "T must be copy constructible");
    emplace(v);
  }

  template <typename P, typename = typename std::enable_if<
                            std::is_constructible<T, P &&>::value>::type>
  void push(P &&v) noexcept(std::is_nothrow_constructible<T, P &&>::value) {
    emplace(std::forward<P>(v));
  }

  bool
  try_push(const T &v) noexcept(std::is_nothrow_copy_constructible<T>::value) {
    static_assert(std::is_copy_constructible<T>::value,
                  "T must be copy constructible");
    return try_emplace(v);
  }

  template <typename P, typename = typename std::enable_if<
                            std::is_constructible<T, P &&>::value>::type>
  bool try_push(P &&v) noexcept(std::is_nothrow_constructible<T, P &&>::value) {
    return try_emplace(std::forward<P>(v));
  }

  T *front() noexcept {
    auto const tail = tail_.load(std::memory_order_relaxed);
    if (head_.load(std::memory_order_acquire) == tail) {
      return nullptr;
    }
    return &slots_[tail + kPadding];
  }

  void pop() noexcept {
    static_assert(std::is_nothrow_destructible<T>::value,
                  "T must be nothrow destructible");
    auto const tail = tail_.load(std::memory_order_relaxed);
    assert(head_.load(std::memory_order_acquire) != tail);
    slots_[tail + kPadding].~T();
    auto nextTail = tail + 1;
    if (nextTail == capacity_) {
      nextTail = 0;
    }
    tail_.store(nextTail, std::memory_order_release);
  }

  size_t size() const noexcept {
    std::ptrdiff_t diff = head_.load(std::memory_order_acquire) -
                          tail_.load(std::memory_order_acquire);
    if (diff < 0) {
      diff += capacity_;
    }
    return static_cast<size_t>(diff);
  }

  bool empty() const noexcept { return size() == 0; }

  size_t capacity() const noexcept { return capacity_; }

private:
  static constexpr size_t kCacheLineSize = 128;

  // Padding to avoid false sharing between slots_ and adjacent allocations
  static constexpr size_t kPadding = (kCacheLineSize - 1) / sizeof(T) + 1;

private:
  const size_t capacity_;
  T *const slots_;

  // Align to avoid false sharing between head_ and tail_
  alignas(kCacheLineSize) std::atomic<size_t> head_;
  alignas(kCacheLineSize) std::atomic<size_t> tail_;

  // Padding to avoid adjacent allocations to share cache line with tail_
  char padding_[kCacheLineSize - sizeof(tail_)];
};

#endif // __DMATOMIC_QUEUE_H_INCLUDE__
