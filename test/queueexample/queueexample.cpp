#include <iostream>
#include <thread>
#include "gtest.h"
#include "dmatomic_queue.h"
#include "dmqueue.h"
#include "concurrentqueue.h"
#include "blockingconcurrentqueue.h"
#include "thread_safe_queue.h"
#include "atomic_queue.h"

#include "dmformat.h"

const int gNum = 100000000;
const int MaxPoolSize = 100000;

TEST(CDMAtomicQueue, CDMAtomicQueue)
{
	fmt::print("test CDMAtomicQueue {}\n", gNum);

    CDMAtomicQueue<int> q(MaxPoolSize);
	uint64_t total = 0;
    auto t = std::thread([&] {

        for (int i = 1; i < gNum;)
        {
            if (q.empty())
            {
				std::this_thread::yield();
                continue;
            }
			int *a = q.front();
            q.pop();

			total += *a;
            ++i;
        }
    });

    for (int i = 1; i < gNum;)
    {
		if (!q.try_push(i))
		{
			std::this_thread::yield();
			continue;
		}
		i++;
    }

    t.join();
	fmt::print("test CDMAtomicQueue Done total = {}\n", total);
}

TEST(CDMQueue, CDMQueue)
{
	fmt::print("test CDMQueue {}\n", gNum);

	CDMQueue q;
	q.Init(MaxPoolSize);
	uint64_t total = 0;
	auto t = std::thread([&] {
		for (int i = 1; i < gNum;)
		{
			void* p = q.PopFront();

			if (p == nullptr)
			{
				std::this_thread::yield();
				continue;
			}

			int a = reinterpret_cast<int>(p);

			total += a;
			++i;
		}
		});

	for (int i = 1; i < gNum; )
	{
		if (!q.PushBack(reinterpret_cast<void*>(i)))
		{
			std::this_thread::yield();
			continue;
		}
		i++;
	}

	t.join();
	fmt::print("test CDMQueue Done total = {}\n", total);
}

TEST(ConcurrentQueue, ConcurrentQueue)
{
	fmt::print("test ConcurrentQueue {}\n", gNum);
	moodycamel::ConcurrentQueue<int> q(MaxPoolSize);

	uint64_t total = 0;
	auto t = std::thread([&] {
		for (int i = 1; i < gNum;)
		{
			int a;
			if (!q.try_dequeue(a))
			{
				std::this_thread::yield();
				continue;
			}

			total += a;
			++i;
		}
		});

	for (int i = 1; i < gNum; )
	{
		if (!q.try_enqueue(i))
		{
			std::this_thread::yield();
			continue;
		}
		i++;
	}

	t.join();
	fmt::print("test ConcurrentQueue Done total = {}\n", total);
}

TEST(BlockingConcurrentQueue, BlockingConcurrentQueue)
{
	fmt::print("test BlockingConcurrentQueue {}\n", gNum);
	moodycamel::BlockingConcurrentQueue<int> q(MaxPoolSize);

	uint64_t total = 0;
	auto t = std::thread([&] {
		for (int i = 1; i < gNum;)
		{
			int a;
			if (!q.try_dequeue(a))
			{
				std::this_thread::yield();
				continue;
			}

			total += a;
			++i;
		}
		});

	for (int i = 1; i < gNum; )
	{
		if (!q.try_enqueue(i))
		{
			std::this_thread::yield();
			continue;
		}
		i++;
	}

	t.join();
	fmt::print("test BlockingConcurrentQueue Done total = {}\n", total);
}
TEST(ThreadSafeQueue, ThreadSafeQueue)
{
	fmt::print("test ThreadSafeQueue {}\n", gNum);
	ThreadSafeQueue<int> q;

	uint64_t total = 0;
	auto t = std::thread([&] {
		for (int i = 1; i < gNum;)
		{
			int a;
			if (!q.try_pop(a))
			{
				std::this_thread::yield();
				continue;
			}

			total += a;
			++i;
		}
		});

	for (int i = 1; i < gNum; )
	{
		q.push(i);
		i++;
	}

	t.join();
	fmt::print("test ThreadSafeQueue Done total = {}\n", total);
}

TEST(ThreadSafeQueue_condition, ThreadSafeQueue_condition)
{
	fmt::print("test ThreadSafeQueue {}\n", gNum);
	ThreadSafeQueue<int> q;

	uint64_t total = 0;
	auto t = std::thread([&] {
		for (int i = 1; i < gNum;)
		{
			int a;
			q.pop(a);
			total += a;
			++i;
		}
		});

	for (int i = 1; i < gNum; )
	{
		q.push(i);
		i++;
	}

	t.join();
	fmt::print("test ThreadSafeQueue Done total = {}\n", total);
}

TEST(CAtomicQueue, CAtomicQueue)
{
	fmt::print("test CAtomicQueue {}\n", gNum);
	CAtomicQueue<int> q(MaxPoolSize);

	uint64_t total = 0;
	auto t = std::thread([&] {
		for (int i = 1; i < gNum;)
		{
			if (q.empty())
			{
				std::this_thread::yield();
				continue;
			}
			int a = *(q.front());
			q.pop();
			total += a;
			++i;
		}
		});

	for (int i = 1; i < gNum; )
	{
		if (!q.try_push(i))
		{
			std::this_thread::yield();
			continue;
		}
		i++;
	}

	t.join();
	fmt::print("test CAtomicQueue Done total = {}\n", total);
}
