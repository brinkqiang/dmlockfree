#include <iostream>
#include <thread>
#include <atomic>
#include "gtest.h"
#include "dmqueue.h"
#include "dmformat.h"

const int kNumElements = 100000000;
const int kMaxQueueSize = 10000;

TEST(CDMQueue, BasicFunctionality) {

	fmt::print("Starting test for CDMQueue with {} elements...\n", kNumElements);

	CDMQueue queue;
	queue.Init(kMaxQueueSize);

	uint64_t actualTotal(0);
	uint64_t expectedTotal = 0;

	auto consumerThread = std::thread([&] {
		for (int i = 1; i < kNumElements;) {
			void* p = queue.PopFront();
			if (p == nullptr) {
				std::this_thread::yield();
				continue;
			}

			int value = reinterpret_cast<int>(p);
			actualTotal += value;
			++i;
		}
		});

	for (int i = 1; i < kNumElements;) {
		if (!queue.PushBack(reinterpret_cast<void*>(i))) {
			std::this_thread::yield();
			continue;
		}
		++i;
	}

	for (int i = 1; i < kNumElements; ++i) {
		expectedTotal += i;
	}

	consumerThread.join();

	fmt::print("Test completed.\nActual total   = {}\nExpected total = {}\n", actualTotal, expectedTotal);

	EXPECT_EQ(actualTotal, expectedTotal);
}