
// Copyright (c) 2018 brinkqiang (brink.qiang@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef __DMATOMIC_KFIFO_H_INCLUDE__
#define __DMATOMIC_KFIFO_H_INCLUDE__

#include <vector>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <atomic>
#include <cstdint>

class IAtomicKFifo {
public:
    virtual ~IAtomicKFifo() = default;
    virtual uint32_t put(const unsigned char* data, uint32_t len) = 0;
    virtual uint32_t get(unsigned char* data, uint32_t len) = 0;
    virtual uint32_t peek(unsigned char* data, uint32_t len) const = 0;
    virtual bool isEmpty() const = 0;
    virtual bool isFull() const = 0;
    virtual uint32_t len() const = 0;
    virtual uint32_t avail() const = 0;
    virtual uint32_t capacity() const = 0;
    virtual void reset() = 0;
};

class AtomicKFifo : public IAtomicKFifo {
private:
    std::vector<unsigned char> buffer_;
    uint32_t capacity_;
    uint32_t mask_;
    std::atomic<uint32_t> in_idx_;
    std::atomic<uint32_t> out_idx_;

    static uint32_t roundup_power_of_two(uint32_t v) {
        if (v == 0) return 0;
        uint32_t n = v - 1;
        n |= n >> 1;
        n |= n >> 2;
        n |= n >> 4;
        n |= n >> 8;
        n |= n >> 16;
        if (sizeof(uint32_t) > 4) { // Support for 64-bit uint32_t if applicable
            n |= n >> 32;
        }
        return n + 1;
    }

public:
    explicit AtomicKFifo(uint32_t requested_capacity) : in_idx_{ 0 }, out_idx_{ 0 } {
        if (requested_capacity == 0) {
            throw std::invalid_argument("KFifo capacity must be greater than 0.");
        }
        uint32_t cap = roundup_power_of_two(requested_capacity);
        if (cap < 2) {
            capacity_ = 2;
        }
        else {
            capacity_ = cap;
        }
        mask_ = capacity_ - 1;
        buffer_.resize(capacity_);
    }

    uint32_t put(const unsigned char* data, uint32_t len_to_write) override {
        auto current_in_val = in_idx_.load(std::memory_order_relaxed);
        auto current_out_val = out_idx_.load(std::memory_order_acquire);

        uint32_t current_length = current_in_val - current_out_val;
        uint32_t free_space = capacity_ - current_length;

        uint32_t actual_write_len = std::min(len_to_write, free_space);
        if (actual_write_len == 0) {
            return 0;
        }

        uint32_t offset_in_buffer = current_in_val & mask_;

        uint32_t l = std::min(actual_write_len, capacity_ - offset_in_buffer);
        std::memcpy(buffer_.data() + offset_in_buffer, data, l);
        if (actual_write_len > l) {
            std::memcpy(buffer_.data(), data + l, actual_write_len - l);
        }

        in_idx_.store(current_in_val + actual_write_len, std::memory_order_release);
        return actual_write_len;
    }

    uint32_t get(unsigned char* data, uint32_t len_to_read) override {
        auto current_out_val = out_idx_.load(std::memory_order_relaxed);
        auto current_in_val = in_idx_.load(std::memory_order_acquire);

        uint32_t current_data_len = current_in_val - current_out_val;
        uint32_t actual_read_len = std::min(len_to_read, current_data_len);

        if (actual_read_len == 0) {
            return 0;
        }

        uint32_t offset_in_buffer = current_out_val & mask_;

        uint32_t l = std::min(actual_read_len, capacity_ - offset_in_buffer);
        std::memcpy(data, buffer_.data() + offset_in_buffer, l);
        if (actual_read_len > l) {
            std::memcpy(data + l, buffer_.data(), actual_read_len - l);
        }

        out_idx_.store(current_out_val + actual_read_len, std::memory_order_release);
        return actual_read_len;
    }

    uint32_t peek(unsigned char* data, uint32_t len_to_peek) const override {
        auto current_out_val = out_idx_.load(std::memory_order_relaxed);
        auto current_in_val = in_idx_.load(std::memory_order_acquire);

        uint32_t current_data_len = current_in_val - current_out_val;
        uint32_t actual_peek_len = std::min(len_to_peek, current_data_len);

        if (actual_peek_len == 0) {
            return 0;
        }

        uint32_t offset_in_buffer = current_out_val & mask_;

        uint32_t l = std::min(actual_peek_len, capacity_ - offset_in_buffer);
        std::memcpy(data, buffer_.data() + offset_in_buffer, l);
        if (actual_peek_len > l) {
            std::memcpy(data + l, buffer_.data(), actual_peek_len - l);
        }

        return actual_peek_len;
    }

    bool isEmpty() const override {
        return in_idx_.load(std::memory_order_acquire) == out_idx_.load(std::memory_order_relaxed);
    }

    bool isFull() const override {
        auto current_in = in_idx_.load(std::memory_order_relaxed);
        auto current_out = out_idx_.load(std::memory_order_acquire);
        uint32_t current_length = current_in - current_out;
        return current_length == capacity_;
    }

    uint32_t len() const override { // Primarily for consumer context
        auto current_in = in_idx_.load(std::memory_order_acquire);
        auto current_out = out_idx_.load(std::memory_order_relaxed);
        return current_in - current_out;
    }

    uint32_t avail() const override { // Primarily for producer context
        auto current_in = in_idx_.load(std::memory_order_relaxed);
        auto current_out = out_idx_.load(std::memory_order_acquire);
        uint32_t current_length = current_in - current_out;
        return capacity_ - current_length;
    }

    uint32_t capacity() const override {
        return capacity_;
    }

    void reset() override {
        in_idx_.store(0, std::memory_order_release);
        out_idx_.store(0, std::memory_order_release);
    }
};

#endif // __DMATOMIC_KFIFO_H_INCLUDE__
