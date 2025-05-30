#include <iostream>
#include <thread>
#include <atomic>
#include "gtest.h"
#include "dmformat.h"
#include "dmqueue.h"
#include "dmrapidpool.h"
const int gNumElements = 10000 * 10000;

const int gMaxPoolSize = 100000;

class PoolTest : public testing::Test {
protected:
    static const uint64_t kNum = gNumElements;
    static const uint64_t kMaxPoolSize = gMaxPoolSize;
    uint64_t expected_total;

    void SetUp() override {
        // 计算期望总和: sum = 1 + 2 + ... + (kNum - 1)
        expected_total = (kNum - 1) * kNum / 2;
    }
};


TEST_F(PoolTest, PoolTest) {
    CDMQueue q;
    q.Init(kMaxPoolSize);
    std::atomic<uint64_t> total{ 0 };

    auto str = DMNew<std::string>("hello world");

    q.PushBack(str);

    auto r = (std::string*)q.PopFront();

    fmt::print("{}\n", *r);
}