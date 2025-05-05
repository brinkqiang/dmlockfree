#include "atomic_queue.h"
#include "blockingconcurrentqueue.h"
#include "concurrentqueue.h"
#include "dmatomic_queue.h"
#include "dmqueue.h"
#include "gtest.h"
#include "thread_safe_queue.h"
#include <atomic>
#include <iostream>
#include <thread>

#include "dmformat.h"

const uint64_t gNum = 100000000;       // 测试数据量调整为百万级，便于快速验证
const uint64_t MaxPoolSize = 100000; // 根据实际需求调整

class QueueTest : public testing::Test {
protected:
    static const uint64_t kNum = gNum;
    static const uint64_t kMaxPoolSize = MaxPoolSize;
    uint64_t expected_total;

    void SetUp() override {
        // 计算期望总和: sum = 1 + 2 + ... + (kNum - 1)
        expected_total = (kNum - 1) * kNum / 2;
    }
};

TEST_F(QueueTest, CDMAtomicQueue) {
    CDMAtomicQueue<int> q(kMaxPoolSize);
    std::atomic<uint64_t> total{0};

    auto consumer = std::thread([&] {
        for (int i = 0; i < kNum - 1;) {
            if (q.empty()) {
                std::this_thread::yield();
                continue;
            }
            int* val = q.front();
            q.pop();
            total.fetch_add(*val, std::memory_order_relaxed);
            ++i;
        }
    });

    auto producer = std::thread([&] {
        for (int i = 1; i < kNum;) {
            if (q.try_push(i)) {
                ++i;
            } else {
                std::this_thread::yield();
            }
        }
    });

    producer.join();
    consumer.join();
    ASSERT_EQ(total.load(), expected_total);
}

TEST_F(QueueTest, CDMQueue) {
    CDMQueue q;
    q.Init(kMaxPoolSize);
    std::atomic<uint64_t> total{0};

    auto consumer = std::thread([&] {
        for (int i = 0; i < kNum - 1;) {
            void* p = q.PopFront();
            if (!p) {
                std::this_thread::yield();
                continue;
            }
            int val = static_cast<int>(reinterpret_cast<intptr_t>(p));
            total.fetch_add(val, std::memory_order_relaxed);
            ++i;
        }
    });

    auto producer = std::thread([&] {
        for (int i = 1; i < kNum;) {
            if (q.PushBack(reinterpret_cast<void*>(static_cast<intptr_t>(i)))) {
                ++i;
            } else {
                std::this_thread::yield();
            }
        }
    });

    producer.join();
    consumer.join();
    ASSERT_EQ(total.load(), expected_total);
}

TEST_F(QueueTest, ConcurrentQueue) {
    moodycamel::ConcurrentQueue<int> q(kMaxPoolSize);
    std::atomic<uint64_t> total{0};

    auto consumer = std::thread([&] {
        for (int i = 0; i < kNum - 1;) {
            int val;
            if (q.try_dequeue(val)) {
                total.fetch_add(val, std::memory_order_relaxed);
                ++i;
            } else {
                std::this_thread::yield();
            }
        }
    });

    auto producer = std::thread([&] {
        for (int i = 1; i < kNum;) {
            if (q.try_enqueue(i)) {
                ++i;
            } else {
                std::this_thread::yield();
            }
        }
    });

    producer.join();
    consumer.join();
    ASSERT_EQ(total.load(), expected_total);
}

TEST_F(QueueTest, BlockingConcurrentQueue) {
    moodycamel::BlockingConcurrentQueue<int> q(kMaxPoolSize);
    std::atomic<uint64_t> total{0};

    auto consumer = std::thread([&] {
        for (int i = 0; i < kNum - 1;) {
            int val;
            if (q.try_dequeue(val)) {
                total.fetch_add(val, std::memory_order_relaxed);
                ++i;
            } else {
                std::this_thread::yield();
            }
        }
    });

    auto producer = std::thread([&] {
        for (int i = 1; i < kNum;) {
            if (q.try_enqueue(i)) {
                ++i;
            } else {
                std::this_thread::yield();
            }
        }
    });

    producer.join();
    consumer.join();
    ASSERT_EQ(total.load(), expected_total);
}

TEST_F(QueueTest, ThreadSafeQueue) {
    ThreadSafeQueue<int> q;
    std::atomic<uint64_t> total{0};

    auto consumer = std::thread([&] {
        for (int i = 0; i < kNum - 1;) {
            int val;
            if (q.try_pop(val)) {
                total.fetch_add(val, std::memory_order_relaxed);
                ++i;
            } else {
                std::this_thread::yield();
            }
        }
    });

    auto producer = std::thread([&] {
        for (int i = 1; i < kNum; ++i) {
            q.push(i);
        }
    });

    producer.join();
    consumer.join();
    ASSERT_EQ(total.load(), expected_total);
}

TEST_F(QueueTest, ThreadSafeQueueCondition) {
    ThreadSafeQueue<int> q;
    std::atomic<uint64_t> total{0};

    auto consumer = std::thread([&] {
        for (int i = 0; i < kNum - 1;) {
            int val;
            q.pop(val);
            total.fetch_add(val, std::memory_order_relaxed);
            ++i;
        }
    });

    auto producer = std::thread([&] {
        for (int i = 1; i < kNum; ++i) {
            q.push(i);
        }
    });

    producer.join();
    consumer.join();
    ASSERT_EQ(total.load(), expected_total);
}

TEST_F(QueueTest, CAtomicQueue) {
    CAtomicQueue<int> q(kMaxPoolSize);
    std::atomic<uint64_t> total{0};

    auto producer = std::thread([&] {
        for (int i = 1; i < kNum;) {
            if (q.try_push(i)) {
                ++i;
            }
            else {
                std::this_thread::yield();
            }
        }
        });

    auto consumer = std::thread([&] {
        for (int i = 0; i < kNum - 1;) {
            if (q.empty()) {
                std::this_thread::yield();
                continue;
            }
            int val = *(q.front());
            q.pop();
            total.fetch_add(val, std::memory_order_relaxed);
            ++i;
        }
    });

    producer.join();
    consumer.join();
    ASSERT_EQ(total.load(), expected_total);
}