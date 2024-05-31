#include <atomic>
#include <memory>
#include <iostream>
#include <thread>
#include <vector>

template <typename T>
class CAtomicQueue {
public:
    CAtomicQueue(size_t capacity) : capacity(capacity), size(0) {
        buffer = new std::atomic<T*>[capacity];
        for (size_t i = 0; i < capacity; ++i) {
            buffer[i].store(nullptr);
        }
        head.store(0);
        tail.store(0);
    }

    ~CAtomicQueue() {
        delete[] buffer;
    }

    bool try_push(const T& value) {
        size_t current_tail = tail.load(std::memory_order_relaxed);
        size_t next_tail = (current_tail + 1) % capacity;
        if (next_tail == head.load(std::memory_order_acquire)) {
            return false; // queue is full
        }
        buffer[current_tail].store(new T(value), std::memory_order_release);
        tail.store(next_tail, std::memory_order_release);
        ++size;
        return true;
    }

    bool pop() {
        size_t current_head = head.load(std::memory_order_relaxed);
        if (current_head == tail.load(std::memory_order_acquire)) {
            return false; // queue is empty
        }
        T* value = buffer[current_head].exchange(nullptr, std::memory_order_acquire);
        if (value == nullptr) {
            return false; // queue is empty
        }
        head.store((current_head + 1) % capacity, std::memory_order_release);
        delete value;
        --size;
        return true;
    }

    T* front() {
        size_t current_head = head.load(std::memory_order_relaxed);
        if (current_head == tail.load(std::memory_order_acquire)) {
            return nullptr; // queue is empty
        }
        return buffer[current_head].load(std::memory_order_acquire);
    }

    bool empty() const {
        return head.load(std::memory_order_acquire) == tail.load(std::memory_order_acquire);
    }

private:
    std::atomic<size_t> head;
    std::atomic<size_t> tail;
    std::atomic<size_t> size;
    size_t capacity;
    std::atomic<T*>* buffer;
};