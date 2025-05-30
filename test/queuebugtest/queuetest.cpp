#include <iostream>
#include <thread>
#include <atomic>
#include "gtest.h"
#include "dmformat.h"
#include "dmkfifo.h"

const int kNumElements = 10000 * 10000;

const int kKfifoBufferIntCapacity = 100000;
// KFifo 需要字节容量
const unsigned int kKfifoBufferByteCapacity = kKfifoBufferIntCapacity * sizeof(int);

TEST(KFifoMultiThreaded, ProducerConsumerFunctionality) { // 测试套件和名称已修改

    // 使用 fmt::print，和您的示例一致
    fmt::print("Starting test for KFifo with {} elements (values 1 to {})...\n", kNumElements, kNumElements - 1);

    KFifo kfifo(kKfifoBufferByteCapacity); // 初始化 KFifo

    uint64_t actualTotal(0);      // 由消费者线程累加
    uint64_t expectedTotal = 0;   // 在主线程（生产者之后）计算

    auto consumerThread = std::thread([&] {
        // 消费者循环，处理 kNumElements-1 个元素
        for (int count = 1; count < kNumElements; ) {
            int value_read;
            // 尝试从队列获取一个 int 的字节
            if (kfifo.get(reinterpret_cast<unsigned char*>(&value_read), sizeof(int)) == sizeof(int)) {
                actualTotal += value_read;
                count++; // 成功读取一个元素，计数器增加
            }
            else {
                std::this_thread::yield(); // 队列空或数据不足一个int，让出CPU
            }
        }
        });

    // 生产者循环（在主线程中），推送 kNumElements-1 个元素
    for (int i = 1; i < kNumElements; ) {
        int value_to_push = i; // 要推送的值
        // 尝试向队列放入一个 int 的字节
        if (kfifo.put(reinterpret_cast<const unsigned char*>(&value_to_push), sizeof(int)) == sizeof(int)) {
            i++; // 成功放入一个元素，计数器增加
        }
        else {
            std::this_thread::yield(); // 队列满，让出CPU
        }
    }

    // 所有元素都已推送到队列后，计算期望的总和
    // 这个循环也对应 kNumElements-1 个元素
    for (int i = 1; i < kNumElements; ++i) {
        expectedTotal += i;
    }

    consumerThread.join(); // 等待消费者线程完成

    fmt::print("Test completed.\nActual total   = {}\nExpected total = {}\n", actualTotal, expectedTotal);

    EXPECT_EQ(actualTotal, expectedTotal);
}
