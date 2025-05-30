
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

#ifndef __DMKFIFO_H_INCLUDE__
#define __DMKFIFO_H_INCLUDE__

#include <vector>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <cstdint>

class IKFifo {
public:
    virtual ~IKFifo() = default;
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

class KFifo : public IKFifo {
private:
    std::vector<unsigned char> buffer_;
    uint32_t capacity_;
    uint32_t mask_;
    uint32_t in_idx_;
    uint32_t out_idx_;

    static uint32_t roundup_power_of_two(uint32_t v) {
        if (v == 0) return 0;
        uint32_t n = v - 1;
        n |= n >> 1;
        n |= n >> 2;
        n |= n >> 4;
        n |= n >> 8;
        n |= n >> 16;
        if (sizeof(uint32_t) > 4) {
             n |= n >> 32;
        }
        return n + 1;
    }

public:
    explicit KFifo(uint32_t requested_capacity) : in_idx_(0), out_idx_(0) {
        if (requested_capacity == 0) {
            throw std::invalid_argument("KFifo capacity must be greater than 0.");
        }
        uint32_t cap = roundup_power_of_two(requested_capacity);
        if (cap < 2) {
            capacity_ = 2;
        } else {
            capacity_ = cap;
        }
        mask_ = capacity_ - 1;
        buffer_.resize(capacity_);
    }

    uint32_t put(const unsigned char* data, uint32_t len) override {
        uint32_t free_space = avail();
        uint32_t write_len = std::min(len, free_space);
        if (write_len == 0) {
            return 0;
        }

        uint32_t offset = in_idx_ & mask_;
        uint32_t l = std::min(write_len, capacity_ - offset);

        std::memcpy(buffer_.data() + offset, data, l);
        if (write_len > l) {
            std::memcpy(buffer_.data(), data + l, write_len - l);
        }

        in_idx_ += write_len;
        return write_len;
    }

    uint32_t get(unsigned char* data, uint32_t len) override {
        uint32_t current_data_len = this->len();
        uint32_t read_len = std::min(len, current_data_len);
        if (read_len == 0) {
            return 0;
        }

        uint32_t offset = out_idx_ & mask_;
        uint32_t l = std::min(read_len, capacity_ - offset);

        std::memcpy(data, buffer_.data() + offset, l);
        if (read_len > l) {
            std::memcpy(data + l, buffer_.data(), read_len - l);
        }

        out_idx_ += read_len;
        return read_len;
    }

    uint32_t peek(unsigned char* data, uint32_t len) const override {
        uint32_t current_data_len = this->len();
        uint32_t peek_len = std::min(len, current_data_len);
        if (peek_len == 0) {
            return 0;
        }

        uint32_t offset = out_idx_ & mask_;
        uint32_t l = std::min(peek_len, capacity_ - offset);
        
        std::memcpy(data, buffer_.data() + offset, l);
        if (peek_len > l) {
            std::memcpy(data + l, buffer_.data(), peek_len - l);
        }
        
        return peek_len;
    }

    bool isEmpty() const override {
        return in_idx_ == out_idx_;
    }

    bool isFull() const override {
        return len() == capacity_;
    }

    uint32_t len() const override {
        return in_idx_ - out_idx_;
    }

    uint32_t avail() const override {
        return capacity_ - len();
    }

    uint32_t capacity() const override {
        return capacity_;
    }

    void reset() override {
        in_idx_ = 0;
        out_idx_ = 0;
    }
};


#endif // __DMKFIFO_H_INCLUDE__
